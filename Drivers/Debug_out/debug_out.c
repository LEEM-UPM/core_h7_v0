#include "debug_out.h"

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>

#include "main.h"
#include "usb_cdc.h"

/* Tamaño del buffer circular: potencia de 2 */
#define DEBUG_OUT_BUFFER_SIZE 8192U
#define DEBUG_OUT_BUFFER_MASK (DEBUG_OUT_BUFFER_SIZE - 1U)

/* Bytes por transferencia USB (se reparten en paquetes de 64) */
#define DEBUG_OUT_CHUNK_MAX 512U

/*
 * Buffer circular entre el bucle principal y la interrupción del USB.
 * s_head: lo escribe SOLO el bucle principal (DebugOut_Write).
 * s_tail: lo escribe SOLO _kick(), con la interrupción del USB bloqueada o
 * desde la propia interrupción.
 * Contadores monótonos: ocupado = s_head - s_tail.
 */
static char s_buffer[DEBUG_OUT_BUFFER_SIZE];
static volatile uint32_t s_head = 0U;
static volatile uint32_t s_tail = 0U;

/* Transferencia en curso y su longitud */
static volatile bool s_tx_busy = false;
static volatile uint32_t s_tx_len = 0U;

static volatile uint32_t s_dropped_bytes = 0U;

/* Arranca el siguiente trozo si el USB está libre. Llamar desde la
 * interrupción del USB o con las interrupciones deshabilitadas. */
static void _kick(void) {
  if (s_tx_busy || (s_head == s_tail)) {
    return;
  }

  const uint32_t tail = s_tail & DEBUG_OUT_BUFFER_MASK;
  uint32_t len = s_head - s_tail;

  /* Solo bytes contiguos: sin dar la vuelta al final del buffer */
  if (len > (DEBUG_OUT_BUFFER_SIZE - tail)) {
    len = DEBUG_OUT_BUFFER_SIZE - tail;
  }
  if (len > DEBUG_OUT_CHUNK_MAX) {
    len = DEBUG_OUT_CHUNK_MAX;
  }

  if (UsbCdc_Transmit((uint8_t *)&s_buffer[tail], (uint16_t)len) ==
      USB_CDC_TX_OK) {
    s_tx_len = len;
    s_tx_busy = true;
  }
}

/* Interrupción del USB: terminó la transferencia en curso */
static void _on_tx_done(void) {
  if (s_tx_busy) {
    s_tail += s_tx_len;
    s_tx_busy = false;
  }
  _kick();
}

void DebugOut_Init(void) { UsbCdc_Init(_on_tx_done); }

size_t DebugOut_Write(const char *data, size_t len) {
  if ((data == NULL) || (len == 0U)) {
    return 0U;
  }

  const uint32_t free_bytes = DEBUG_OUT_BUFFER_SIZE - (s_head - s_tail);
  if (len > free_bytes) {
    s_dropped_bytes += (uint32_t)len;
    return 0U;
  }

  uint32_t head = s_head;
  for (size_t i = 0U; i < len; i++) {
    s_buffer[(head + i) & DEBUG_OUT_BUFFER_MASK] = data[i];
  }
  s_head = head + (uint32_t)len;

  uint32_t primask = __get_PRIMASK();
  __disable_irq();
  _kick();
  __set_PRIMASK(primask);

  return len;
}

void DebugOut_Printf(const char *format, ...) {
  char line[DEBUG_OUT_LINE_MAX];

  va_list args;
  va_start(args, format);
  int n = vsnprintf(line, sizeof(line), format, args);
  va_end(args);

  if (n <= 0) {
    return;
  }
  if ((size_t)n >= sizeof(line)) {
    n = (int)(sizeof(line) - 1U); /* línea truncada */
  }
  (void)DebugOut_Write(line, (size_t)n);
}

uint32_t DebugOut_DroppedBytes(void) { return s_dropped_bytes; }
