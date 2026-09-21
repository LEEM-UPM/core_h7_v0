#include "debug_console.h"

#include <math.h>
#include <stdbool.h>

#include "can_driver.h"
#include "can_protocol/can_protocol_version.h"
#include "debug_out.h"
#include "fx_api.h"
#include "main.h"
#include "sensor_board.h"
#include "sounds.h"
#include "storage.h"

#if DEBUG_CONSOLE_USE_COLOR
#define C_RESET "\033[0m"
#define C_TITLE "\033[1;36m" /* cian negrita */
#define C_OK    "\033[32m"   /* verde */
#define C_BAD   "\033[31m"   /* rojo */
#define C_WARN  "\033[33m"   /* amarillo */
#define C_DIM   "\033[90m"   /* gris */
#else
#define C_RESET ""
#define C_TITLE ""
#define C_OK    ""
#define C_BAD   ""
#define C_WARN  ""
#define C_DIM   ""
#endif

#define LINE_THICK "=================================================================\r\n"
#define LINE_THIN  "-----------------------------------------------------------------\r\n"

#define P DebugOut_Printf

static uint32_t _last_report_ms;
static uint32_t _report_count;

/* ===========================================================================
 * Formato de valores
 * ======================================================================== */

/* "OK" en verde / "--" en rojo */
static const char *_flag(bool ok) { return ok ? C_OK "OK" C_RESET : C_BAD "--" C_RESET; }

/* Antigüedad de un dato: "hace 0.4 s" o "sin datos" */
static void _age(bool valid, uint32_t rx_ms) {
  if (!valid) {
    P(C_DIM "(sin datos)" C_RESET "\r\n");
    return;
  }
  uint32_t age_ms = HAL_GetTick() - rx_ms;
  P("%s(hace %lu.%lu s)" C_RESET "\r\n", (age_ms > 3000U) ? C_WARN : C_DIM,
    (unsigned long)(age_ms / 1000U), (unsigned long)((age_ms % 1000U) / 100U));
}

/* Campo uint16 de POWER_RAILS: número o "----" si es 0xFFFF */
static void _rail_field(uint16_t value) {
  if (value == SYS_POWER_RAILS_INVALID) {
    P(C_BAD "  ----" C_RESET);
  } else {
    P("%6u", value);
  }
}

/* Terna X Y Z en float; NaN -> "---" */
static void _xyz(const float v[3], int decimals) {
  for (int i = 0; i < 3; i++) {
    if (isnan(v[i])) {
      P(C_BAD "    ---   " C_RESET);
    } else {
      P(" %+9.*f", decimals, (double)v[i]);
    }
  }
}

/* ===========================================================================
 * Secciones del informe
 * ======================================================================== */

static void _section_boards(const sys_state *s) {
  P(" " C_TITLE "PLACAS" C_RESET "    power %s   avionica %s   rf %s\r\n",
    _flag(s->power), _flag(s->avionics), _flag(s->rf));
}

/* Componentes de cada placa (INIT_STATUS, ICD 6.11) */
typedef struct {
  uint32_t bit;
  const char *name;
} _component_t;

static const _component_t _avionics_components[] = {
    {SYS_AVIONICS_INIT_BARO1, "baro1"},
    {SYS_AVIONICS_INIT_BARO2, "baro2"},
    {SYS_AVIONICS_INIT_IMU1, "imu1"},
    {SYS_AVIONICS_INIT_IMU2, "imu2"},
    {SYS_AVIONICS_INIT_MAG, "mag"},
    {SYS_AVIONICS_INIT_GPS, "gps"},
    {SYS_AVIONICS_INIT_VECTORNAV, "vn100"},
};

/* Una línea con cada componente y si ha arrancado. Los bits que la placa no
 * tiene (fuera de supported) no se enseñan. */
