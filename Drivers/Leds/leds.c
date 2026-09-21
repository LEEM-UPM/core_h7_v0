#include "leds.h"

#include <string.h>

#include "main.h"

/*
 * Tiempos del protocolo. TIM4 está en APB1 (reloj de timer: 200 MHz), así que
 * con ARR = 249 cada periodo son 250 cuentas = 1,25 us (800 kbit/s):
 *   bit 0 -> pulso alto de 70 cuentas  (0,35 us)
 *   bit 1 -> pulso alto de 180 cuentas (0,90 us)
 * Valores dentro del margen del WL-ICLED y de los WS2812 típicos.
 */
#define LEDS_BIT_PERIOD_TICKS 250U
#define LEDS_TICK_HZ (800000U * LEDS_BIT_PERIOD_TICKS) /* 200 MHz */
#define LEDS_T0H_TICKS 70U
#define LEDS_T1H_TICKS 180U

#define LEDS_BITS_PER_LED 24U

/* Reset: la línea tiene que quedarse en bajo más de 50 us para que todos
 * apliquen el color. 48 bits a 0 son 60 us. */
#define LEDS_RESET_SLOTS 48U

#define LEDS_BUFFER_LEN ((LEDS_COUNT * LEDS_BITS_PER_LED) + LEDS_RESET_SLOTS)

#define LEDS_SHOW_TIMEOUT_MS 10U

static TIM_HandleTypeDef s_htim;
static DMA_HandleTypeDef s_hdma;

/*
 * Un valor de comparación por bit. En RAM_D1 (0x24000000), a donde DMA1 sí
 * llega; la caché de datos está desactivada, así que no hace falta
 * mantenimiento de caché.
 */
static uint32_t s_buffer[LEDS_BUFFER_LEN];

/* Color pedido de cada LED, en el orden del cable: G, R, B */
static uint8_t s_grb[LEDS_COUNT][3];

static volatile bool s_busy;
static bool s_ready;

/* Reloj de TIM4 (APB1): el doble de PCLK1 si el divisor de APB1 no es 1 */
static uint32_t _timer_clock_hz(void) {
  const uint32_t pclk1 = HAL_RCC_GetPCLK1Freq();
  return ((RCC->D2CFGR & RCC_D2CFGR_D2PPRE1_Msk) == 0U) ? pclk1 : (2U * pclk1);
}

/* Traduce los colores a un valor de comparación por bit, bit más
 * significativo primero, y deja el reset final a 0. */
static void _fill_buffer(void) {
  uint32_t slot = 0U;

  for (uint32_t led = 0U; led < LEDS_COUNT; led++) {
    for (uint32_t byte = 0U; byte < 3U; byte++) {
      const uint8_t value = s_grb[led][byte];
      for (int bit = 7; bit >= 0; bit--) {
        s_buffer[slot++] =
            ((value >> bit) & 1U) ? LEDS_T1H_TICKS : LEDS_T0H_TICKS;
      }
    }
  }

  while (slot < LEDS_BUFFER_LEN) {
    s_buffer[slot++] = 0U;
  }
}

static bool _dma_init(void) {
  __HAL_RCC_DMA1_CLK_ENABLE();

  s_hdma.Instance = DMA1_Stream0;
  s_hdma.Init.Request = DMA_REQUEST_TIM4_CH1;
  s_hdma.Init.Direction = DMA_MEMORY_TO_PERIPH;
  s_hdma.Init.PeriphInc = DMA_PINC_DISABLE;
  s_hdma.Init.MemInc = DMA_MINC_ENABLE;
  s_hdma.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
  s_hdma.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
  s_hdma.Init.Mode = DMA_NORMAL;
  s_hdma.Init.Priority = DMA_PRIORITY_LOW;
  s_hdma.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
  if (HAL_DMA_Init(&s_hdma) != HAL_OK) {
    return false;
  }

  __HAL_LINKDMA(&s_htim, hdma[TIM_DMA_ID_CC1], s_hdma);

  HAL_NVIC_SetPriority(DMA1_Stream0_IRQn, 7, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream0_IRQn);
  return true;
}

