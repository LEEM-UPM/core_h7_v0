#include "fdcan.h"
#include "gpio.h"
#include "main.h"
#include "sensor_board.h"
#include "stm32h7xx_hal_gpio.h"
#include "sys_utilities.h"

extern void SystemClock_Config(void);
extern void PeriphCommonClock_Config(void);

/* Profundidad del ring buffer de recepción. Debe ser potencia de 2. */
#define CAN2_RX_RB_DEPTH 32U
#define CAN2_RX_RB_MASK (CAN2_RX_RB_DEPTH - 1U)

/* Periodo del LED de actividad: parpadea mientras lleguen tramas válidas */
#define LED_ACTIVITY_PERIOD_MS 250U

/*
 * Trama de prueba de 1 byte por CAN2 a 50 Hz.
 * ID por encima de 0x101–0x105: en el arbitraje ganan siempre las tramas de
 * la placa de sensores, que no retransmite y las perdería.
 */
#define CAN2_TX_TEST_ID 0x200U
#define CAN2_TX_PERIOD_MS 20U        /* 50 Hz */
#define CAN2_TX_VALUE_PERIOD_MS 1000U /* alterna 0 <-> 255 cada segundo */

typedef struct {
  uint32_t id;
  uint32_t dlc_bytes;
  uint32_t rx_tick_ms; /* solo para detectar resets de la placa */
  /*
   * 64 bytes aunque el elemento de la RX FIFO0 sea de 32: HAL copia tantos
   * bytes como indique el DLC, así que una trama de 48/64 bytes desbordaría
   * un buffer de 32. Solo los primeros 32 bytes son válidos en ese caso.
   */
  uint8_t data[64];
} can_rx_frame_t;

/*
 * Ring buffer SPSC entre la ISR y el bucle principal.
 * _rb_head: escrito SOLO por la ISR.
 * _rb_tail: escrito SOLO por el bucle principal.
 */
static can_rx_frame_t _rb[CAN2_RX_RB_DEPTH];
static volatile uint32_t _rb_head = 0U;
static volatile uint32_t _rb_tail = 0U;

static volatile uint32_t _rb_overflows = 0U;   /* ring buffer lleno */
static volatile uint32_t _fifo_msg_lost = 0U; /* FIFO0 HW desbordada */

static uint32_t _tx_errors = 0U; /* cola TX llena o error al encolar */

static void _can2_rx_init(void);
static void _can2_send_byte(uint8_t value);
static uint32_t _fdcan_dlc_to_bytes(uint32_t dlc);

int main(void) {
  HAL_Init();
  SystemClock_Config();
  PeriphCommonClock_Config();
  MX_GPIO_Init();
  MX_FDCAN2_Init();

  /* TCAN1044: STB = LOW -> modo normal */
  HAL_GPIO_WritePin(CAN2_STB_GPIO_Port, CAN2_STB_Pin, GPIO_PIN_RESET);

  _can2_rx_init();

  uint32_t valid_frames = 0U;
  uint32_t led_last_frames = 0U;
  uint32_t led_last_tick = HAL_GetTick();

  uint8_t tx_value = 0U;
  uint32_t tx_last_tick = HAL_GetTick();
  uint32_t tx_value_last_tick = tx_last_tick;

  while (1) {
    while (_rb_tail != _rb_head) {
      const can_rx_frame_t *frame = &_rb[_rb_tail & CAN2_RX_RB_MASK];

      valid_frames += sensor_board_process_frame(
          frame->id, frame->data, frame->dlc_bytes, frame->rx_tick_ms);

      _rb_tail++;
    }

    /* TODO: usar sensor_board_get_data() según lo que haya que hacer con los
     * datos (log, reenvío...) */

    uint32_t now = HAL_GetTick();

    if ((now - tx_value_last_tick) >= CAN2_TX_VALUE_PERIOD_MS) {
      tx_value_last_tick += CAN2_TX_VALUE_PERIOD_MS;
      tx_value = (tx_value == 0U) ? 255U : 0U;
    }

    if ((now - tx_last_tick) >= CAN2_TX_PERIOD_MS) {
      tx_last_tick += CAN2_TX_PERIOD_MS;
      _can2_send_byte(tx_value);
    }

    if ((now - led_last_tick) >= LED_ACTIVITY_PERIOD_MS) {
      led_last_tick = now;
      if (valid_frames != led_last_frames) {
        led_last_frames = valid_frames;
        HAL_GPIO_TogglePin(LED_1_GPIO_Port, LED_1_Pin);
      } else {
        HAL_GPIO_WritePin(LED_1_GPIO_Port, LED_1_Pin, GPIO_PIN_RESET);
      }
    }
  }
}