static void _components_line(const char *board, const sys_init_status *st,
                             const _component_t *table, uint32_t count) {
  P("   %-9s", board);

  if (!st->valid) {
    P(C_DIM "(sin INIT_STATUS)" C_RESET "\r\n");
    return;
  }

  for (uint32_t i = 0U; i < count; i++) {
    if ((st->supported & table[i].bit) == 0U) {
      continue;
    }
    const bool ok = (st->ok & table[i].bit) != 0U;
    P(" %s%s%s" C_RESET, ok ? C_OK : C_BAD, ok ? "" : "!", table[i].name);
  }

  P("\r\n");
}

static void _section_components(const sys_state *s) {
  P(LINE_THIN);
  P(" " C_TITLE "COMPONENTES INICIADOS" C_RESET "\r\n");
  _components_line("avionica", &s->avionics_init, _avionics_components,
                   sizeof(_avionics_components) /
                       sizeof(_avionics_components[0]));

  /* power y rf todavía no mandan INIT_STATUS: se enseña el mapa en crudo */
  if (s->power_init.valid) {
    P("   %-9s ok 0x%08lX / tiene 0x%08lX\r\n", "power",
      (unsigned long)s->power_init.ok, (unsigned long)s->power_init.supported);
  }
  if (s->rf_init.valid) {
    P("   %-9s ok 0x%08lX / tiene 0x%08lX\r\n", "rf",
      (unsigned long)s->rf_init.ok, (unsigned long)s->rf_init.supported);
  }
}

static void _section_power(const sys_state *s) {
  P(LINE_THIN);
  P(" " C_TITLE "POWER" C_RESET "  railes ");
  _age(s->power_rails_valid, s->power_rails_rx_ms);

  const sys_power_rails *r = &s->power_rails;
  P("          tension [mV]  corriente [mA]\r\n");
  P("   3V3  ");
  _rail_field(r->voltage_3v3_mv);
  P("        ");
  _rail_field(r->current_3v3_ma);
  P("\r\n   5V   ");
  _rail_field(r->voltage_5v_mv);
  P("        ");
  _rail_field(r->current_5v_ma);
  P("\r\n   11V  ");
  _rail_field(r->voltage_11v_mv);
  P("        ");
  _rail_field(r->current_11v_ma);
  P("\r\n");

  P("   Pirotecnicos (continuidad) ");
  if (s->pyro_continuity_valid) {
    P(" 1A %s  1B %s  2A %s  2B %s  ", _flag(s->pyro_continuity & SYS_PYRO_1A),
      _flag(s->pyro_continuity & SYS_PYRO_1B),
      _flag(s->pyro_continuity & SYS_PYRO_2A),
      _flag(s->pyro_continuity & SYS_PYRO_2B));
  }
  _age(s->pyro_continuity_valid, s->pyro_continuity_rx_ms);
}

static void _section_calibration(const sys_state *s) {
  static const char *const names[] = {
      [SYS_CALIBRATION_IDLE] = C_DIM "no pedida" C_RESET,
      [SYS_CALIBRATION_WAITING] = C_WARN "en curso..." C_RESET,
      [SYS_CALIBRATION_DONE] = C_OK "hecha" C_RESET,
      [SYS_CALIBRATION_TIMEOUT] = C_BAD "sin respuesta (timeout)" C_RESET,
  };

  P(LINE_THIN);
  P(" " C_TITLE "CALIBRACION AVIONICA" C_RESET "  %s\r\n",
    names[s->avionics_calibration_status]);

  if (s->avionics_calibration_status != SYS_CALIBRATION_DONE) {
    return;
  }

  const sys_avionics_calibration *c = &s->avionics_calibration;
  P("                     X          Y          Z\r\n");
  P("   IMU1 acc [m/s2] ");
  _xyz(c->imu1_accel_mps2, 3);
  P("\r\n   IMU1 gyr [rad/s]");
  _xyz(c->imu1_gyro_radps, 4);
  P("\r\n   IMU2 acc [m/s2] ");
  _xyz(c->imu2_accel_mps2, 3);
  P("\r\n   IMU2 gyr [rad/s]");
  _xyz(c->imu2_gyro_radps, 4);
  P("\r\n   Presion base [kPa]   baro1 %.3f   baro2 %.3f\r\n",
    (double)c->baro1_kpa, (double)c->baro2_kpa);
}

