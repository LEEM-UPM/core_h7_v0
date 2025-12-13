#include "gpio.h"
#include "main.h"
#include "stm32h7xx_hal_gpio.h"

extern void SystemClock_Config(void);
extern void PeriphCommonClock_Config(void);

int main(void) {
  HAL_Init();
  SystemClock_Config();
  PeriphCommonClock_Config();
  MX_GPIO_Init();

  while (1) {
    HAL_GPIO_TogglePin(LED_1_GPIO_Port, LED_1_Pin);
    HAL_Delay(500);
  }
}