static void _can2_rx_init(void) {
  /* Solo los IDs estándar de la placa de sensores: 0x101–0x105 */
  FDCAN_FilterTypeDef filter = {.IdType = FDCAN_STANDARD_ID,
                                .FilterIndex = 0,
                                .FilterType = FDCAN_FILTER_RANGE,
                                .FilterConfig = FDCAN_FILTER_TO_RXFIFO0,
                                .FilterID1 = SENSOR_BOARD_CAN_ID_FIRST,
                                .FilterID2 = SENSOR_BOARD_CAN_ID_LAST};

  if (HAL_FDCAN_ConfigFilter(&hfdcan2, &filter) != HAL_OK) {
    Error_Handler();
  }

  /* Todo lo demás (otros IDs, IDs extendidos, tramas remotas) se rechaza */
  if (HAL_FDCAN_ConfigGlobalFilter(&hfdcan2, FDCAN_REJECT, FDCAN_REJECT,
                                   FDCAN_REJECT_REMOTE,
                                   FDCAN_REJECT_REMOTE) != HAL_OK) {
    Error_Handler();
  }

  if (HAL_FDCAN_ActivateNotification(&hfdcan2,
                                     FDCAN_IT_RX_FIFO0_NEW_MESSAGE |
                                         FDCAN_IT_RX_FIFO0_MESSAGE_LOST,
                                     0) != HAL_OK) {
    Error_Handler();
  }

  if (HAL_FDCAN_Start(&hfdcan2) != HAL_OK) {
    Error_Handler();
  }
}

static void _can2_send_byte(uint8_t value) {
  FDCAN_TxHeaderTypeDef hdr = {.Identifier = CAN2_TX_TEST_ID,
                               .IdType = FDCAN_STANDARD_ID,
                               .TxFrameType = FDCAN_DATA_FRAME,
                               .DataLength = FDCAN_DLC_BYTES_1,
                               .ErrorStateIndicator = FDCAN_ESI_ACTIVE,
                               .BitRateSwitch = FDCAN_BRS_OFF,
                               .FDFormat = FDCAN_FD_CAN,
                               .TxEventFifoControl = FDCAN_NO_TX_EVENTS,
                               .MessageMarker = 0U};

  if (HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan2, &hdr, &value) != HAL_OK) {
    _tx_errors++;
  }
}

/*
 * ISR: sin bloqueos y con el mínimo trabajo posible.
 * Se vacía la FIFO0 entera porque si llegan varias tramas antes de atender
 * la interrupción solo se genera un flag de "new message".
 */
void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan,
                               uint32_t RxFifo0ITs) {
  if (hfdcan->Instance != FDCAN2) {
    return;
  }

  if (RxFifo0ITs & FDCAN_IT_RX_FIFO0_MESSAGE_LOST) {
    _fifo_msg_lost++;
  }

  if (!(RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE)) {
    return;
  }

  FDCAN_RxHeaderTypeDef rx_header;
  uint32_t now = HAL_GetTick();

  while (HAL_FDCAN_GetRxFifoFillLevel(hfdcan, FDCAN_RX_FIFO0) > 0U) {
    uint32_t head = _rb_head;

    if ((head - _rb_tail) >= CAN2_RX_RB_DEPTH) {
      /* Buffer lleno: se lee igualmente para drenar la FIFO HW */
      can_rx_frame_t dummy;
      HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &rx_header, dummy.data);
      _rb_overflows++;
      continue;
    }

    can_rx_frame_t *slot = &_rb[head & CAN2_RX_RB_MASK];

    if (HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &rx_header,
                               slot->data) != HAL_OK) {
      break;
    }

    slot->id = rx_header.Identifier;
    slot->dlc_bytes = _fdcan_dlc_to_bytes(rx_header.DataLength);
    slot->rx_tick_ms = now;

    __DMB();

    _rb_head = head + 1U;
  }
}

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
