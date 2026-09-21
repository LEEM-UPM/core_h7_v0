#include "storage.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fx_api.h"
#include "main.h"
#include "sd_card_fx_driver.h"

#define STORAGE_MEDIA_BUFFER_SIZE (16U * SD_CARD_BLOCK_SIZE) /* caché de FileX */

#define STORAGE_TEST_FILE "CORE_TEST.TXT"
#define STORAGE_TEST_LINES 64U /* 64 líneas de 64 bytes: 4096 B, 8 sectores */
#define STORAGE_TEST_LINE_LEN 64U
#define STORAGE_TEST_SIZE (STORAGE_TEST_LINES * STORAGE_TEST_LINE_LEN)

#define STORAGE_BOOT_COUNT_FILE "BOOTCNT.TXT"

typedef struct {
  FX_MEDIA media;
  UCHAR buffer[STORAGE_MEDIA_BUFFER_SIZE];
  storage_sd_info_t info;
} storage_sd_t;

static storage_sd_t s_sd[SD_CARD_COUNT];

/* Buffers de la prueba: estáticos (no caben cómodos en la pila) */
static uint8_t s_test_write[STORAGE_TEST_SIZE];
static uint8_t s_test_read[STORAGE_TEST_SIZE];

static const char *const s_media_names[SD_CARD_COUNT] = {"SD1", "SD2"};

static storage_sd_t *_mounted(sd_card_id_t sd) {
  if ((unsigned)sd >= (unsigned)SD_CARD_COUNT) {
    return NULL;
  }
  return (s_sd[sd].info.state == STORAGE_SD_MOUNTED) ? &s_sd[sd] : NULL;
}

/* Guarda el error de FileX (y el del HAL, que suele ser la causa) */
static bool _fail(storage_sd_t *s, sd_card_id_t sd, UINT fx_status) {
  s->info.fx_error = fx_status;
  s->info.hal_error = sd_card_last_error(sd);
  return false;
}

static void _update_free_space(storage_sd_t *s) {
  ULONG64 free_bytes = 0U;
  if (fx_media_extended_space_available(&s->media, &free_bytes) == FX_SUCCESS) {
    s->info.free_mb = (uint32_t)(free_bytes / (1024U * 1024U));
  }
}

/* ===========================================================================
 * Prueba de arranque
 * ======================================================================== */

/* Texto legible desde el PC: "SD1 linea 00 ABCD...\r\n" */
static void _fill_test_pattern(sd_card_id_t sd) {
  for (uint32_t line = 0U; line < STORAGE_TEST_LINES; line++) {
    char *p = (char *)&s_test_write[line * STORAGE_TEST_LINE_LEN];
    int n = snprintf(p, STORAGE_TEST_LINE_LEN, "%s linea %02lu ",
                     s_media_names[sd], (unsigned long)line);
    for (uint32_t i = (uint32_t)n; i < (STORAGE_TEST_LINE_LEN - 2U); i++) {
      p[i] = (char)('A' + ((i + line) % 26U));
    }
    p[STORAGE_TEST_LINE_LEN - 2U] = '\r';
    p[STORAGE_TEST_LINE_LEN - 1U] = '\n';
  }
}

static void _self_test(sd_card_id_t sd) {
  storage_sd_t *s = &s_sd[sd];
  const uint32_t start = HAL_GetTick();

  _fill_test_pattern(sd);
  memset(s_test_read, 0, sizeof(s_test_read));

  uint32_t read_len = 0U;
  bool ok = storage_write_file(sd, STORAGE_TEST_FILE, s_test_write,
                               STORAGE_TEST_SIZE, false) &&
            storage_read_file(sd, STORAGE_TEST_FILE, s_test_read,
                              sizeof(s_test_read), &read_len) &&
            (read_len == STORAGE_TEST_SIZE) &&
            (memcmp(s_test_write, s_test_read, STORAGE_TEST_SIZE) == 0);

  s->info.self_test = ok ? STORAGE_TEST_OK : STORAGE_TEST_FAILED;
  s->info.self_test_ms = HAL_GetTick() - start;
}

/* Lee el contador de arranques, lo incrementa y lo vuelve a escribir:
 * comprueba que lo escrito se conserva de un arranque al siguiente. */
static void _boot_count(sd_card_id_t sd) {
  char text[16] = {0};
  uint32_t len = 0U;
  uint32_t count = 0U;

  /* En el primer arranque el archivo no existe: no es un error a mostrar */
  const storage_sd_info_t saved = s_sd[sd].info;
  if (storage_read_file(sd, STORAGE_BOOT_COUNT_FILE, (uint8_t *)text,
                        sizeof(text) - 1U, &len)) {
    count = (uint32_t)strtoul(text, NULL, 10);
  } else {
    s_sd[sd].info.fx_error = saved.fx_error;
    s_sd[sd].info.hal_error = saved.hal_error;
  }
  count++;

  const int n = snprintf(text, sizeof(text), "%lu\r\n", (unsigned long)count);
  if (storage_write_file(sd, STORAGE_BOOT_COUNT_FILE, (const uint8_t *)text,
                         (uint32_t)n, false)) {
    s_sd[sd].info.boot_count = count;
  }
}

