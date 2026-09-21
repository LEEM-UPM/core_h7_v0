/**
 * @file sounds.h
 * @brief Melodías y avisos sonoros con el zumbador (Drivers/Buzzer).
 */

#ifndef SOUNDS_H
#define SOUNDS_H

/* Poner a 0 para arrancar sin melodía (tarda unos 13 s) */
#define SOUNDS_STARTUP_ENABLE 1

/* Bella Ciao al arrancar (bloqueante) */
void sounds_startup(void);

#endif /* SOUNDS_H */
