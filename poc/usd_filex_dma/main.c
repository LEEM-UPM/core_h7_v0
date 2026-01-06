#include "main.h"
#include "fx_api.h"
#include "fx_stm32_sd_driver_dma_rtos.h"
#include "gpio.h"
#include "sdmmc.h"
#include "tx_api.h"
#include <stdio.h>

#define FX_APP_THREAD_NAME "FileX app thread"
#define FX_APP_THREAD_TIME_SLICE TX_NO_TIME_SLICE
#define FX_APP_THREAD_AUTO_START TX_AUTO_START
#define FX_APP_PREEMPTION_THRESHOLD FX_APP_THREAD_PRIO
#define FX_APP_MEM_POOL_SIZE 8192
#define FX_APP_THREAD_STACK_SIZE 4096
#define FX_APP_THREAD_PRIO 10

#define SD_MEDIA_NAME "SD_DISK"
#define TEST_FILE_NAME "/TEST.TXT"

__ALIGN_BEGIN static UCHAR
    fx_byte_pool_buffer[FX_APP_MEM_POOL_SIZE] __ALIGN_END;
static TX_BYTE_POOL fx_app_byte_pool;
static TX_THREAD fx_app_thread;
static FX_MEDIA sd_media;

/* Buffer para el media de FileX (debe ser al menos 512 bytes) */
__ALIGN_BEGIN static UCHAR
    media_memory[FX_STM32_SD_DEFAULT_SECTOR_SIZE] __ALIGN_END;

extern void SystemClock_Config(void);
extern void PeriphCommonClock_Config(void);

static UINT fx_app_init(VOID *memory_ptr);
static void fx_app_thread_entry(ULONG thread_input);
static void wait_for_sd_card_insertion(void);
static UINT mount_sd_card(void);
static UINT write_test_file(void);
static void indicate_status(UINT status);

int main(void) {
  HAL_Init();
  SystemClock_Config();
  PeriphCommonClock_Config();

  MX_GPIO_Init();
  MX_SDMMC1_SD_Init();

  tx_kernel_enter();

  while (1) {
  }
}

VOID tx_application_define(VOID *first_unused_memory) {
  UINT status = TX_SUCCESS;
  VOID *memory_ptr;
  (void)first_unused_memory;

  status = tx_byte_pool_create(&fx_app_byte_pool, "Fx App memory pool",
                               fx_byte_pool_buffer, FX_APP_MEM_POOL_SIZE);

  if (status != TX_SUCCESS) {
    HAL_GPIO_WritePin(LED_1_GPIO_Port, LED_1_Pin, GPIO_PIN_SET);
    while (1) {
    }
  }

  memory_ptr = (VOID *)&fx_app_byte_pool;
  status = fx_app_init(memory_ptr);

  if (status != FX_SUCCESS) {
    while (1) {
      HAL_GPIO_TogglePin(LED_1_GPIO_Port, LED_1_Pin);
      HAL_Delay(100);
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
  UINT status;
  (void)thread_input;

  wait_for_sd_card_insertion();

  /* for debouncing purpose we wait a bit till it settles down */
  tx_thread_sleep(TX_TIMER_TICKS_PER_SECOND / 2);

  status = mount_sd_card();

  if (status == FX_SUCCESS) {
    status = write_test_file();

    fx_media_close(&sd_media);
  }

  indicate_status(status);

  while (1) {
    tx_thread_sleep(TX_TIMER_TICKS_PER_SECOND);
  }
}

static void wait_for_sd_card_insertion(void) {
  HAL_GPIO_WritePin(LED_1_GPIO_Port, LED_1_Pin, GPIO_PIN_RESET);

  /* Polling del pin SD_CD hasta que se detecte tarjeta
   * Nota: SD_CD suele estar activo bajo (0 = tarjeta presente)
   * Ajusta GPIO_PIN_RESET/SET según tu hardware
   */
  while (HAL_GPIO_ReadPin(SD1_CD_GPIO_Port, SD1_CD_Pin) != GPIO_PIN_RESET) {
    HAL_GPIO_TogglePin(LED_1_GPIO_Port, LED_1_Pin);
    tx_thread_sleep(TX_TIMER_TICKS_PER_SECOND);
  }

  /* Parpadeo rápido para indicar que se detectó la tarjeta */
  for (int i = 0; i < 6; i++) {
    HAL_GPIO_TogglePin(LED_1_GPIO_Port, LED_1_Pin);
    tx_thread_sleep(TX_TIMER_TICKS_PER_SECOND / 5);
  }

  HAL_GPIO_WritePin(LED_1_GPIO_Port, LED_1_Pin, GPIO_PIN_RESET);
}

static UINT mount_sd_card(void) {
  UINT status;

  /* Abrir el media SD usando el driver de ST
   * El tercer parámetro es la función del driver (fx_stm32_sd_driver)
   * que ya está implementada en fx_stm32_sd_driver.c
   */
  status = fx_media_open(&sd_media, SD_MEDIA_NAME, fx_stm32_sd_driver,
                         (VOID *)FX_NULL, media_memory, sizeof(media_memory));

  return status;
}

static UINT write_test_file(void) {
  UINT status;
  FX_FILE test_file;
  CHAR test_data[256];

  snprintf(test_data, sizeof(test_data),
           "=== STM32H723 SD DMA Card Test ===\r\n"
           "FileX Version: %s\r\n"
           "ThreadX Version: %s\r\n"
           "Status: SD card detected and mounted successfully!\r\n"
           "This file was created automatically.\r\n"
           "Date: Build %s %s\r\n"
           "================================\r\n",
           "6.x", "6.x", __DATE__, __TIME__);

  status = fx_file_create(&sd_media, TEST_FILE_NAME);

  if (status != FX_SUCCESS && status != FX_ALREADY_CREATED) {
    return status;
  }

  status =
      fx_file_open(&sd_media, &test_file, TEST_FILE_NAME, FX_OPEN_FOR_WRITE);

  if (status != FX_SUCCESS) {
    return status;
  }

  /* If the file already existed, we look for the end to add it. */
  status = fx_file_seek(&test_file, 0);

  if (status != FX_SUCCESS) {
    fx_file_close(&test_file);
    return status;
  }

  status = fx_file_write(&test_file, test_data, strlen(test_data));

  if (status != FX_SUCCESS) {
    fx_file_close(&test_file);
    return status;
  }

  /* Close the file to ensure that the data is written. */
  status = fx_file_close(&test_file);

  if (status != FX_SUCCESS) {
    return status;
  }

  /* Flush the media to ensure physical writing */
  status = fx_media_flush(&sd_media);

  return status;
}

static void indicate_status(UINT status) {
  if (status == FX_SUCCESS) {
    HAL_GPIO_WritePin(LED_1_GPIO_Port, LED_1_Pin, GPIO_PIN_SET);
  } else {
    /* Error: LED parpadeando según el código de error */
    /* Parpadeos rápidos = error en montaje/detección */
    /* Parpadeos lentos = error en escritura */
    UINT delay = (status < FX_MEDIA_NOT_OPEN) ? 200 : 500;

    while (1) {
      HAL_GPIO_TogglePin(LED_1_GPIO_Port, LED_1_Pin);
      tx_thread_sleep(delay * TX_TIMER_TICKS_PER_SECOND / 1000);
    }
  }
}