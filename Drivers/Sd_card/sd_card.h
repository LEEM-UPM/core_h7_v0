/**
 * @file sd_card.h
 * @brief Acceso por bloques a las dos tarjetas microSD del core.
 *
 * SD1 en SDMMC1 (PC8-PC12, PD2, detección PC7) y SD2 en SDMMC2 (PG9-PG11, PB4,
 * PD6, PD7, detección PA8), bus de 4 bits. Las transferencias van por DMA
 * (IDMA del SDMMC) y las funciones esperan a que terminen: son bloqueantes.
 *
 * A diferencia de MX_SDMMCx_SD_Init(), si no hay tarjeta o falla no se llama a
 * Error_Handler(): sd_card_init() devuelve false y el resto del core sigue.
 */

#ifndef SD_CARD_H
#define SD_CARD_H

#include <stdbool.h>
#include <stdint.h>

typedef enum {
  SD_CARD_1 = 0, /* SDMMC1 */
  SD_CARD_2,     /* SDMMC2 */
  SD_CARD_COUNT
} sd_card_id_t;

#define SD_CARD_BLOCK_SIZE 512U

/* Inicializa la tarjeta (identificación, bus de 4 bits). Se puede volver a
 * llamar tras un fallo para reintentar. */
bool sd_card_init(sd_card_id_t id);
void sd_card_deinit(sd_card_id_t id);
bool sd_card_is_ready(sd_card_id_t id);

/* Nivel crudo del pin de detección de tarjeta (SDx_CD) */
bool sd_card_cd_pin_high(sd_card_id_t id);

/* Número de bloques de 512 bytes de la tarjeta (0 si no está lista) */
uint32_t sd_card_block_count(sd_card_id_t id);

/* Último código de error del HAL (HAL_SD_ERROR_*), 0 si no hubo */
uint32_t sd_card_last_error(sd_card_id_t id);

/* Lectura / escritura de 'count' bloques a partir de 'block'. El buffer debe
 * estar en RAM accesible por DMA (RAM_D1, donde el linker pone todo ahora). */
bool sd_card_read_blocks(sd_card_id_t id, uint8_t *buf, uint32_t block,
                         uint32_t count);
bool sd_card_write_blocks(sd_card_id_t id, const uint8_t *buf, uint32_t block,
                          uint32_t count);

#endif /* SD_CARD_H */
