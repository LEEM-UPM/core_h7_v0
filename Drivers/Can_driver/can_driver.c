#include "can_driver.h"

#include <string.h>

#include "can_protocol/can_ids.h"
#include "fdcan.h"
#include "main.h"
#include "sensor_board.h"

/* Profundidad del ring buffer de recepción de cada puerto. Potencia de 2. */
#define RX_RB_DEPTH 32U
#define RX_RB_MASK (RX_RB_DEPTH - 1U)

#define MAX_FILTERS 3U

typedef struct {
  uint32_t type; /* FDCAN_FILTER_MASK / FDCAN_FILTER_RANGE ... */
  uint32_t id1;
  uint32_t id2;
} can_filter_t;

typedef struct {
  /* Configuración */
  FDCAN_HandleTypeDef *hfdcan;
  GPIO_TypeDef *stb_port;
  uint16_t stb_pin;
  uint32_t tx_max_bytes; /* debe coincidir con TxElmtSize del .ioc */
  can_filter_t filters[MAX_FILTERS]; /* tantos como StdFiltersNbr del .ioc */
  uint32_t filter_count;

  /*
   * Ring buffer SPSC entre la ISR y el bucle principal.
   * rb_head: escrito SOLO por la ISR.
   * rb_tail: escrito SOLO por el bucle principal.
   */
  can_rx_frame_t rb[RX_RB_DEPTH];
  volatile uint32_t rb_head;
  volatile uint32_t rb_tail;

  volatile can_driver_stats_t stats;
} can_port_ctx_t;

static can_port_ctx_t _ports[CAN_PORT_COUNT] = {
    [CAN_PORT_1] =
        {
            .hfdcan = &hfdcan1,
            .stb_port = CAN1_STB_GPIO_Port,
            .stb_pin = CAN1_STB_Pin,
            .tx_max_bytes = 32U,
            /*
             * El core recibe todo lo que le puede interesar del bus (ICD, 5.3):
             * - clase 0 (críticos):                0x000–0x0FF
             * - clases 2 y 3 (respuestas, telem.): 0x200–0x3FF
             * - clases 6 y 7 (heartbeat, debug):   0x600–0x7FF
             * Los comandos (clase 1) no: el core es el único que los envía.
             */
            .filters =
                {
                    {FDCAN_FILTER_MASK, CAN_ID(CAN_CLASS_CRITICAL, 0U, 0U),
                     CAN_ID_CLASS_MASK},
                    {FDCAN_FILTER_MASK, CAN_ID(CAN_CLASS_RESPONSE, 0U, 0U),
                     CAN_ID(0x6U, 0U, 0U)},
                    {FDCAN_FILTER_MASK, CAN_ID(CAN_CLASS_HEARTBEAT, 0U, 0U),
                     CAN_ID(0x6U, 0U, 0U)},
                },
            .filter_count = 3U,
        },
    [CAN_PORT_2] =
        {
            .hfdcan = &hfdcan2,
            .stb_port = CAN2_STB_GPIO_Port,
            .stb_pin = CAN2_STB_Pin,
            .tx_max_bytes = 8U,
            /* Solo los IDs de la placa de sensores: 0x101–0x105 */
            .filters =
                {
                    {FDCAN_FILTER_RANGE, SENSOR_BOARD_CAN_ID_FIRST,
                     SENSOR_BOARD_CAN_ID_LAST},
                },
            .filter_count = 1U,
        },
};

/* Bytes de datos de cada código DLC (en esta HAL, FDCAN_DLC_BYTES_x = 0..15) */
static const uint8_t _dlc_to_bytes[16] = {0,  1,  2,  3,  4,  5,  6,  7,
                                          8,  12, 16, 20, 24, 32, 48, 64};

static void _port_init(can_port_ctx_t *ctx);
static void _rx_isr(can_port_ctx_t *ctx, uint32_t rx_fifo0_its);

void can_driver_init(void) {
  for (uint32_t i = 0U; i < (uint32_t)CAN_PORT_COUNT; i++) {
    _port_init(&_ports[i]);
  }
}

bool can_driver_send(can_port_t port, uint32_t id, const uint8_t *data,
                     uint32_t len) {
  if ((uint32_t)port >= (uint32_t)CAN_PORT_COUNT) {
    return false;
  }
  can_port_ctx_t *ctx = &_ports[port];

  if (len > ctx->tx_max_bytes) {
    ctx->stats.tx_errors++;
    return false;
  }

  /* Menor DLC que quepa len; el relleno va a 0 */
  uint32_t dlc = 0U;
  while (_dlc_to_bytes[dlc] < len) {
    dlc++;
  }

  uint8_t buf[64] = {0};
  if (len > 0U) {
    memcpy(buf, data, len);
  }

  FDCAN_TxHeaderTypeDef hdr = {.Identifier = id,
                               .IdType = FDCAN_STANDARD_ID,
                               .TxFrameType = FDCAN_DATA_FRAME,
                               .DataLength = dlc,
                               .ErrorStateIndicator = FDCAN_ESI_ACTIVE,
                               .BitRateSwitch = FDCAN_BRS_OFF,
                               .FDFormat = FDCAN_FD_CAN,
                               .TxEventFifoControl = FDCAN_NO_TX_EVENTS,
                               .MessageMarker = 0U};

  if (HAL_FDCAN_AddMessageToTxFifoQ(ctx->hfdcan, &hdr, buf) != HAL_OK) {
    ctx->stats.tx_errors++;
    return false;
  }
  return true;
}

