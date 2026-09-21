/**
 * @file sd_card_fx_driver.h
 * @brief Driver de FileX sobre sd_card (una instancia por tarjeta).
 *
 * Uso: fx_media_open(&media, "SD1", sd_card_fx_driver,
 *                    SD_CARD_FX_DRIVER_INFO(SD_CARD_1), buffer, tamaño);
 */

#ifndef SD_CARD_FX_DRIVER_H
#define SD_CARD_FX_DRIVER_H

#include <stdint.h>

#include "fx_api.h"
#include "sd_card.h"

/* La tarjeta a usar viaja en el puntero driver_info de fx_media_open() */
#define SD_CARD_FX_DRIVER_INFO(id) ((VOID *)(uintptr_t)(id))

VOID sd_card_fx_driver(FX_MEDIA *media_ptr);

#endif /* SD_CARD_FX_DRIVER_H */