bool leds_init(void) {
  /* PD12 (LED_PWM) en AF2 = TIM4_CH1 */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  GPIO_InitTypeDef gpio = {0};
  gpio.Pin = LED_PWM_Pin;
  gpio.Mode = GPIO_MODE_AF_PP;
  gpio.Pull = GPIO_NOPULL;
  gpio.Speed = GPIO_SPEED_FREQ_HIGH;
  gpio.Alternate = GPIO_AF2_TIM4;
  HAL_GPIO_Init(LED_PWM_GPIO_Port, &gpio);

  __HAL_RCC_TIM4_CLK_ENABLE();

  s_htim.Instance = TIM4;
  s_htim.Init.Prescaler = (_timer_clock_hz() / LEDS_TICK_HZ) - 1U;
  s_htim.Init.CounterMode = TIM_COUNTERMODE_UP;
  s_htim.Init.Period = LEDS_BIT_PERIOD_TICKS - 1U;
  s_htim.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  s_htim.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_PWM_Init(&s_htim) != HAL_OK) {
    return false;
  }

  TIM_OC_InitTypeDef oc = {0};
  oc.OCMode = TIM_OCMODE_PWM1;
  oc.Pulse = 0U;
  oc.OCPolarity = TIM_OCPOLARITY_HIGH;
  oc.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&s_htim, &oc, TIM_CHANNEL_1) != HAL_OK) {
    return false;
  }

  if (!_dma_init()) {
    return false;
  }

  s_ready = true;
  leds_clear();
  return leds_show();
}

void leds_set(uint32_t index, uint8_t r, uint8_t g, uint8_t b) {
  if (index >= LEDS_COUNT) {
    return;
  }
  s_grb[index][0] = g;
  s_grb[index][1] = r;
  s_grb[index][2] = b;
}

void leds_set_all(uint8_t r, uint8_t g, uint8_t b) {
  for (uint32_t i = 0U; i < LEDS_COUNT; i++) {
    leds_set(i, r, g, b);
  }
}

void leds_clear(void) { memset(s_grb, 0, sizeof(s_grb)); }

bool leds_show(void) {
  if (!s_ready) {
    return false;
  }

  _fill_buffer();

  s_busy = true;
  if (HAL_TIM_PWM_Start_DMA(&s_htim, TIM_CHANNEL_1, s_buffer,
                            (uint16_t)LEDS_BUFFER_LEN) != HAL_OK) {
    s_busy = false;
    return false;
  }

  const uint32_t start = HAL_GetTick();
  while (s_busy) {
    if ((HAL_GetTick() - start) >= LEDS_SHOW_TIMEOUT_MS) {
      HAL_TIM_PWM_Stop_DMA(&s_htim, TIM_CHANNEL_1);
      s_busy = false;
      return false;
    }
  }
  return true;
}

void leds_hue_to_rgb(uint8_t hue, uint8_t *r, uint8_t *g, uint8_t *b) {
  /* Tres sectores de 85: rojo -> verde -> azul -> rojo */
  const uint8_t sector = (uint8_t)(hue / 85U);
  const uint8_t ramp = (uint8_t)(((hue % 85U) * 255U) / 84U);

  switch (sector) {
  case 0U:
    *r = (uint8_t)(255U - ramp);
    *g = ramp;
    *b = 0U;
    break;
  case 1U:
    *r = 0U;
    *g = (uint8_t)(255U - ramp);
    *b = ramp;
    break;
  default:
    *r = ramp;
    *g = 0U;
    *b = (uint8_t)(255U - ramp);
    break;
  }
}

/* Fin de la trama: el DMA ha soltado el último bit */
void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim) {
  if (htim->Instance == TIM4) {
    HAL_TIM_PWM_Stop_DMA(&s_htim, TIM_CHANNEL_1);
    s_busy = false;
  }
}

/* CubeMX no genera este handler: el DMA de TIM4_CH1 se configura aquí */
void DMA1_Stream0_IRQHandler(void) { HAL_DMA_IRQHandler(&s_hdma); }
