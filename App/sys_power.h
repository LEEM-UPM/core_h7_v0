/*
 * Gestión de la placa de potencia desde el core: continuidad de los
 * pirotécnicos, órdenes de encendido y tensiones/corrientes de los raíles
 * (ICD, 6.3 a 6.7).
 *
 * Uso interno de sys_utilities.c. Las funciones públicas (sys_pyro_fire_*) se
 * declaran en sys_utilities.h, que es lo que incluye el resto del código.
 */

#ifndef SYS_POWER_H
#define SYS_POWER_H

#include <stdbool.h>

#include "can_driver.h"
#include "sys_utilities.h"

/* Estado inicial. Llamar desde sys_init(), al terminar el wake up. */
void sys_power_init(sys_state *state);

/* Tareas periódicas (petición de continuidad a 1 Hz). */
void sys_power_update(sys_state *state);

/* Trata la trama si es de power. Devuelve true si la ha tratado. */
bool sys_power_process_frame(sys_state *state, const can_rx_frame_t *frame);

#endif /* SYS_POWER_H */