/* Coordenada en 1e-7 grados: "+40.4167890" */
static void _deg_e7(int32_t v) {
  if (v == SENSOR_BOARD_GPS_INVALID_I32) {
    P(C_BAD "     ---    " C_RESET);
    return;
  }
  const uint32_t a = (v < 0) ? (uint32_t)(-(int64_t)v) : (uint32_t)v;
  P("%c%3lu.%07lu", (v < 0) ? '-' : '+', (unsigned long)(a / 10000000UL),
    (unsigned long)(a % 10000000UL));
}

/* Milímetros con signo: "650.123" */
static void _mm(int32_t v) {
  if (v == SENSOR_BOARD_GPS_INVALID_I32) {
    P(C_BAD "---" C_RESET);
    return;
  }
  const uint32_t a = (v < 0) ? (uint32_t)(-(int64_t)v) : (uint32_t)v;
  P("%s%lu.%03lu", (v < 0) ? "-" : "", (unsigned long)(a / 1000UL),
    (unsigned long)(a % 1000UL));
}

static void _utc(uint32_t ms) {
  if (ms == SENSOR_BOARD_GPS_INVALID_TIME) {
    P(C_BAD "--:--:--.-" C_RESET);
    return;
  }
  P("%02lu:%02lu:%02lu.%lu", (unsigned long)(ms / 3600000UL),
    (unsigned long)((ms / 60000UL) % 60UL), (unsigned long)((ms / 1000UL) % 60UL),
    (unsigned long)((ms % 1000UL) / 100UL));
}

/* Tramas recibidas desde el informe anterior (un informe por segundo = Hz) */
static uint32_t _rate(uint32_t count, uint32_t *last) {
  const uint32_t r = count - *last;
  *last = count;
  return r;
}

static void _section_gps(const sensor_board_data_t *d) {
  static uint32_t last_gga;
  static uint32_t last_rmc;
  const gps_gga_data_t *g = &d->gps_gga;
  const gps_rmc_data_t *r = &d->gps_rmc;

  if ((g->count == 0U) && (r->count == 0U)) {
    P("   GPS      " C_DIM "(sin datos)" C_RESET "\r\n");
    return;
  }

  P("   GPS GGA  fix %s%u" C_RESET "  sat %2u  hdop ", (g->fix_quality > 0U) ? C_OK : C_BAD,
    g->fix_quality, g->satellites);
  if (g->hdop_x100 == SENSOR_BOARD_GPS_INVALID_U16) {
    P(C_BAD "----" C_RESET);
  } else {
    P("%u.%02u", g->hdop_x100 / 100U, g->hdop_x100 % 100U);
  }
  P("  lat ");
  _deg_e7(g->latitude_e7);
  P("  lon ");
  _deg_e7(g->longitude_e7);
  P("  alt ");
  _mm(g->altitude_mm);
  P(" m  ");
  _utc(g->time_ms);
  P(" UTC  (%lu, %lu Hz)\r\n", (unsigned long)g->count,
    (unsigned long)_rate(g->count, &last_gga));

  P("   GPS RMC  %s" C_RESET "  %02u/%02u/%04u  vel ", r->valid ? C_OK "valido" : C_BAD "no valido",
    r->day, r->month, r->year);
  _mm(r->speed_mmps);
  P(" m/s  rumbo ");
  if (r->course_cdeg == SENSOR_BOARD_GPS_INVALID_U16) {
    P(C_BAD "---" C_RESET);
  } else {
    P("%u.%02u", r->course_cdeg / 100U, r->course_cdeg % 100U);
  }
  P("  ");
  _utc(r->time_ms);
  P(" UTC  (%lu, %lu Hz)\r\n", (unsigned long)r->count,
    (unsigned long)_rate(r->count, &last_rmc));
}

