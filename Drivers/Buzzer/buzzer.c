#include "buzzer.h"

#include "main.h"

/* El contador avanza a 1 MHz: ARR = 1e6 / f - 1 cabe en 16 bits desde 16 Hz */
#define BUZZER_TICK_HZ 1000000U

static TIM_HandleTypeDef s_htim15;

/* Reloj de TIM15 (APB2): el doble de PCLK2 si el divisor de APB2 no es 1 */
static uint32_t _timer_clock_hz(void) {
  const uint32_t pclk2 = HAL_RCC_GetPCLK2Freq();
  return ((RCC->D2CFGR & RCC_D2CFGR_D2PPRE2_Msk) == 0U) ? pclk2 : (2U * pclk2);
}

void buzzer_init(void) {
  __HAL_RCC_TIM15_CLK_ENABLE();

  s_htim15.Instance = TIM15;
  s_htim15.Init.Prescaler = (_timer_clock_hz() / BUZZER_TICK_HZ) - 1U;
  s_htim15.Init.CounterMode = TIM_COUNTERMODE_UP;
  s_htim15.Init.Period = (BUZZER_TICK_HZ / 1000U) - 1U;
  s_htim15.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  s_htim15.Init.RepetitionCounter = 0U;
  s_htim15.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_PWM_Init(&s_htim15) != HAL_OK) {
    Error_Handler();
  }

  /* Solo se usa la salida complementaria CH1N; sin tiempo muerto ni break */
  TIM_OC_InitTypeDef oc = {0};
  oc.OCMode = TIM_OCMODE_PWM1;
  oc.Pulse = 0U; /* ciclo 0 %: salida a nivel bajo, en silencio */
  oc.OCPolarity = TIM_OCPOLARITY_HIGH;
  oc.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  oc.OCFastMode = TIM_OCFAST_DISABLE;
  oc.OCIdleState = TIM_OCIDLESTATE_RESET;
  oc.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&s_htim15, &oc, TIM_CHANNEL_1) != HAL_OK) {
    Error_Handler();
  }

  /* Con OSSR activo la salida sigue controlada por el timer (a nivel bajo)
   * cuando no suena, en vez de quedar flotando */
  TIM_BreakDeadTimeConfigTypeDef bdtr = {0};
  bdtr.OffStateRunMode = TIM_OSSR_ENABLE;
  bdtr.OffStateIDLEMode = TIM_OSSI_ENABLE;
  bdtr.LockLevel = TIM_LOCKLEVEL_OFF;
  bdtr.DeadTime = 0U;
  bdtr.BreakState = TIM_BREAK_DISABLE;
  bdtr.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  bdtr.BreakFilter = 0U;
  bdtr.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&s_htim15, &bdtr) != HAL_OK) {
    Error_Handler();
  }

  if (HAL_TIMEx_PWMN_Start(&s_htim15, TIM_CHANNEL_1) != HAL_OK) {
    Error_Handler();
  }
}

void buzzer_tone(uint32_t freq_hz) {
  if (freq_hz == 0U) {
    buzzer_off();
    return;
  }
  if (freq_hz < BUZZER_MIN_FREQ_HZ) {
    freq_hz = BUZZER_MIN_FREQ_HZ;
  } else if (freq_hz > BUZZER_MAX_FREQ_HZ) {
    freq_hz = BUZZER_MAX_FREQ_HZ;
  }

  const uint32_t period = (BUZZER_TICK_HZ + (freq_hz / 2U)) / freq_hz;
  __HAL_TIM_SET_AUTORELOAD(&s_htim15, period - 1U);
  __HAL_TIM_SET_COMPARE(&s_htim15, TIM_CHANNEL_1, period / 2U);
}

void buzzer_off(void) { __HAL_TIM_SET_COMPARE(&s_htim15, TIM_CHANNEL_1, 0U); }

void buzzer_beep(uint32_t freq_hz, uint32_t duration_ms) {
  buzzer_tone(freq_hz);
  HAL_Delay(duration_ms);
  buzzer_off();
}
