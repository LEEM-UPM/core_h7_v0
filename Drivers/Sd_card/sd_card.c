#include "sd_card.h"

#include <string.h>

#include "main.h"
#include "sdmmc.h"

/* Reloj del bus: SDMMCCLK (20 MHz, PLL1Q) / (2 * div). 16 -> 625 kHz, igual
 * que el .ioc. La identificación la hace el HAL a 400 kHz por su cuenta. */
#define SD_CARD_CLOCK_DIV 16U

#define SD_CARD_TRANSFER_TIMEOUT_MS 2000U /* fin de la transferencia DMA */
#define SD_CARD_READY_TIMEOUT_MS 1000U    /* tarjeta de vuelta en TRANSFER */

typedef struct {
  SD_HandleTypeDef *hsd;
  SDMMC_TypeDef *instance;
  GPIO_TypeDef *cd_port;
  uint16_t cd_pin;
  bool ready;
  uint32_t last_error;
  volatile bool rx_done;
  volatile bool tx_done;
  volatile bool failed;
} sd_card_t;

static sd_card_t s_cards[SD_CARD_COUNT] = {
    [SD_CARD_1] = {.hsd = &hsd1,
                   .instance = SDMMC1,
                   .cd_port = SD1_CD_GPIO_Port,
                   .cd_pin = SD1_CD_Pin},
    [SD_CARD_2] = {.hsd = &hsd2,
                   .instance = SDMMC2,
                   .cd_port = SD2_CD_GPIO_Port,
                   .cd_pin = SD2_CD_Pin},
};

/* El IDMA trabaja con palabras de 32 bits: si FileX pasa un buffer que no está
 * alineado a 4 bytes, se transfiere bloque a bloque a través de este. */
static uint8_t s_scratch[SD_CARD_BLOCK_SIZE] __attribute__((aligned(4)));

static sd_card_t *_card(sd_card_id_t id) {
  return ((unsigned)id < (unsigned)SD_CARD_COUNT) ? &s_cards[id] : NULL;
}

static sd_card_t *_card_from_handle(const SD_HandleTypeDef *hsd) {
  for (int i = 0; i < (int)SD_CARD_COUNT; i++) {
    if (s_cards[i].hsd == hsd) {
      return &s_cards[i];
    }
  }
  return NULL;
}

/* Espera a que la tarjeta vuelva al estado TRANSFER (lista para otra orden) */
static bool _wait_transfer_state(sd_card_t *c) {
  const uint32_t start = HAL_GetTick();
  while ((HAL_GetTick() - start) < SD_CARD_READY_TIMEOUT_MS) {
    if (HAL_SD_GetCardState(c->hsd) == HAL_SD_CARD_TRANSFER) {
      return true;
    }
  }
  c->last_error = HAL_SD_ERROR_TIMEOUT;
  return false;
}

/* Espera al callback de fin de transferencia (o de error) */
static bool _wait_done(sd_card_t *c, const volatile bool *done) {
  const uint32_t start = HAL_GetTick();
  while (!*done && !c->failed) {
    if ((HAL_GetTick() - start) >= SD_CARD_TRANSFER_TIMEOUT_MS) {
      (void)HAL_SD_Abort(c->hsd);
      c->last_error = HAL_SD_ERROR_TIMEOUT;
      return false;
    }
  }
  if (c->failed) {
    c->last_error = HAL_SD_GetError(c->hsd);
    return false;
  }
  return _wait_transfer_state(c);
}

static bool _read(sd_card_t *c, uint8_t *buf, uint32_t block, uint32_t count) {
  c->rx_done = false;
  c->failed = false;
  if (HAL_SD_ReadBlocks_DMA(c->hsd, buf, block, count) != HAL_OK) {
    c->last_error = c->hsd->ErrorCode;
    return false;
  }
  return _wait_done(c, &c->rx_done);
}

static bool _write(sd_card_t *c, const uint8_t *buf, uint32_t block,
                   uint32_t count) {
  c->tx_done = false;
  c->failed = false;
  if (HAL_SD_WriteBlocks_DMA(c->hsd, buf, block, count) != HAL_OK) {
    c->last_error = c->hsd->ErrorCode;
    return false;
  }
  return _wait_done(c, &c->tx_done);
}

static bool _aligned(const uint8_t *buf) { return (((uintptr_t)buf) & 3U) == 0U; }

/* ===========================================================================
 * API
 * ======================================================================== */