bool can_driver_receive(can_port_t port, can_rx_frame_t *frame) {
  if ((uint32_t)port >= (uint32_t)CAN_PORT_COUNT) {
    return false;
  }
  can_port_ctx_t *ctx = &_ports[port];
  uint32_t tail = ctx->rb_tail;

  if (tail == ctx->rb_head) {
    return false;
  }

  *frame = ctx->rb[tail & RX_RB_MASK];
  ctx->rb_tail = tail + 1U;
  return true;
}

void can_driver_get_stats(can_port_t port, can_driver_stats_t *stats) {
  if ((uint32_t)port >= (uint32_t)CAN_PORT_COUNT) {
    return;
  }
  *stats = _ports[port].stats;
}

static void _port_init(can_port_ctx_t *ctx) {
  /* TCAN1044: STB = LOW -> modo normal */
  HAL_GPIO_WritePin(ctx->stb_port, ctx->stb_pin, GPIO_PIN_RESET);

  for (uint32_t i = 0U; i < ctx->filter_count; i++) {
    FDCAN_FilterTypeDef filter = {.IdType = FDCAN_STANDARD_ID,
                                  .FilterIndex = i,
                                  .FilterType = ctx->filters[i].type,
                                  .FilterConfig = FDCAN_FILTER_TO_RXFIFO0,
                                  .FilterID1 = ctx->filters[i].id1,
                                  .FilterID2 = ctx->filters[i].id2};

    if (HAL_FDCAN_ConfigFilter(ctx->hfdcan, &filter) != HAL_OK) {
      Error_Handler();
    }
  }

  /* Todo lo demás (otros IDs, IDs extendidos, tramas remotas) se rechaza */
  if (HAL_FDCAN_ConfigGlobalFilter(ctx->hfdcan, FDCAN_REJECT, FDCAN_REJECT,
                                   FDCAN_REJECT_REMOTE,
                                   FDCAN_REJECT_REMOTE) != HAL_OK) {
    Error_Handler();
  }

  if (HAL_FDCAN_ActivateNotification(ctx->hfdcan,
                                     FDCAN_IT_RX_FIFO0_NEW_MESSAGE |
                                         FDCAN_IT_RX_FIFO0_MESSAGE_LOST,
                                     0U) != HAL_OK) {
    Error_Handler();
  }

  if (HAL_FDCAN_Start(ctx->hfdcan) != HAL_OK) {
    Error_Handler();
  }
}

/*
 * Callback de HAL común a los dos periféricos: se reparte por instancia.
 */
void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan,
                               uint32_t RxFifo0ITs) {
  for (uint32_t i = 0U; i < (uint32_t)CAN_PORT_COUNT; i++) {
    if (_ports[i].hfdcan == hfdcan) {
      _rx_isr(&_ports[i], RxFifo0ITs);
      return;
    }
  }
}

/*
 * ISR: sin bloqueos y con el mínimo trabajo posible.
 * Se vacía la FIFO0 entera porque si llegan varias tramas antes de atender
 * la interrupción solo se genera un flag de "new message".
 */
static void _rx_isr(can_port_ctx_t *ctx, uint32_t rx_fifo0_its) {
  if (rx_fifo0_its & FDCAN_IT_RX_FIFO0_MESSAGE_LOST) {
    ctx->stats.fifo_msg_lost++;
  }

  if (!(rx_fifo0_its & FDCAN_IT_RX_FIFO0_NEW_MESSAGE)) {
    return;
  }

  FDCAN_RxHeaderTypeDef rx_header;
  uint32_t now = HAL_GetTick();

  while (HAL_FDCAN_GetRxFifoFillLevel(ctx->hfdcan, FDCAN_RX_FIFO0) > 0U) {
    uint32_t head = ctx->rb_head;

    if ((head - ctx->rb_tail) >= RX_RB_DEPTH) {
      /* Buffer lleno: se lee igualmente para drenar la FIFO HW */
      uint8_t dummy[64];
      HAL_FDCAN_GetRxMessage(ctx->hfdcan, FDCAN_RX_FIFO0, &rx_header, dummy);
      ctx->stats.rx_overflows++;
      continue;
    }

    can_rx_frame_t *slot = &ctx->rb[head & RX_RB_MASK];

    if (HAL_FDCAN_GetRxMessage(ctx->hfdcan, FDCAN_RX_FIFO0, &rx_header,
                               slot->data) != HAL_OK) {
      break;
    }

    slot->id = rx_header.Identifier;
    slot->len = _dlc_to_bytes[rx_header.DataLength & 0xFU];
    slot->rx_tick_ms = now;

    __DMB();

    ctx->rb_head = head + 1U;
    ctx->stats.rx_frames++;
  }
}
