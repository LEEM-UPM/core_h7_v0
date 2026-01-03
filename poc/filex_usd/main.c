#include "fx_api.h"
#include "gpio.h"
#include "sdmmc.h"
#include "tx_api.h"

#define FX_APP_THREAD_NAME "FileX app thread"
#define FX_APP_THREAD_TIME_SLICE TX_NO_TIME_SLICE
#define FX_APP_THREAD_AUTO_START TX_AUTO_START
#define FX_APP_PREEMPTION_THRESHOLD FX_APP_THREAD_PRIO
#define FX_APP_MEM_POOL_SIZE 8192
#define FX_APP_THREAD_STACK_SIZE 4096
#define FX_APP_THREAD_PRIO 10

__ALIGN_BEGIN static UCHAR
    fx_byte_pool_buffer[FX_APP_MEM_POOL_SIZE] __ALIGN_END;
static TX_BYTE_POOL fx_app_byte_pool;

TX_THREAD fx_app_thread;

extern void SystemClock_Config(void);
extern void PeriphCommonClock_Config(void);
UINT fx_app_init(VOID *memory_ptr);
void fx_app_thread_entry(ULONG thread_input);

int main(void) {
  HAL_Init();
  SystemClock_Config();
  PeriphCommonClock_Config();

  MX_GPIO_Init();

  tx_kernel_enter();

  while (1) {
  }
}

VOID tx_application_define(VOID *first_unused_memory) {
  UINT status = TX_SUCCESS;
  VOID *memory_ptr;

  if (tx_byte_pool_create(&fx_app_byte_pool, "Fx App memory pool",
                          fx_byte_pool_buffer,
                          FX_APP_MEM_POOL_SIZE) != TX_SUCCESS) {
    // ERROR
  } else {
    memory_ptr = (VOID *)&fx_app_byte_pool;
    status = fx_app_init(memory_ptr);
    if (status != FX_SUCCESS) {
      while (1) {
      }
    }
  }
}

UINT fx_app_init(VOID *memory_ptr) {
  UINT ret = FX_SUCCESS;
  TX_BYTE_POOL *byte_pool = (TX_BYTE_POOL *)memory_ptr;
  VOID *pointer;

  /* Allocate the stack for fx app thread  */
  ret = tx_byte_allocate(byte_pool, &pointer, FX_APP_THREAD_STACK_SIZE,
                         TX_NO_WAIT);

  if (ret != FX_SUCCESS) {
    return TX_POOL_ERROR;
  }

  /* Create fx app thread.  */
  ret = tx_thread_create(
      &fx_app_thread, FX_APP_THREAD_NAME, fx_app_thread_entry, 0, pointer,
      FX_APP_THREAD_STACK_SIZE, FX_APP_THREAD_PRIO, FX_APP_PREEMPTION_THRESHOLD,
      FX_APP_THREAD_TIME_SLICE, FX_APP_THREAD_AUTO_START);

  if (ret != TX_SUCCESS) {
    return TX_THREAD_ERROR;
  }

  fx_system_initialize();

  return ret;
}

void fx_app_thread_entry(ULONG thread_input) {
  (void)thread_input;

  while (1) {
    HAL_GPIO_TogglePin(LED_1_GPIO_Port, LED_1_Pin);
    tx_thread_sleep(TX_TIMER_TICKS_PER_SECOND);
  }
}