bool sd_card_init(sd_card_id_t id) {
  sd_card_t *c = _card(id);
  if (c == NULL) {
    return false;
  }
  if (c->ready) {
    return true;
  }

  /* Tras un intento fallido el handle queda en ERROR: volver a RESET para que
   * HAL_SD_Init() rehaga todo (reloj, pines) */
  if (c->hsd->State != HAL_SD_STATE_RESET) {
    (void)HAL_SD_DeInit(c->hsd);
  }

  c->hsd->Instance = c->instance;
  c->hsd->Init.ClockEdge = SDMMC_CLOCK_EDGE_RISING;
  c->hsd->Init.ClockPowerSave = SDMMC_CLOCK_POWER_SAVE_DISABLE;
  c->hsd->Init.BusWide = SDMMC_BUS_WIDE_4B;
  c->hsd->Init.HardwareFlowControl = SDMMC_HARDWARE_FLOW_CONTROL_DISABLE;
  c->hsd->Init.ClockDiv = SD_CARD_CLOCK_DIV;

  if (HAL_SD_Init(c->hsd) != HAL_OK) {
    c->last_error = c->hsd->ErrorCode;
    return false;
  }

  c->last_error = 0U;
  c->ready = true;
  return true;
}

void sd_card_deinit(sd_card_id_t id) {
  sd_card_t *c = _card(id);
  if (c == NULL) {
    return;
  }
  (void)HAL_SD_DeInit(c->hsd);
  c->ready = false;
}

bool sd_card_is_ready(sd_card_id_t id) {
  const sd_card_t *c = _card(id);
  return (c != NULL) && c->ready;
}

bool sd_card_cd_pin_high(sd_card_id_t id) {
  const sd_card_t *c = _card(id);
  return (c != NULL) && (HAL_GPIO_ReadPin(c->cd_port, c->cd_pin) == GPIO_PIN_SET);
}

uint32_t sd_card_block_count(sd_card_id_t id) {
  const sd_card_t *c = _card(id);
  if ((c == NULL) || !c->ready) {
    return 0U;
  }
  HAL_SD_CardInfoTypeDef info;
  if (HAL_SD_GetCardInfo(c->hsd, &info) != HAL_OK) {
    return 0U;
  }
  /* LogBlockSize es 512 en SDSC y SDHC/SDXC */
  return info.LogBlockNbr;
}

uint32_t sd_card_last_error(sd_card_id_t id) {
  const sd_card_t *c = _card(id);
  return (c != NULL) ? c->last_error : 0U;
}

bool sd_card_read_blocks(sd_card_id_t id, uint8_t *buf, uint32_t block,
                         uint32_t count) {
  sd_card_t *c = _card(id);
  if ((c == NULL) || !c->ready || (buf == NULL) || (count == 0U)) {
    return false;
  }

  if (_aligned(buf)) {
    return _read(c, buf, block, count);
  }

  for (uint32_t i = 0U; i < count; i++) {
    if (!_read(c, s_scratch, block + i, 1U)) {
      return false;
    }
    memcpy(&buf[i * SD_CARD_BLOCK_SIZE], s_scratch, SD_CARD_BLOCK_SIZE);
  }
  return true;
}

bool sd_card_write_blocks(sd_card_id_t id, const uint8_t *buf, uint32_t block,
                          uint32_t count) {
  sd_card_t *c = _card(id);
  if ((c == NULL) || !c->ready || (buf == NULL) || (count == 0U)) {
    return false;
  }

  if (_aligned(buf)) {
    return _write(c, buf, block, count);
  }

  for (uint32_t i = 0U; i < count; i++) {
    memcpy(s_scratch, &buf[i * SD_CARD_BLOCK_SIZE], SD_CARD_BLOCK_SIZE);
    if (!_write(c, s_scratch, block + i, 1U)) {
      return false;
    }
  }
  return true;
}

/* ===========================================================================
 * Callbacks del HAL (desde SDMMCx_IRQHandler)
 * ======================================================================== */

void HAL_SD_RxCpltCallback(SD_HandleTypeDef *hsd) {
  sd_card_t *c = _card_from_handle(hsd);
  if (c != NULL) {
    c->rx_done = true;
  }
}

void HAL_SD_TxCpltCallback(SD_HandleTypeDef *hsd) {
  sd_card_t *c = _card_from_handle(hsd);
  if (c != NULL) {
    c->tx_done = true;
  }
}

void HAL_SD_ErrorCallback(SD_HandleTypeDef *hsd) {
  sd_card_t *c = _card_from_handle(hsd);
  if (c != NULL) {
    c->failed = true;
  }
}
