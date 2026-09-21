/**
 * @file buzzer.h
 * @brief Zumbador de la placa (red ALARM, PE4 = TIM15_CH1N).
 *
 * PWM al 50 % con la frecuencia pedida. TIM15 no está activado en CubeMX: se
 * configura aquí (el pin PE4 en AF4 sí lo deja MX_GPIO_Init()).
 */

#ifndef BUZZER_H
#define BUZZER_H

#include <stdint.h>

#define BUZZER_MIN_FREQ_HZ 20U
#define BUZZER_MAX_FREQ_HZ 20000U

void buzzer_init(void);

/* Suena a 'freq_hz' hasta que se cambie o se llame a buzzer_off().
 * 0 apaga. Fuera de rango se limita a BUZZER_MIN/MAX_FREQ_HZ. */
void buzzer_tone(uint32_t freq_hz);
void buzzer_off(void);

/* Tono durante 'duration_ms' y apaga (bloqueante) */
void buzzer_beep(uint32_t freq_hz, uint32_t duration_ms);

#endif /* BUZZER_H */