static void _section_sensor_board(void) {
  const sensor_board_data_t *d = sensor_board_get_data();
  const uint32_t errors =
      d->err_unknown_id + d->err_short_frame + d->err_id_mismatch;

  P(LINE_THIN);
  P(" " C_TITLE "PLACA DE SENSORES (CAN2)" C_RESET "  resets %lu  errores %s%lu" C_RESET
    "\r\n",
    (unsigned long)d->board_resets, (errors > 0U) ? C_BAD : "",
    (unsigned long)errors);

  for (int i = 0; i < 2; i++) {
    P("   Baro%d  %9.3f kPa  %6.2f C          (%lu tramas)\r\n", i + 1,
      (double)d->baro[i].pressure_kpa, (double)d->baro[i].temperature_c,
      (unsigned long)d->baro[i].count);
  }
  for (int i = 0; i < 2; i++) {
    P("   IMU%d acc [m/s2] ", i + 1);
    _xyz(d->imu[i].accel_mps2, 3);
    P("  (%lu)\r\n   IMU%d gyr [rad/s]", (unsigned long)d->imu[i].count, i + 1);
    _xyz(d->imu[i].gyro_rps, 4);
    P("\r\n");
  }
  P("   Mag  [uT]       ");
  const float mag_ut[3] = {d->mag.field_t[0] * 1e6f, d->mag.field_t[1] * 1e6f,
                           d->mag.field_t[2] * 1e6f};
  _xyz(mag_ut, 2);
  P("  (%lu)\r\n", (unsigned long)d->mag.count);
  _section_gps(d);
}

/* Errores de FileX más habituales */
static const char *_fx_error_name(uint32_t e) {
  switch (e) {
  case FX_BOOT_ERROR:      return "boot record no valido";
  case FX_MEDIA_INVALID:   return "formato no soportado (exFAT? usar FAT32)";
  case FX_FAT_READ_ERROR:  return "error leyendo la FAT";
  case FX_NOT_FOUND:       return "archivo no encontrado";
  case FX_NO_MORE_SPACE:   return "tarjeta llena";
  case FX_INVALID_NAME:    return "nombre no valido";
  case FX_IO_ERROR:        return "error de E/S (SDMMC)";
  default:                 return "";
  }
}

static void _section_storage(void) {
  P(LINE_THIN);
  P(" " C_TITLE "TARJETAS SD" C_RESET "\r\n");

  for (int i = 0; i < (int)SD_CARD_COUNT; i++) {
    const storage_sd_info_t *sd = storage_get_info((sd_card_id_t)i);
    P("   SD%d  CD=%d  ", i + 1, sd->cd_pin_high ? 1 : 0);

    switch (sd->state) {
    case STORAGE_SD_MOUNTED:
      P(C_OK "montada" C_RESET "  %lu MB, libres %lu MB   arranques %lu   test %s"
        C_RESET " (%lu ms)\r\n",
        (unsigned long)sd->capacity_mb, (unsigned long)sd->free_mb,
        (unsigned long)sd->boot_count,
        (sd->self_test == STORAGE_TEST_OK) ? C_OK "OK" : C_BAD "FALLO",
        (unsigned long)sd->self_test_ms);
      break;
    case STORAGE_SD_INIT_ERROR:
      P(C_BAD "no responde" C_RESET " (sin tarjeta?)\r\n");
      break;
    case STORAGE_SD_MOUNT_ERROR:
      P(C_BAD "no se puede montar" C_RESET "\r\n");
      break;
    default:
      P(C_DIM "sin iniciar" C_RESET "\r\n");
      break;
    }

    if ((sd->self_test == STORAGE_TEST_FAILED) && (sd->fx_error == 0U)) {
      P("        " C_WARN "lo releido no coincide con lo escrito: la tarjeta acepta"
        " escrituras pero no las guarda (gastada / solo lectura?)" C_RESET "\r\n");
    }
    if ((sd->fx_error != 0U) || (sd->hal_error != 0U)) {
      P("        " C_WARN "ultimo error: FileX 0x%02lX %s  HAL SD 0x%08lX" C_RESET "\r\n",
        (unsigned long)sd->fx_error, _fx_error_name(sd->fx_error),
        (unsigned long)sd->hal_error);
    }
  }
}

