/*
 * Gestión de la placa de RF desde el core (ICD, 6.10).
 *
 * Por ahora solo transmisión: el core manda CMD_RF_TRANSMIT (0x140) con los
 * bytes que quiere poner en el aire y la placa de RF los envía tal cual en un
 * paquete LoRa, sin interpretarlos. La recepción está pendiente.
 *
 * Uso interno de sys_utilities.c; la función pública (sys_rf_transmit) se
 * declara en sys_utilities.h, que es lo que incluye el resto del código.
 */

#ifndef SYS_RF_H
#define SYS_RF_H

#include "sys_utilities.h"

/* Estado inicial. Llamar desde sys_init(), al terminar el wake up. */
void sys_rf_init(sys_state *state);

#endif /* SYS_RF_H */
