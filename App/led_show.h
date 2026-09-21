/**
 * @file led_show.h
 * @brief Animaciones en los 6 LEDs RGB de la placa (Drivers/Leds).
 */

#ifndef LED_SHOW_H
#define LED_SHOW_H

/* Poner a 0 para arrancar sin animación (dura unos 4 s) */
#define LED_SHOW_STARTUP_ENABLE 1

/* Brillo máximo de las animaciones, 0-255. A 255 deslumbran bastante. */
#define LED_SHOW_BRIGHTNESS 150U

/* Secuencia de arranque, antes de la melodía (bloqueante). */
void led_show_startup(void);

#endif /* LED_SHOW_H */