static void _section_can(void) {
  P(LINE_THIN);
  for (int port = 0; port < (int)CAN_PORT_COUNT; port++) {
    can_driver_stats_t st;
    can_driver_get_stats((can_port_t)port, &st);
    const bool bad = (st.rx_overflows + st.fifo_msg_lost + st.tx_errors) > 0U;
    P(" " C_TITLE "CAN%d" C_RESET "  rx %-9lu %sdesbordes %lu  perdidas %lu  errores tx %lu"
      C_RESET "\r\n",
      port + 1, (unsigned long)st.rx_frames, bad ? C_WARN : "",
      (unsigned long)st.rx_overflows, (unsigned long)st.fifo_msg_lost,
      (unsigned long)st.tx_errors);
  }
  const uint32_t dropped = DebugOut_DroppedBytes();
  if (dropped > 0U) {
    P(" " C_WARN "Debug USB: %lu bytes descartados (sin PC o buffer lleno)" C_RESET "\r\n",
      (unsigned long)dropped);
  }
}

/* ===========================================================================
 * API
 * ======================================================================== */

void DebugConsole_Banner(void) {
  P("\r\n\r\n" C_TITLE LINE_THICK);
  P("   LEEM  -  CORE H7  (ordenador de vuelo)\r\n");
  P(LINE_THICK C_RESET);
  P("   Compilado      " __DATE__ "  " __TIME__ "\r\n");
  P("   Protocolo CAN1 v%u.%u   (1 Mbit/s, CAN FD sin BRS)\r\n",
    (unsigned)CAN_PROTOCOL_VERSION_MAJOR, (unsigned)CAN_PROTOCOL_VERSION_MINOR);
  P("   Consola        USB (puerto COM virtual), informe cada %lu ms\r\n",
    (unsigned long)DEBUG_CONSOLE_PERIOD_MS);
  P(LINE_THIN);
#if SOUNDS_STARTUP_ENABLE
  P("   Melodia de arranque en el zumbador (PE4, TIM15)...\r\n");
#endif
}

void DebugConsole_WakeUpStart(void) {
  P("   Enviando wake up al resto de placas...\r\n");
}

void DebugConsole_Startup(const sys_state *state) {
  P("   Wake up terminado en %lu ms:  power %s   avionica %s   rf %s\r\n",
    (unsigned long)HAL_GetTick(), _flag(state->power), _flag(state->avionics),
    _flag(state->rf));
  _section_components(state);
  P(C_TITLE LINE_THICK C_RESET "\r\n");
  _last_report_ms = HAL_GetTick();
}

void DebugConsole_Update(const sys_state *state) {
  if ((HAL_GetTick() - _last_report_ms) < DEBUG_CONSOLE_PERIOD_MS) {
    return;
  }
  _last_report_ms += DEBUG_CONSOLE_PERIOD_MS;
  _report_count++;

  const uint32_t t_s = HAL_GetTick() / 1000U;
  P(C_TITLE LINE_THICK C_RESET);
  P(" " C_TITLE "CORE H7" C_RESET "   t = %02lu:%02lu:%02lu   informe #%lu\r\n",
    (unsigned long)(t_s / 3600U), (unsigned long)((t_s / 60U) % 60U),
    (unsigned long)(t_s % 60U), (unsigned long)_report_count);
  P(C_TITLE LINE_THICK C_RESET);

  _section_boards(state);
  _section_components(state);
  _section_power(state);
  _section_calibration(state);
  _section_sensor_board();
  _section_storage();
  _section_can();

  P(C_TITLE LINE_THICK C_RESET "\r\n");
}
