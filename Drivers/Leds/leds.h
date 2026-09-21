/**
 * @file leds.h
 * @brief Los 6 LEDs RGB direccionables de la placa (Würth WL-ICLED
 *        1312020030000, protocolo tipo WS2812).
 *
 * Están en cadena: el micro solo pilota el DIN del primero por la red
 * LED_PWM (PD12 = TIM4_CH1) y cada LED se queda con sus 24 bits (G, R, B) y
 * reenvía el resto al siguiente por su DOUT.
 *
 * Cada bit dura 1,25 us (800 kbit/s) y su valor está en lo que dura el pulso
 * alto: corto = 0, largo = 1. Se genera con TIM4_CH1 en PWM y DMA, así que la
 * CPU no tiene que contar microsegundos; leds_show() bloquea el tiempo que
 * tarda la trama (~0,2 ms con 6 LEDs) y el color se queda fijo hasta la
 * siguiente llamada.
 *
 * TIM4 no lo configura CubeMX con estos tiempos: leds_init() lo hace entero
 * (igual que el zumbador con TIM15), y MX_TIM4_Init() no se llama.
 */

#ifndef LEDS_H
#define LEDS_H

#include <stdbool.h>
#include <stdint.h>

/* LEDs de la cadena: D13, D15, D17... hasta 6 */
#define LEDS_COUNT 6U

/* Arranca TIM4_CH1 + DMA y apaga todos los LEDs. */
bool leds_init(void);

/* Color de un LED de la cadena (0 = el primero). No se ve hasta leds_show(). */
void leds_set(uint32_t index, uint8_t r, uint8_t g, uint8_t b);
void leds_set_all(uint8_t r, uint8_t g, uint8_t b);
void leds_clear(void);

/* Envía la trama a la cadena. Bloquea hasta terminar (~0,25 ms). */
bool leds_show(void);

/* Color a partir de un tono (0-255 recorre todo el círculo), a máximo brillo.
 * Útil para arcoíris: evita tener que escribir tablas de colores. */
void leds_hue_to_rgb(uint8_t hue, uint8_t *r, uint8_t *g, uint8_t *b);

#endif /* LEDS_H */
