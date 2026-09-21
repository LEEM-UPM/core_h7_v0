#include "sounds.h"

#include <stddef.h>
#include <stdint.h>

#include "buzzer.h"
#include "main.h"

/* Frecuencias [Hz], redondeadas (La4 = 440 Hz) */
#define NOTE_E4 330U /* mi */
#define NOTE_A4 440U /* la */
#define NOTE_B4 494U /* si */
#define NOTE_C5 523U /* DO */
#define NOTE_D5 587U /* RE */
#define NOTE_E5 659U /* MI */
#define NOTE_F5 698U /* FA */

#define STARTUP_SONG_BPM 130U
#define SIXTEENTH_MS     (60000U / STARTUP_SONG_BPM / 4U) /* ~115 ms */

/* Silencio al final de cada nota para que se distingan las repetidas */
#define NOTE_GAP_MS 20U

typedef struct {
  uint16_t freq_hz;
  uint8_t sixteenths; /* duración en semicorcheas (2 = corchea, 4 = negra) */
} sound_note_t;

#define REST 0U /* silencio */

/* Bella Ciao, según la partitura en 2/4 (4 corcheas por compás), desde la
 * anacrusa hasta el primer La (blanca) de la tercera línea.
 * Duraciones: 2 = corchea, 4 = negra, 6 = negra con puntillo, 8 = blanca. */
static const sound_note_t s_bella_ciao[] = {
    /* Línea 1 */
    {NOTE_E4, 2}, {NOTE_A4, 2}, {NOTE_B4, 2},                /* anacrusa  */
    {NOTE_C5, 2}, {NOTE_A4, 6},                              /* Do' La.   */
    {REST, 2}, {NOTE_E4, 2}, {NOTE_A4, 2}, {NOTE_B4, 2},     /* - Mi La Si */
    {NOTE_C5, 2}, {NOTE_A4, 6},                              /* Do' La.   */
    {REST, 2}, {NOTE_E4, 2}, {NOTE_A4, 2}, {NOTE_B4, 2},     /* - Mi La Si */
    {NOTE_C5, 4}, {NOTE_B4, 2}, {NOTE_A4, 2},                /* Do' Si La */
    {NOTE_C5, 4}, {NOTE_B4, 2}, {NOTE_A4, 2},                /* Do' Si La */

    /* Línea 2 */
    {NOTE_E5, 4}, {NOTE_E5, 4},                              /* Mi' Mi'   */
    {NOTE_E5, 4}, {NOTE_D5, 2}, {NOTE_E5, 2},                /* Mi' Re' Mi' */
    {NOTE_F5, 2}, {NOTE_F5, 6},                              /* Fa' Fa'.  */
    {REST, 2}, {NOTE_F5, 2}, {NOTE_E5, 2}, {NOTE_D5, 2},     /* - Fa' Mi' Re' */
    {NOTE_F5, 2}, {NOTE_E5, 6},                              /* Fa' Mi'.  */
    {REST, 4}, {NOTE_D5, 2}, {NOTE_C5, 2},                   /* -- Re' Do' */

    /* Línea 3, hasta el primer La */
    {NOTE_B4, 4}, {NOTE_E5, 4},                              /* Si Mi'    */
    {NOTE_B4, 4}, {NOTE_C5, 4},                              /* Si Do'    */
    {NOTE_A4, 8},                                            /* La (blanca) */
};

static void _play(const sound_note_t *notes, size_t count) {
  for (size_t i = 0U; i < count; i++) {
    const uint32_t duration_ms = notes[i].sixteenths * SIXTEENTH_MS;
    if (notes[i].freq_hz == REST) {
      HAL_Delay(duration_ms);
      continue;
    }
    buzzer_beep(notes[i].freq_hz, duration_ms - NOTE_GAP_MS);
    HAL_Delay(NOTE_GAP_MS);
  }
}

void sounds_startup(void) {
#if SOUNDS_STARTUP_ENABLE
  _play(s_bella_ciao, sizeof(s_bella_ciao) / sizeof(s_bella_ciao[0]));
#endif
}
