#include "sensor_board.h"

#include <string.h>

#include "can_protocol/can_pack.h"

#define SENSOR_BOARD_BARO_NFLOATS 2U
#define SENSOR_BOARD_IMU_NFLOATS 6U
#define SENSOR_BOARD_MAG_NFLOATS 3U

#define SENSOR_BOARD_HEADER_BYTES 5U /* id_sensor + timestamp */

/* Bytes de enteros tras la cabecera en las tramas del GPS */
#define SENSOR_BOARD_GPS_GGA_BYTES 20U
#define SENSOR_BOARD_GPS_RMC_BYTES 23U

/*
 * Margen para distinguir "muestra algo desordenada / latencia" de "reset de
 * la placa". Los desórdenes entre sensores y la latencia de la cola son de
 * milisegundos; la deriva entre relojes tras minutos sin tramas, decenas de ms.
 */
#define SENSOR_BOARD_TS_TOLERANCE_US 500000U

typedef struct {
  uint8_t valid;
  uint32_t last_raw;   /* último timestamp crudo que hizo avanzar el reloj */
  uint64_t last_us;    /* el mismo, desenrollado */
  uint32_t last_rx_ms; /* HAL_GetTick() cuando llegó */
} ts_unwrap_t;

static sensor_board_data_t _data;
static ts_unwrap_t _ts;

static uint32_t _read_u32_le(const uint8_t *p) {
  return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) |
         ((uint32_t)p[3] << 24);
}

/* float32 IEEE-754 little-endian, independiente del endianness del receptor */
static float _read_f32_le(const uint8_t *p) {
  uint32_t u = _read_u32_le(p);
  float f;
  memcpy(&f, &u, sizeof(f));
  return f;
}

/* Lee n floats a partir del byte 5. El relleno final se ignora. */
static void _read_floats(const uint8_t *data, float *out, uint32_t n) {
  for (uint32_t i = 0U; i < n; i++) {
    out[i] = _read_f32_le(&data[SENSOR_BOARD_HEADER_BYTES + 4U * i]);
  }
}

static uint64_t _ts_restart(uint32_t raw, uint32_t rx_tick_ms) {
  _ts.valid = 1U;
  _ts.last_raw = raw;
  _ts.last_us = raw;
  _ts.last_rx_ms = rx_tick_ms;
  return _ts.last_us;
}

/*
 * Desenrollado del timestamp de 32 bits, con un único estado para toda la
 * placa (todos los sensores comparten reloj).
 *
 * Las tramas de distintos sensores no llegan ordenadas, así que no basta con
 * "nuevo < anterior -> vuelta": una muestra unos µs más antigua que la última
 * recibida parecería una vuelta. Se usa la diferencia módulo 2^32:
 *   - hacia delante (< 2^31): avanza el reloj; cruza la vuelta sola.
 *   - hacia atrás: muestra más antigua que la última; no mueve el reloj.
 *
 * Reset de la placa (el contador vuelve a 0), detectado por:
 *   - salto hacia atrás mayor que la tolerancia, o
 *   - salto hacia delante mayor que el tiempo real transcurrido en el
 *     receptor (pasa si la placa llevaba > 35,8 min encendida al resetear).
 */
static uint64_t _ts_unwrap(uint32_t raw, uint32_t rx_tick_ms) {
  if (!_ts.valid) {
    return _ts_restart(raw, rx_tick_ms);
  }

  uint32_t fwd = raw - _ts.last_raw;

  if (fwd < 0x80000000U) {
    uint64_t rx_elapsed_us =
        (uint64_t)(uint32_t)(rx_tick_ms - _ts.last_rx_ms) * 1000U;

    if ((uint64_t)fwd > rx_elapsed_us + SENSOR_BOARD_TS_TOLERANCE_US) {
      _data.board_resets++;
      return _ts_restart(raw, rx_tick_ms);
    }

    _ts.last_raw = raw;
    _ts.last_us += fwd;
    _ts.last_rx_ms = rx_tick_ms;
    return _ts.last_us;
  }

  uint32_t back = _ts.last_raw - raw;

  if (back > SENSOR_BOARD_TS_TOLERANCE_US) {
    _data.board_resets++;
    return _ts_restart(raw, rx_tick_ms);
  }

  return _ts.last_us - back;
}

static void _decode_gps_gga(const uint8_t *p, uint64_t t_us) {
  gps_gga_data_t *d = &_data.gps_gga;
  d->time_ms = can_get_u32_le(&p[0]);
  d->latitude_e7 = can_get_i32_le(&p[4]);
  d->longitude_e7 = can_get_i32_le(&p[8]);
  d->altitude_mm = can_get_i32_le(&p[12]);
  d->fix_quality = can_get_u8(&p[16]);
  d->satellites = can_get_u8(&p[17]);
  d->hdop_x100 = can_get_u16_le(&p[18]);
  d->timestamp_us = t_us;
  d->count++;
}

