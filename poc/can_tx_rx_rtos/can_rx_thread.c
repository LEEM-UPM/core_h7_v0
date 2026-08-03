#include "can_rx_thread.h"
#include "can_protocol.h"
#include "can_shared_data.h"
#include "tx_api.h"
#include "usart.h"

#include <string.h>

static uint32_t _fdcan_dlc_to_bytes(uint32_t dlc) {
  switch (dlc) {
  case FDCAN_DLC_BYTES_0:
    return 0U;
  case FDCAN_DLC_BYTES_1:
    return 1U;
  case FDCAN_DLC_BYTES_2:
    return 2U;
  case FDCAN_DLC_BYTES_3:
    return 3U;
  case FDCAN_DLC_BYTES_4:
    return 4U;
  case FDCAN_DLC_BYTES_5:
    return 5U;
  case FDCAN_DLC_BYTES_6:
    return 6U;
  case FDCAN_DLC_BYTES_7:
    return 7U;
  case FDCAN_DLC_BYTES_8:
    return 8U;
  case FDCAN_DLC_BYTES_12:
    return 12U;
  case FDCAN_DLC_BYTES_16:
    return 16U;
  case FDCAN_DLC_BYTES_20:
    return 20U;
  case FDCAN_DLC_BYTES_24:
    return 24U;
  case FDCAN_DLC_BYTES_32:
    return 32U;
  case FDCAN_DLC_BYTES_48:
    return 48U;
  case FDCAN_DLC_BYTES_64:
    return 64U;
  default:
    return 0U;
  }
}

/*
 * Ring buffer SPSC
 *
 * Los índices son uint32_t sin límite superior explícito: se dejan
 * dar vuelta naturalmente (wrap-around en 2^32). El enmascaramiento
 * con CAN_RX_RB_MASK convierte cualquier índice en una posición
 * válida dentro del array. Esto es correcto mientras la profundidad
 * sea potencia de 2, que es un requisito documentado en el .h.
 *
 * _rb_head: escrito SOLO por la ISR.
 * _rb_tail: escrito SOLO por el hilo consumidor.
 * Ambos se declaran volatile para impedir que el compilador cachee
 * su valor en un registro entre accesos.
 */
static can_rx_frame_t _rb[CAN_RX_RB_DEPTH];
static volatile uint32_t _rb_head = 0U; /* próximo slot a escribir */
static volatile uint32_t _rb_tail = 0U; /* próximo slot a leer     */

static TX_SEMAPHORE _can_rx_sem;
static TX_THREAD _can_rx_thread;
static FDCAN_HandleTypeDef *_hfdcan_rx = NULL;

static volatile uint32_t _overflows = 0U;

static void _can_rx_thread_entry(ULONG thread_input);
static void _can_parse_frame(const can_rx_frame_t *frame);

UINT can_rx_thread_create(TX_BYTE_POOL *byte_pool,
                          FDCAN_HandleTypeDef *hfdcan) {
  UINT status;
  CHAR *stack_ptr;

  _hfdcan_rx = hfdcan;

  status = tx_semaphore_create(&_can_rx_sem, "CAN RX sem", 0U);
  if (status != TX_SUCCESS) {
    return status;
  }

  /* Primero todo el software que va a consumir los datos, después el hardware
   * que los produce */
  status = tx_byte_allocate(byte_pool, (VOID **)&stack_ptr,
                            CAN_RX_THREAD_STACK_SIZE, TX_NO_WAIT);
  if (status != TX_SUCCESS) {
    return status;
  }

  tx_thread_create(&_can_rx_thread, "CAN RX thread", _can_rx_thread_entry, 0U,
                   stack_ptr, CAN_RX_THREAD_STACK_SIZE, CAN_RX_THREAD_PRIO,
                   CAN_RX_THREAD_PREEMPT_THR, TX_NO_TIME_SLICE, TX_AUTO_START);

  /*
   * Filtro FDCAN
   *
   * Acepta IDs 0x080–0x083 (los cuatro mensajes de la placa Sensores)
   * usando máscara de bits:
   *   (ID_rx & 0x7FC) == (0x080 & 0x7FC)
   *   → bits 10..2 fijos, bits 1..0 libres: acepta 080,081,082,083
   *   → 0x084 no pasa porque cambia el bit 2.
   */
  FDCAN_FilterTypeDef filter = {
      .IdType = FDCAN_STANDARD_ID,
      .FilterIndex = 0,
      .FilterType = FDCAN_FILTER_MASK,
      .FilterConfig = FDCAN_FILTER_TO_RXFIFO0,
      .FilterID1 = CAN_ID_IMU_HG, /* 0x080 — valor base */
      .FilterID2 = 0x7FCU         /* máscara: fija bits 10..2 */
  };

  if (HAL_FDCAN_ConfigFilter(hfdcan, &filter) != HAL_OK) {
    return TX_START_ERROR;
  }

  if (HAL_FDCAN_ActivateNotification(hfdcan, FDCAN_IT_RX_FIFO0_NEW_MESSAGE,
                                     0) != HAL_OK) {
    return TX_START_ERROR;
  }

  if (HAL_FDCAN_Start(hfdcan) != HAL_OK) {
    return TX_START_ERROR;
  }

  return TX_SUCCESS;
}

/*
 * Requisitos de diseño para una ISR correcta:
 *   · Sin bloqueos (no llamar a funciones que esperen).
 *   · Sin asignación dinámica de memoria.
 *   · Mínimo tiempo de CPU.
 */
