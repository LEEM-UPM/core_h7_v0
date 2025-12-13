#include "fdcan.h"
#include "gpio.h"
#include "stm32h7xx_hal_def.h"
#include "usart.h"
#include <stdbool.h>
#include <string.h>

extern void SystemClock_Config(void);
extern void PeriphCommonClock_Config(void);

/* ============================================================
 * EXAMPLE CONFIGURATION
 * ============================================================ */
#define TEST_CAN_ID 0x123
#define TEST_PAYLOAD 0xA5
#define TX_PERIOD_MS 200

/* ============================================================
 * VARIABLES SHARED BETWEEN IRQ AND MAIN CONTEXT
 * ============================================================ */

/* Simple flag indicating that a new CAN frame has been received */
static volatile bool fdcan_rx_pending = false;

/* Buffer where the IRQ copies the last received CAN frame */
static FDCAN_RxHeaderTypeDef rx_header;
static uint8_t rx_data[8];

/* ============================================================
 * INTERNAL VARIABLES
 * ============================================================ */
static uint32_t last_tx_tick = 0;

/* ============================================================
 * PROTOTYPES
 * ============================================================ */
static void fdcan_core_init(void);
static void fdcan_send_test_frame(void);
static void fdcan_process_rx(void);
static void log_uart(const char *msg);

/* ============================================================
 * MAIN
 * ============================================================ */
int main(void) {
  HAL_Init();
  SystemClock_Config();
  PeriphCommonClock_Config();

  MX_GPIO_Init();
  MX_UART5_Init();
  MX_FDCAN1_Init();
  MX_FDCAN2_Init();

  // Enablement PIN to use the UART corresponding to camera 3.
  HAL_GPIO_WritePin(CAM_EN_GPIO_Port, CAM_EN_Pin, GPIO_PIN_SET);

  /* ------------------------------------------------------------
   * TCAN1044:
   * STB = LOW -> Normal mode (CAN enabled)
   * ------------------------------------------------------------ */
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_3 | GPIO_PIN_4, GPIO_PIN_RESET);

  fdcan_core_init();

  log_uart("\r\n--- FDCAN CORE EXAMPLE ---\r\n");

  while (1) {
    uint32_t now = HAL_GetTick();

    /* Periodic CAN frame transmission */
    if ((now - last_tx_tick) >= TX_PERIOD_MS) {
      last_tx_tick = now;
      fdcan_send_test_frame();
      log_uart(">");
    }

    /* Deferred processing of the received CAN frame */
    if (fdcan_rx_pending) {
      fdcan_process_rx();
    }
  }
}

static void fdcan_core_init(void) {
  /* Accept any standard ID and route it to RX FIFO0 */
  FDCAN_FilterTypeDef filter = {.IdType = FDCAN_STANDARD_ID,
                                .FilterIndex = 0,
                                .FilterType = FDCAN_FILTER_MASK,
                                .FilterConfig = FDCAN_FILTER_TO_RXFIFO0,
                                .FilterID1 = 0x000,
                                .FilterID2 = 0x000};

  HAL_FDCAN_ConfigFilter(&hfdcan2, &filter);

  /* Enable interrupt on new message in RX FIFO0 */
  HAL_FDCAN_ActivateNotification(&hfdcan2, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0);

  HAL_FDCAN_Start(&hfdcan1);
  HAL_FDCAN_Start(&hfdcan2);
}

static void fdcan_send_test_frame(void) {
  FDCAN_TxHeaderTypeDef hdr = {.Identifier = TEST_CAN_ID,
                               .IdType = FDCAN_STANDARD_ID,
                               .TxFrameType = FDCAN_DATA_FRAME,
                               .DataLength = FDCAN_DLC_BYTES_1,
                               .ErrorStateIndicator = FDCAN_ESI_ACTIVE,
                               .BitRateSwitch = FDCAN_BRS_OFF,
                               .FDFormat = FDCAN_CLASSIC_CAN,
                               .TxEventFifoControl = FDCAN_NO_TX_EVENTS,
                               .MessageMarker = 0};

  uint8_t data = TEST_PAYLOAD;

  HAL_StatusTypeDef status =
      HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &hdr, &data);

  if (status != HAL_OK) {
    log_uart("!");
  }
}

/* ============================================================
 * FDCAN RX INTERRUPT CALLBACK
 *
 * IMPORTANT:
 * - Keep this function as short as possible
 * - Do NOT add application logic here
 * - Do NOT use blocking calls (e.g. UART)
 * ============================================================ */
void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan,
                               uint32_t RxFifo0ITs) {
  if ((hfdcan->Instance == FDCAN2) &&
      (RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE)) {
    HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &rx_header, rx_data);

    /* Notify main loop that a new frame is available */
    fdcan_rx_pending = true;
  }
}

static void fdcan_process_rx(void) {
  fdcan_rx_pending = false;

  if ((rx_header.Identifier == TEST_CAN_ID) && (rx_data[0] == TEST_PAYLOAD)) {
    HAL_GPIO_TogglePin(LED_1_GPIO_Port, LED_1_Pin);
    log_uart("<");
  } else {
    log_uart("\r\n[FDCAN RX ERROR]\r\n");
  }
}

static void log_uart(const char *msg) {
  HAL_UART_Transmit(&huart5, (uint8_t *)msg, strlen(msg), 10);
}