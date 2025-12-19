#include "gpio.h"
#include "tx_api.h"

extern void SystemClock_Config(void);
extern void PeriphCommonClock_Config(void);

#define USE_STATIC_ALLOCATION 1
#define TX_APP_MEM_POOL_SIZE 4096
#define TX_APP_STACK_SIZE 512
#define TX_APP_THREAD_PRIO 10
#define TX_APP_THREAD_PREEMPTION_THRESHOLD TX_APP_THREAD_PRIO
#define TX_APP_THREAD_TIME_SLICE TX_NO_TIME_SLICE
#define TX_APP_THREAD_AUTO_START TX_AUTO_START

__ALIGN_BEGIN static UCHAR
    tx_byte_pool_buffer[TX_APP_MEM_POOL_SIZE] __ALIGN_END;
static TX_BYTE_POOL tx_app_byte_pool;
TX_THREAD tx_app_thread;

UINT app_threadx_init(VOID *memory_ptr);
void tx_app_thread_entry(ULONG thread_input);

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

  if (tx_byte_pool_create(&tx_app_byte_pool, "Tx App memory pool",
                          tx_byte_pool_buffer,
                          TX_APP_MEM_POOL_SIZE) != TX_SUCCESS) {

  } else {

    memory_ptr = (VOID *)&tx_app_byte_pool;
    status = app_threadx_init(memory_ptr);
    if (status != TX_SUCCESS) {

      while (1) {
      }
    }
  }
}

UINT app_threadx_init(VOID *memory_ptr) {
  UINT ret = TX_SUCCESS;
  TX_BYTE_POOL *byte_pool = (TX_BYTE_POOL *)memory_ptr;
  CHAR *pointer;

  /* Allocate the stack for tx app thread  */
  if (tx_byte_allocate(byte_pool, (VOID **)&pointer, TX_APP_STACK_SIZE,
                       TX_NO_WAIT) != TX_SUCCESS) {
    return TX_POOL_ERROR;
  }
  /* Create tx app thread.  */
  if (tx_thread_create(&tx_app_thread, "tx app thread", tx_app_thread_entry, 0,
                       pointer, TX_APP_STACK_SIZE, TX_APP_THREAD_PRIO,
                       TX_APP_THREAD_PREEMPTION_THRESHOLD,
                       TX_APP_THREAD_TIME_SLICE,
                       TX_APP_THREAD_AUTO_START) != TX_SUCCESS) {
    return TX_THREAD_ERROR;
  }

  return ret;
}

void tx_app_thread_entry(ULONG thread_input) {
  (void)thread_input;

  while (1) {
    HAL_GPIO_TogglePin(LED_1_GPIO_Port, LED_1_Pin);
    tx_thread_sleep(TX_TIMER_TICKS_PER_SECOND);
  }
}
