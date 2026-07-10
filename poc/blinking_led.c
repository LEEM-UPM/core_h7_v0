#include "gpio.h"
#include "main.h"
#include "stm32h7xx_hal_gpio.h"
#include "core_cm7.h"

#define NUM_LEDS 6
#define PULSE_0 80
#define PULSE_1 170

extern void SystemClock_Config(void);
extern void PeriphCommonClock_Config(void);
extern TIM_HandleTypeDef htim4;

void Set_LED_Color(int index, uint8_t r, uint8_t g, uint8_t b);
void Send_PWM_Buffer(void);

uint16_t pwm_buffer[(24*NUM_LEDS) + 300] = {0};


int main(void) {
  HAL_Init();
  SystemClock_Config();
  
  PeriphCommonClock_Config();
  MX_GPIO_Init();

  for(int i = 0; i < NUM_LEDS; i++) {
      Set_LED_Color(i, 0, 0, 0); 
  }

  Send_PWM_Buffer();
  HAL_Delay(1000); 

  while (1) {
    // 1. Set all LEDs to Red
      for(int i = 0; i < NUM_LEDS; i++) {
          Set_LED_Color(i, 255, 0, 0);
      }
      Send_PWM_Buffer(); // Fire the DMA
      HAL_Delay(500);      // Wait 500ms

      // 2. Set all LEDs to Blue
      for(int i = 0; i < NUM_LEDS; i++) {
          Set_LED_Color(i, 0, 0, 255);
      }
      Send_PWM_Buffer(); // Fire the DMA
      HAL_Delay(500);
  }
}

void Set_LED_Color(int index, uint8_t r, uint8_t g, uint8_t b) {
    uint32_t color = (g << 16) | (r << 8) | b; // Formato GRB
    
    for (int i = 23; i >= 0; i--) {
        if (color & (1 << i)) {
            pwm_buffer[(index * 24) + (23 - i)] = PULSE_1;
        } else {
            pwm_buffer[(index * 24) + (23 - i)] = PULSE_0;
        }
    }
}

void Send_PWM_Buffer(void) {
    SCB_CleanDCache_by_Addr((uint32_t *)((uint32_t)pwm_buffer & ~(uint32_t)0x1F), sizeof(pwm_buffer) + 32);
    HAL_TIM_PWM_Start_DMA(&htim4, TIM_CHANNEL_1, (uint32_t *)pwm_buffer, sizeof(pwm_buffer)/sizeof(uint16_t));
    
}