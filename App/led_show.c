#include "led_show.h"

#include <stdint.h>

#include "leds.h"
#include "main.h"

/* Tiempos de cada parte de la secuencia */
#define WIPE_STEP_MS 70U     /* un LED cada 70 ms */
#define RAINBOW_STEP_MS 20U  /* giro del arcoíris */
#define RAINBOW_TURNS 2U     /* vueltas completas del arcoíris */
#define COMET_STEP_MS 45U    /* barrido del cometa */
#define COMET_PASSES 2U
#define FLASH_MS 90U
#define FADE_STEP_MS 25U
#define FADE_STEPS 12U

/* Escala un canal al brillo configurado */
static uint8_t _dim(uint8_t value, uint8_t scale) {
  return (uint8_t)(((uint32_t)value * scale) / 255U);
}

static void _set_scaled(uint32_t index, uint8_t r, uint8_t g, uint8_t b,
                        uint8_t scale) {
  leds_set(index, _dim(r, scale), _dim(g, scale), _dim(b, scale));
}

/* 1. Se van encendiendo uno a uno con los colores del arcoíris */
static void _rainbow_wipe(void) {
  leds_clear();
  for (uint32_t i = 0U; i < LEDS_COUNT; i++) {
    uint8_t r, g, b;
    leds_hue_to_rgb((uint8_t)((i * 255U) / LEDS_COUNT), &r, &g, &b);
    _set_scaled(i, r, g, b, LED_SHOW_BRIGHTNESS);
    (void)leds_show();
    HAL_Delay(WIPE_STEP_MS);
  }
}

/* 2. El arcoíris gira a lo largo de la tira */
static void _rainbow_cycle(void) {
  const uint32_t steps = RAINBOW_TURNS * 256U / 4U; /* 4 tonos por paso */
  for (uint32_t step = 0U; step < steps; step++) {
    for (uint32_t i = 0U; i < LEDS_COUNT; i++) {
      const uint8_t hue =
          (uint8_t)(((i * 255U) / LEDS_COUNT) + (step * 4U));
      uint8_t r, g, b;
      leds_hue_to_rgb(hue, &r, &g, &b);
      _set_scaled(i, r, g, b, LED_SHOW_BRIGHTNESS);
    }
    (void)leds_show();
    HAL_Delay(RAINBOW_STEP_MS);
  }
}

/* 3. Un cometa blanco recorre la tira dejando estela */
static void _comet(void) {
  for (uint32_t pass = 0U; pass < COMET_PASSES; pass++) {
    for (uint32_t head = 0U; head < (LEDS_COUNT + 2U); head++) {
      leds_clear();
      /* Cabeza a tope y dos de estela cada vez más apagadas */
      for (uint32_t tail = 0U; tail < 3U; tail++) {
        if (head < tail) {
          continue;
        }
        const uint32_t i = head - tail;
        if (i >= LEDS_COUNT) {
          continue;
        }
        const uint8_t scale =
            (uint8_t)(LED_SHOW_BRIGHTNESS >> (tail * 2U)); /* 100 %, 25 %, 6 % */
        _set_scaled(i, 255U, 255U, 255U, scale);
      }
      (void)leds_show();
      HAL_Delay(COMET_STEP_MS);
    }
  }
}

/* 4. Destello verde (todo listo) y fundido a negro */
static void _flash_and_fade(void) {
  leds_set_all(_dim(0U, LED_SHOW_BRIGHTNESS), LED_SHOW_BRIGHTNESS, 0U);
  (void)leds_show();
  HAL_Delay(FLASH_MS);

  for (uint32_t step = FADE_STEPS; step > 0U; step--) {
    const uint8_t scale =
        (uint8_t)((LED_SHOW_BRIGHTNESS * step) / FADE_STEPS);
    leds_set_all(0U, scale, 0U);
    (void)leds_show();
    HAL_Delay(FADE_STEP_MS);
  }

  leds_clear();
  (void)leds_show();
}

void led_show_startup(void) {
#if LED_SHOW_STARTUP_ENABLE
  if (!leds_init()) {
    return;
  }

  _rainbow_wipe();
  _rainbow_cycle();
  _comet();
  _flash_and_fade();
#endif
}