void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan,
                               uint32_t RxFifo0ITs) {
  if (hfdcan->Instance != _hfdcan_rx->Instance) {
    return;
  }
  if (!(RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE)) {
    return;
  }

  FDCAN_RxHeaderTypeDef rx_header;

  /*
   * Verificar si hay espacio en el ring buffer ANTES de leer del HW.
   * Si el buffer está lleno, aun así leemos la trama para vaciar el
   * FIFO del periférico (evitar que el HW deje de generar interrupciones),
   * pero descartamos el dato y contamos el overflow.
   */
  uint32_t head = _rb_head;
  uint32_t tail = _rb_tail;
  uint32_t used = head - tail;

  if (used >= CAN_RX_RB_DEPTH) {
    /*
     * Buffer lleno: leer igualmente para drenar el FIFO HW.
     * Se usa un frame temporal en el stack (72 bytes: aceptable en ISR
     * dado que el stack de interrupción de Cortex-M es suficiente).
     */
    can_rx_frame_t dummy;
    HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &rx_header, dummy.data);
    _overflows++;
    return;
  }

  can_rx_frame_t *slot = &_rb[head & CAN_RX_RB_MASK];

  if (HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &rx_header, slot->data) !=
      HAL_OK) {
    return;
  }

  slot->id = rx_header.Identifier;
  slot->dlc_bytes = _fdcan_dlc_to_bytes(rx_header.DataLength);

  /*
   * Barrera de escritura (Cortex-M4/M7 con caché o reordenamiento
   * del compilador). Asegura que id y dlc_bytes sean visibles al
   * hilo consumidor antes de que _rb_head sea incrementado.
   *
   * En Cortex-M con caché desactivada (caso típico de SRAM interna)
   * __DMB() es suficiente. Si la SRAM está cacheada, añadir
   * SCB_CleanDCache_by_Addr() sobre el slot antes del DMB.
   */
  __DMB();

  _rb_head = head + 1U;

  tx_semaphore_put(&_can_rx_sem);
}

static void _can_rx_thread_entry(ULONG thread_input) {
  (void)thread_input;

  for (;;) {
    UINT status = tx_semaphore_get(&_can_rx_sem, TX_WAIT_FOREVER);
    if (status != TX_SUCCESS) {
      continue;
    }

    while (_rb_tail != _rb_head) {
      /*
       * Barrera de lectura: asegura que leemos los datos del slot
       * DESPUÉS de haber visto el incremento de _rb_head.
       */
      __DMB();

      _can_parse_frame(&_rb[_rb_tail & CAN_RX_RB_MASK]);

      _rb_tail++;
    }
  }
}

static void _can_parse_frame(const can_rx_frame_t *frame) {
  float ts = (float)tx_time_get() * 0.001f;

  switch (frame->id) {
  case CAN_ID_IMU_HG: {
    if (frame->dlc_bytes != sizeof(can_payload_imu_t))
      break;
    can_payload_imu_t p;
    can_deserialize_imu(frame->data, &p);

    if (can_shared_data_lock() != TX_SUCCESS)
      break;
    imu_data_t *d = &g_can_shared.imu_hg;
    d->accel_mps2[0] = p.acc[0];
    d->accel_mps2[1] = p.acc[1];
    d->accel_mps2[2] = p.acc[2];
    d->gyro_rps[0] = p.omega[0];
    d->gyro_rps[1] = p.omega[1];
    d->gyro_rps[2] = p.omega[2];
    d->timestamp_s = ts;
    d->data_valid = 1U;
    g_can_shared.new_data_flags |= CAN_NEW_DATA_IMU_HG;
    can_shared_data_unlock();
    break;
  }

  case CAN_ID_IMU_LG: {
    if (frame->dlc_bytes != sizeof(can_payload_imu_t))
      break;
    can_payload_imu_t p;
    can_deserialize_imu(frame->data, &p);

    if (can_shared_data_lock() != TX_SUCCESS)
      break;
    imu_data_t *d = &g_can_shared.imu_lg;
    d->accel_mps2[0] = p.acc[0];
    d->accel_mps2[1] = p.acc[1];
    d->accel_mps2[2] = p.acc[2];
    d->gyro_rps[0] = p.omega[0];
    d->gyro_rps[1] = p.omega[1];
    d->gyro_rps[2] = p.omega[2];
    d->timestamp_s = ts;
    d->data_valid = 1U;
    g_can_shared.new_data_flags |= CAN_NEW_DATA_IMU_LG;
    can_shared_data_unlock();
    break;
  }

  case CAN_ID_BARO: {
    if (frame->dlc_bytes != sizeof(can_payload_baro_t))
      break;
    can_payload_baro_t p;
    can_deserialize_baro(frame->data, &p);

    if (can_shared_data_lock() != TX_SUCCESS)
      break;
    baro_data_t *d = &g_can_shared.baro;
    d->pressure_pa = p.pressure_pa;
    d->data_valid = 1U;
    g_can_shared.new_data_flags |= CAN_NEW_DATA_BARO;
    can_shared_data_unlock();
    break;
  }

  case CAN_ID_GPS: {
    if (frame->dlc_bytes != sizeof(can_payload_gps_t))
      break;
    can_payload_gps_t p;
    can_deserialize_gps(frame->data, &p);

    if (can_shared_data_lock() != TX_SUCCESS)
      break;
    gps_data_t *d = &g_can_shared.gps;
    d->lat_deg = p.lat_deg;
    d->lon_deg = p.lon_deg;
    d->alt_m = p.alt_m;
    d->vel_ned[0] = p.vel_ned[0];
    d->vel_ned[1] = p.vel_ned[1];
    d->vel_ned[2] = p.vel_ned[2];
    d->fix = p.fix;
    d->data_valid = 1U;
    g_can_shared.new_data_flags |= CAN_NEW_DATA_GPS;
    can_shared_data_unlock();
    break;
  }

  default:
    break;
  }
}

uint32_t can_rx_get_overflows(void) { return _overflows; }