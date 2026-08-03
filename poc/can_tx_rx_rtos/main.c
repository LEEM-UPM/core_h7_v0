#include "main.h"
#include "can_rx_thread.h"
#include "can_shared_data.h"
#include "can_tx_test.h"
#include "fdcan.h"
#include "gpio.h"
#include "tx_api.h"

#define BYTE_POOL_SIZE (16U * 1024U)

extern void SystemClock_Config(void);
extern void PeriphCommonClock_Config(void);

int main(void) {
  HAL_Init();
  SystemClock_Config();
  PeriphCommonClock_Config();

  MX_GPIO_Init();
  MX_FDCAN1_Init();
  MX_FDCAN2_Init();

  // TCAN1044: STB = LOW -> Normal mode (CAN enabled)
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_3 | GPIO_PIN_4, GPIO_PIN_RESET);

  tx_kernel_enter();

  return 0;
}

VOID tx_application_define(VOID *first_unused_memory) {
  UINT status;

  static TX_BYTE_POOL byte_pool;
  static ULONG pool_buf[BYTE_POOL_SIZE / sizeof(ULONG)];

  status =
      tx_byte_pool_create(&byte_pool, "main pool", pool_buf, sizeof(pool_buf));
  if (status != TX_SUCCESS)
    Error_Handler();

  status = can_shared_data_init();
  if (status != TX_SUCCESS)
    Error_Handler();

  status = can_rx_thread_create(&byte_pool, &hfdcan2);
  if (status != TX_SUCCESS)
    Error_Handler();

  status = can_tx_test_create(&byte_pool, &hfdcan1);
  if (status != TX_SUCCESS)
    Error_Handler();
}