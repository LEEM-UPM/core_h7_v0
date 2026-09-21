/*
 * Salida de texto de debug por el puerto serie virtual USB (usb_cdc.h), sin
 * bloquear.
 *
 * DebugOut_Printf() formatea en un buffer local y copia el texto a un buffer
 * circular; se va enviando por USB en trozos, y cada vez que termina uno
 * (interrupción del USB) arranca el siguiente. El bucle principal nunca
 * espera al USB.
 *
 * Si no hay PC conectado o el buffer está lleno, la línea entera se descarta
 * y se cuenta en DebugOut_DroppedBytes(). Lo que ya estaba en el buffer se
 * envía en cuanto el PC abre el puerto.
 */

#ifndef DEBUG_OUT_H
#define DEBUG_OUT_H

#include <stddef.h>
#include <stdint.h>

/* Mayor línea que admite DebugOut_Printf() */
#define DEBUG_OUT_LINE_MAX 256U

/* Arranca el USB. Llamar una vez tras HAL_Init() y SystemClock_Config(). */
void DebugOut_Init(void);

/* Encola `len` bytes. Devuelve los encolados: `len` o 0 si no caben. */
size_t DebugOut_Write(const char *data, size_t len);

/* printf al puerto de debug (línea de hasta DEBUG_OUT_LINE_MAX bytes).
 * Admite %f. */
void DebugOut_Printf(const char *format, ...)
    __attribute__((format(printf, 1, 2)));

/* Bytes descartados por buffer lleno desde el arranque */
uint32_t DebugOut_DroppedBytes(void);

#endif /* DEBUG_OUT_H */
