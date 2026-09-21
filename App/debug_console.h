/*
 * Consola de debug del core por el puerto USB (PA11/PA12), que en el PC
 * aparece como un puerto COM virtual (Drivers/Debug_out).
 *
 *  - DebugConsole_Banner():  al arrancar, antes de la melodía y el wake up.
 *  - DebugConsole_Startup(): tras sys_init(), qué placas han respondido.
 *  - DebugConsole_Update():  en el bucle, un informe cada
 *    DEBUG_CONSOLE_PERIOD_MS con todo lo que el core sabe del sistema.
 *
 * Cada vez que se implemente algo nuevo en el core (mensajes, estado de
 * placas...), se añade su sección aquí.
 */

#ifndef DEBUG_CONSOLE_H
#define DEBUG_CONSOLE_H

#include "sys_utilities.h"

#define DEBUG_CONSOLE_PERIOD_MS 1000U

/* 1 = colores ANSI (PuTTY, Tera Term, minicom...). Poner a 0 si el terminal
 * muestra caracteres raros como "[32m" (p. ej. el monitor serie de Arduino). */
#define DEBUG_CONSOLE_USE_COLOR 0

void DebugConsole_Banner(void);
void DebugConsole_WakeUpStart(void); /* justo antes de sys_init() */
void DebugConsole_Startup(const sys_state *state);
void DebugConsole_Update(const sys_state *state);

#endif /* DEBUG_CONSOLE_H */
