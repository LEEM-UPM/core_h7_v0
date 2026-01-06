#ifndef FX_STM32_SD_DRIVER_H
#define FX_STM32_SD_DRIVER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "fx_api.h"
#include "stm32h7xx_hal.h" // It should be a private "include"

extern TX_SEMAPHORE sd_tx_semaphore;
extern TX_SEMAPHORE sd_rx_semaphore;

/* Default timeout used to wait for fx operations */
#define FX_STM32_SD_DEFAULT_TIMEOUT (10 * TX_TIMER_TICKS_PER_SECOND)

/* let the filex low-level driver initialize the SD driver */
#define FX_STM32_SD_INIT 0

/* Enable the cache maintenance, needed when using SD DMA
 * and accessing buffers in cacheable area
 */
#define FX_STM32_SD_CACHE_MAINTENANCE 1

/* Define the CPU Cache maintenance macros */
#if (FX_STM32_SD_CACHE_MAINTENANCE == 1)
#define invalidate_cache_by_addr(__ptr__, __size__)                            \
  SCB_InvalidateDCache_by_Addr((void *)__ptr__, (int32_t)__size__)
#define clean_cache_by_addr(__ptr__, __size__)                                 \
  SCB_CleanDCache_by_Addr((uint32_t *)__ptr__, (int32_t)__size__)
#endif

/* Use the SD DMA API */
#define FX_STM32_SD_DMA_API 1

/* SDIO instance to be used by FileX */
#define FX_STM32_SD_INSTANCE 0

/* Default sector size, used by the driver */
#define FX_STM32_SD_DEFAULT_SECTOR_SIZE 512

/* Define the macro to get the current time in ticks */
#define FX_STM32_SD_CURRENT_TIME() tx_time_get()

/* Macro called before initializing the SD driver
 * e.g. create a semaphore used for transfer notification */
#define FX_STM32_SD_PRE_INIT(_media_ptr)                                       \
  do {                                                                         \
    if ((tx_semaphore_create(&sd_rx_semaphore, "sd rx transfer semaphore",     \
                             0) != TX_SUCCESS) ||                              \
        (tx_semaphore_create(&sd_tx_semaphore, "sd tx transfer semaphore",     \
                             0) != TX_SUCCESS)) {                              \
      _media_ptr->fx_media_driver_status = FX_IO_ERROR;                        \
    }                                                                          \
  } while (0)

/* Macro called after initializing the SD driver */
#define FX_STM32_SD_POST_INIT(_media_ptr)

/* Macro called after the SD deinit */
#define FX_STM32_SD_POST_DEINIT(_media_ptr)                                    \
  do {                                                                         \
    tx_semaphore_delete(&sd_rx_semaphore);                                     \
    tx_semaphore_delete(&sd_tx_semaphore);                                     \
  } while (0)

/* Macro called after the abort request */
#define FX_STM32_SD_POST_ABORT(_media_ptr)

/* Macro called before performing read operation */
#define FX_STM32_SD_PRE_READ_TRANSFER(_media_ptr)

/* Macro called after performing read operation */
#define FX_STM32_SD_POST_READ_TRANSFER(_media_ptr)

/* Macro for read error handling */
#define FX_STM32_SD_READ_TRANSFER_ERROR(_status_)

/* Define how to notify about Read completion operation */
#define FX_STM32_SD_READ_CPLT_NOTIFY()                                         \
  do {                                                                         \
    if (tx_semaphore_get(&sd_rx_semaphore, FX_STM32_SD_DEFAULT_TIMEOUT) !=     \
        TX_SUCCESS) {                                                          \
      return FX_IO_ERROR;                                                      \
    }                                                                          \
  } while (0)

/* Define how to notify about write completion operation */
#define FX_STM32_SD_WRITE_CPLT_NOTIFY()                                        \
  do {                                                                         \
    if (tx_semaphore_get(&sd_tx_semaphore, FX_STM32_SD_DEFAULT_TIMEOUT) !=     \
        TX_SUCCESS) {                                                          \
      return FX_IO_ERROR;                                                      \
    }                                                                          \
  } while (0)

/* Macro called before performing write operation */
#define FX_STM32_SD_PRE_WRITE_TRANSFER(_media_ptr)

/* Macro called after performing write operation */
#define FX_STM32_SD_POST_WRITE_TRANSFER(_media_ptr)

/* Macro for write error handling */
#define FX_STM32_SD_WRITE_TRANSFER_ERROR(_status_)

INT fx_stm32_sd_init(UINT instance);
INT fx_stm32_sd_deinit(UINT instance);

INT fx_stm32_sd_get_status(UINT instance);

INT fx_stm32_sd_read_blocks(UINT instance, UINT *buffer, UINT start_block,
                            UINT total_blocks);
INT fx_stm32_sd_write_blocks(UINT instance, UINT *buffer, UINT start_block,
                             UINT total_blocks);

VOID fx_stm32_sd_driver(FX_MEDIA *media_ptr);

#ifdef __cplusplus
}
#endif

#endif /* FX_STM32_SD_DRIVER_H */