/* ===========================================================================
 * API
 * ======================================================================== */

void storage_init(void) {
  fx_system_initialize();

  for (int i = 0; i < (int)SD_CARD_COUNT; i++) {
    const sd_card_id_t sd = (sd_card_id_t)i;
    storage_sd_t *s = &s_sd[i];
    memset(&s->info, 0, sizeof(s->info));
    s->info.cd_pin_high = sd_card_cd_pin_high(sd);

    if (!sd_card_init(sd)) {
      s->info.state = STORAGE_SD_INIT_ERROR;
      s->info.hal_error = sd_card_last_error(sd);
      continue;
    }
    s->info.capacity_mb =
        (uint32_t)(((uint64_t)sd_card_block_count(sd) * SD_CARD_BLOCK_SIZE) /
                   (1024U * 1024U));

    const UINT status =
        fx_media_open(&s->media, (CHAR *)s_media_names[i], sd_card_fx_driver,
                      SD_CARD_FX_DRIVER_INFO(sd), s->buffer, sizeof(s->buffer));
    if (status != FX_SUCCESS) {
      s->info.state = STORAGE_SD_MOUNT_ERROR;
      (void)_fail(s, sd, status);
      continue;
    }
    s->info.state = STORAGE_SD_MOUNTED;

    _self_test(sd);
    _boot_count(sd);
    _update_free_space(s);
  }
}

bool storage_write_file(sd_card_id_t sd, const char *path, const uint8_t *data,
                        uint32_t len, bool append) {
  storage_sd_t *s = _mounted(sd);
  if ((s == NULL) || (path == NULL) || ((data == NULL) && (len > 0U))) {
    return false;
  }

  UINT status = fx_file_create(&s->media, (CHAR *)path);
  if ((status != FX_SUCCESS) && (status != FX_ALREADY_CREATED)) {
    return _fail(s, sd, status);
  }

  FX_FILE file;
  status = fx_file_open(&s->media, &file, (CHAR *)path, FX_OPEN_FOR_WRITE);
  if (status != FX_SUCCESS) {
    return _fail(s, sd, status);
  }

  if (append) {
    status = fx_file_relative_seek(&file, 0U, FX_SEEK_END);
  } else {
    status = fx_file_truncate_release(&file, 0U);
    if (status == FX_SUCCESS) {
      status = fx_file_seek(&file, 0U);
    }
  }
  if ((status == FX_SUCCESS) && (len > 0U)) {
    status = fx_file_write(&file, (VOID *)data, len);
  }

  const UINT close_status = fx_file_close(&file);
  if (status == FX_SUCCESS) {
    status = close_status;
  }
  if (status == FX_SUCCESS) {
    status = fx_media_flush(&s->media);
  }
  if (status != FX_SUCCESS) {
    return _fail(s, sd, status);
  }

  _update_free_space(s);
  return true;
}

bool storage_read_file(sd_card_id_t sd, const char *path, uint8_t *buf,
                       uint32_t buf_size, uint32_t *read_len) {
  storage_sd_t *s = _mounted(sd);
  if (read_len != NULL) {
    *read_len = 0U;
  }
  if ((s == NULL) || (path == NULL) || (buf == NULL)) {
    return false;
  }

  FX_FILE file;
  UINT status = fx_file_open(&s->media, &file, (CHAR *)path, FX_OPEN_FOR_READ);
  if (status != FX_SUCCESS) {
    return _fail(s, sd, status);
  }

  ULONG actual = 0U;
  status = fx_file_read(&file, buf, buf_size, &actual);
  if (status == FX_END_OF_FILE) {
    status = FX_SUCCESS; /* archivo vacío */
  }
  (void)fx_file_close(&file);

  if (status != FX_SUCCESS) {
    return _fail(s, sd, status);
  }
  if (read_len != NULL) {
    *read_len = (uint32_t)actual;
  }
  return true;
}

bool storage_delete_file(sd_card_id_t sd, const char *path) {
  storage_sd_t *s = _mounted(sd);
  if ((s == NULL) || (path == NULL)) {
    return false;
  }
  UINT status = fx_file_delete(&s->media, (CHAR *)path);
  if (status == FX_SUCCESS) {
    status = fx_media_flush(&s->media);
  }
  if (status != FX_SUCCESS) {
    return _fail(s, sd, status);
  }
  _update_free_space(s);
  return true;
}

const storage_sd_info_t *storage_get_info(sd_card_id_t sd) {
  return ((unsigned)sd < (unsigned)SD_CARD_COUNT) ? &s_sd[sd].info : NULL;
}
