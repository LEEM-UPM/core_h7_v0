/*
 * can_tx_test.h
 *
 * Hilo de transmisión CAN para prueba de loopback FDCAN1 → FDCAN2.
 */

#ifndef CAN_TX_TEST_H
#define CAN_TX_TEST_H

#include "tx_api.h"
#include "fdcan.h"
#include <stdbool.h>

#define CAN_TX_THREAD_STACK_SIZE   1024U
#define CAN_TX_THREAD_PRIO         10U   /* menor prioridad que el RX */
#define CAN_TX_THREAD_PREEMPT_THR  CAN_TX_THREAD_PRIO

/*
 * Periodo de envío de cada ráfaga de mensajes.
 * Con tick de 1 ms → 100 ms entre ráfagas.
 */
#define CAN_TX_PERIOD_TICKS        100U

/* Número de ráfagas a enviar antes de declarar el test completo. */
#define CAN_TX_TEST_ITERATIONS     10U

UINT can_tx_test_create(TX_BYTE_POOL *byte_pool,
                        FDCAN_HandleTypeDef *hfdcan_tx);

/* Resultado del test: 0 = OK, distinto de 0 = número de errores. */
uint32_t can_tx_test_get_errors(void);
bool     can_tx_test_is_done(void);

#endif /* CAN_TX_TEST_H */