static void _decode_gps_rmc(const uint8_t *p, uint64_t t_us) {
  gps_rmc_data_t *d = &_data.gps_rmc;
  d->time_ms = can_get_u32_le(&p[0]);
  d->latitude_e7 = can_get_i32_le(&p[4]);
  d->longitude_e7 = can_get_i32_le(&p[8]);
  d->speed_mmps = can_get_i32_le(&p[12]);
  d->course_cdeg = can_get_u16_le(&p[16]);
  d->year = can_get_u16_le(&p[18]);
  d->month = can_get_u8(&p[20]);
  d->day = can_get_u8(&p[21]);
  d->valid = can_get_u8(&p[22]);
  d->timestamp_us = t_us;
  d->count++;
}

uint8_t sensor_board_process_frame(uint32_t can_id, const uint8_t *data,
                                   uint32_t len, uint32_t rx_tick_ms) {
  /* Longitud mínima según el ID, nunca según la longitud de la trama */
  uint32_t nfloats = 0U;
  uint32_t min_len;

  switch (can_id) {
  case SENSOR_BOARD_CAN_ID_GPS_GGA:
    min_len = SENSOR_BOARD_HEADER_BYTES + SENSOR_BOARD_GPS_GGA_BYTES;
    break;
  case SENSOR_BOARD_CAN_ID_GPS_RMC:
    min_len = SENSOR_BOARD_HEADER_BYTES + SENSOR_BOARD_GPS_RMC_BYTES;
    break;
  case SENSOR_BOARD_CAN_ID_BARO1:
  case SENSOR_BOARD_CAN_ID_BARO2:
    nfloats = SENSOR_BOARD_BARO_NFLOATS;
    break;
  case SENSOR_BOARD_CAN_ID_IMU1:
  case SENSOR_BOARD_CAN_ID_IMU2:
    nfloats = SENSOR_BOARD_IMU_NFLOATS;
    break;
  case SENSOR_BOARD_CAN_ID_MAG:
    nfloats = SENSOR_BOARD_MAG_NFLOATS;
    break;
  default:
    _data.err_unknown_id++;
    return 0U;
  }
  if (nfloats > 0U) {
    min_len = SENSOR_BOARD_HEADER_BYTES + 4U * nfloats;
  }

  if (len < min_len) {
    _data.err_short_frame++;
    return 0U;
  }

  if (data[0] != (uint8_t)(can_id - SENSOR_BOARD_CAN_ID_BASE)) {
    _data.err_id_mismatch++;
    return 0U;
  }

  uint64_t t_us = _ts_unwrap(_read_u32_le(&data[1]), rx_tick_ms);

  float v[SENSOR_BOARD_IMU_NFLOATS];
  _read_floats(data, v, nfloats);

  switch (can_id) {
  case SENSOR_BOARD_CAN_ID_GPS_GGA:
    _decode_gps_gga(&data[SENSOR_BOARD_HEADER_BYTES], t_us);
    break;

  case SENSOR_BOARD_CAN_ID_GPS_RMC:
    _decode_gps_rmc(&data[SENSOR_BOARD_HEADER_BYTES], t_us);
    break;

  case SENSOR_BOARD_CAN_ID_BARO1:
  case SENSOR_BOARD_CAN_ID_BARO2: {
    baro_data_t *d = &_data.baro[can_id - SENSOR_BOARD_CAN_ID_BARO1];
    d->pressure_kpa = v[0];
    d->temperature_c = v[1];
    d->timestamp_us = t_us;
    d->count++;
    break;
  }

  case SENSOR_BOARD_CAN_ID_IMU1:
  case SENSOR_BOARD_CAN_ID_IMU2: {
    imu_data_t *d = &_data.imu[can_id - SENSOR_BOARD_CAN_ID_IMU1];
    d->accel_mps2[0] = v[0];
    d->accel_mps2[1] = v[1];
    d->accel_mps2[2] = v[2];
    d->gyro_rps[0] = v[3];
    d->gyro_rps[1] = v[4];
    d->gyro_rps[2] = v[5];
    d->timestamp_us = t_us;
    d->count++;
    break;
  }

  default: { /* SENSOR_BOARD_CAN_ID_MAG */
    mag_data_t *d = &_data.mag;
    d->field_t[0] = v[0];
    d->field_t[1] = v[1];
    d->field_t[2] = v[2];
    d->timestamp_us = t_us;
    d->count++;
    break;
  }
  }

  return 1U;
}

const sensor_board_data_t *sensor_board_get_data(void) { return &_data; }
