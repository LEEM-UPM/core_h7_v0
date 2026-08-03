/*
 * can_shared_data.h  —  versión prueba de validación
 *
 * Sustituye a la versión de producción que depende de Navigation_types.h.
 * Usa can_rx_data_t definido en can_test_types.h.
 */

#ifndef CAN_SHARED_DATA_H
#define CAN_SHARED_DATA_H

#include "tx_api.h"
#include "can_test_types.h"

/*
 * Instancia global de los datos compartidos.
 * Escrita por el hilo CAN RX, leída por el hilo de test.
 */
extern can_rx_data_t g_can_shared;

#define CAN_SHARED_MUTEX_TIMEOUT_TICKS  10U

UINT can_shared_data_init(void);
UINT can_shared_data_lock(void);
void can_shared_data_unlock(void);

#endif /* CAN_SHARED_DATA_H */