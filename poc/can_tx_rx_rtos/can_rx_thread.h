/*
 * can_rx_thread.h
 *
 * Recepción CAN con ring buffer SPSC + semáforo ThreadX.
 *
 * Arquitectura de dos capas:
 *
 *  Capa 1 — ISR (HAL_FDCAN_RxFifo0Callback)
 *    · Escribe la trama en el siguiente slot libre del ring buffer.
 *    · Avanza _rb_head (único escritor: no necesita lock).
 *    · Señaliza _can_rx_sem para despertar al hilo consumidor.
 *    · Latencia mínima, sin bloqueos.
 *
 *  Capa 2 — _can_rx_thread_entry (hilo ThreadX)
 *    · Espera en tx_semaphore_get().
 *    · Lee el slot apuntado por _rb_tail, parsea el frame,
 *      actualiza g_can_shared bajo mutex y avanza _rb_tail.
 *    · Único consumidor: tampoco necesita lock sobre el índice.
 *
 * Garantías SPSC:
 *    · _rb_head solo lo escribe la ISR.
 *    · _rb_tail solo lo escribe el hilo.
 *    · Ambos índices son volatile uint32_t: visibilidad garantizada
 *      sin necesidad de mutex adicional sobre el buffer.
 *    · El semáforo actúa como señal de "hay trabajo", no como mutex.
 */

#ifndef CAN_RX_THREAD_H
#define CAN_RX_THREAD_H

#include "tx_api.h"
#include "fdcan.h"

/* ------------------------------------------------------------------ */
/*  Parámetros configurables                                           */
/* ------------------------------------------------------------------ */

/*
 * Profundidad del ring buffer. DEBE ser potencia de 2 para que el
 * wrap-around sea un AND en lugar de un módulo (más rápido en ISR).
 */
#define CAN_RX_RB_DEPTH             16U  /* Ha de ser potencia de 2    */
#define CAN_RX_RB_MASK              (CAN_RX_RB_DEPTH - 1U)

#define CAN_FD_MAX_DLC              64U

#define CAN_RX_THREAD_STACK_SIZE    2048U
#define CAN_RX_THREAD_PRIO          5U
#define CAN_RX_THREAD_PREEMPT_THR   CAN_RX_THREAD_PRIO

/* ------------------------------------------------------------------ */
/*  Estructura de frame CAN                                            */
/* ------------------------------------------------------------------ */

/*
 * Únicamente los campos que el parser necesita.
 * Tamaño: 4 + 4 + 64 = 72 bytes → sin restricción de tamaño al
 * no depender de TX_QUEUE (que limita a 16 palabras = 64 bytes).
 */
typedef struct {
    uint32_t id;
    uint32_t dlc_bytes;
    uint8_t  data[CAN_FD_MAX_DLC];
} can_rx_frame_t;

/* ------------------------------------------------------------------ */
/*  API pública                                                        */
/* ------------------------------------------------------------------ */

/*
 * Inicializa el ring buffer, el semáforo, configura los filtros FDCAN
 * y arranca el hilo consumidor.
 * Llamar desde app_threadx_init() DESPUÉS de can_shared_data_init().
 */
UINT can_rx_thread_create(TX_BYTE_POOL *byte_pool,
                          FDCAN_HandleTypeDef *hfdcan);

/*
 * Devuelve el número de tramas descartadas por buffer lleno.
 * Útil para diagnóstico en tiempo de desarrollo.
 */
uint32_t can_rx_get_overflows(void);

/*
 * Callback FDCAN — sobreescribe la implementación débil del HAL.
 * NO llamar directamente.
 */
void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan,
                                uint32_t RxFifo0ITs);

#endif /* CAN_RX_THREAD_H */