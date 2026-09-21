/**
 * @file storage.h
 * @brief Archivos en las dos tarjetas microSD del core (FileX, FAT12/16/32).
 *
 * storage_init() inicializa y monta las dos tarjetas y hace una prueba en cada
 * una: escribe CORE_TEST.TXT, lo relee y compara, y actualiza el contador de
 * arranques en BOOTCNT.TXT. Todo es bloqueante (pensado para el bucle
 * principal, sin RTOS).
 *
 * Nombres de archivo: "/LOG.TXT" o "LOG.TXT" (se admiten nombres largos).
 */

#ifndef STORAGE_H
#define STORAGE_H

#include <stdbool.h>
#include <stdint.h>

#include "sd_card.h"

typedef enum {
  STORAGE_SD_NOT_STARTED = 0,
  STORAGE_SD_INIT_ERROR,  /* la tarjeta no responde (¿no hay tarjeta?) */
  STORAGE_SD_MOUNT_ERROR, /* responde pero no se puede abrir el FAT */
  STORAGE_SD_MOUNTED,
} storage_sd_state_t;

typedef enum {
  STORAGE_TEST_NOT_RUN = 0,
  STORAGE_TEST_OK,
  STORAGE_TEST_FAILED,
} storage_test_t;

typedef struct {
  storage_sd_state_t state;
  bool cd_pin_high;       /* nivel del pin de detección al arrancar */
  uint32_t capacity_mb;   /* tamaño de la tarjeta */
  uint32_t free_mb;       /* espacio libre en el FAT */
  uint32_t boot_count;    /* valor de BOOTCNT.TXT tras este arranque */
  storage_test_t self_test;
  uint32_t self_test_ms;  /* duración de la prueba de escritura + lectura */
  uint32_t fx_error;      /* último error de FileX (FX_*), 0 si no hubo */
  uint32_t hal_error;     /* último error del HAL SD (HAL_SD_ERROR_*) */
} storage_sd_info_t;

void storage_init(void);

/* Escribe 'len' bytes en el archivo (lo crea si no existe). append = false
 * sustituye el contenido; append = true añade al final. Al terminar vuelca la
 * caché a la tarjeta, así que lo escrito sobrevive a un corte de corriente. */
bool storage_write_file(sd_card_id_t sd, const char *path, const uint8_t *data,
                        uint32_t len, bool append);

/* Lee hasta 'buf_size' bytes desde el principio del archivo. */
bool storage_read_file(sd_card_id_t sd, const char *path, uint8_t *buf,
                       uint32_t buf_size, uint32_t *read_len);

bool storage_delete_file(sd_card_id_t sd, const char *path);

const storage_sd_info_t *storage_get_info(sd_card_id_t sd);

#endif /* STORAGE_H */
