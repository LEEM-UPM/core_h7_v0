/*
 * Gestión de la placa de aviónica desde el core: calibración de IMUs y
 * barómetros (ICD, 6.8 y 6.9).
 *
 * Uso interno de sys_utilities.c. La función pública (sys_avionics_calibrate)
 * se declara en sys_utilities.h, que es lo que incluye el resto del código.
 */

#ifndef SYS_AVIONICS_H
#define SYS_AVIONICS_H

#include <stdbool.h>

#include "can_driver.h"
#include "sys_utilities.h"

/* Estado inicial. Llamar desde sys_init(), al terminar el wake up. */
void sys_avionics_init(sys_state *state);

/* Tareas periódicas (timeout de la calibración). */
void sys_avionics_update(sys_state *state);

/* Trata la trama si es de aviónica. Devuelve true si la ha tratado. */
bool sys_avionics_process_frame(sys_state *state, const can_rx_frame_t *frame);

#endif /* SYS_AVIONICS_H */
