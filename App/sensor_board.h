#ifndef SENSOR_BOARD_H
#define SENSOR_BOARD_H

#include <stdint.h>

/*
 * Telemetría de la placa de sensores (STM32G473) por CAN FD sin BRS.
 *
 * ID estándar = 0x100 + id_sensor.
 * Payload (little-endian):
 *   byte 0     id_sensor
 *   bytes 1..4 timestamp uint32 [µs] desde el arranque de la placa
 *   bytes 5..  N float32 IEEE-754
 *   resto      relleno con ceros hasta la longitud CAN FD válida
 */
#define SENSOR_BOARD_CAN_ID_BASE 0x100U

#define SENSOR_BOARD_CAN_ID_BARO1 0x101U /* WSEN-PADS:  2 floats, 16 bytes */
#define SENSOR_BOARD_CAN_ID_BARO2 0x102U /* WSEN-PADS:  2 floats, 16 bytes */
#define SENSOR_BOARD_CAN_ID_IMU1 0x103U  /* WSEN-ISDS:  6 floats, 32 bytes */
#define SENSOR_BOARD_CAN_ID_IMU2 0x104U  /* LSM6DSV32X: 6 floats, 32 bytes */
#define SENSOR_BOARD_CAN_ID_MAG 0x105U   /* MLX90393:   3 floats, 20 bytes */
#define SENSOR_BOARD_CAN_ID_GPS_GGA 0x106U /* MAX-M10S: enteros, 32 bytes */
#define SENSOR_BOARD_CAN_ID_GPS_RMC 0x107U /* MAX-M10S: enteros, 32 bytes */

#define SENSOR_BOARD_CAN_ID_FIRST SENSOR_BOARD_CAN_ID_BARO1
#define SENSOR_BOARD_CAN_ID_LAST SENSOR_BOARD_CAN_ID_GPS_RMC

/*
 * Tramas del GPS (10 Hz): tras la misma cabecera de 5 bytes llevan enteros
 * little-endian en vez de floats (sin pérdida de precisión en lat/lon).
 * Posición desde el byte 0 de la trama:
 *
 *  0x106 GGA                           0x107 RMC
 *   5  uint32 hora UTC [ms del día]      5  uint32 hora UTC [ms del día]
 *   9  int32  latitud  [1e-7 grados]     9  int32  latitud  [1e-7 grados]
 *  13  int32  longitud [1e-7 grados]    13  int32  longitud [1e-7 grados]
 *  17  int32  altitud MSL [mm]          17  int32  velocidad [mm/s]
 *  21  uint8  calidad del fix (0 = no)  21  uint16 rumbo [0.01 grados]
 *  22  uint8  satélites usados          23  uint16 año (0 = desconocido)
 *  23  uint16 HDOP x100                 25  uint8  mes
 *                                       26  uint8  día
 *                                       27  uint8  estado (1 = 'A' válido)
 *
 * No válidos: int32 0x80000000, uint16 0xFFFF, hora 0xFFFFFFFF.
 */
#define SENSOR_BOARD_GPS_INVALID_I32 INT32_MIN
#define SENSOR_BOARD_GPS_INVALID_U16 0xFFFFU
#define SENSOR_BOARD_GPS_INVALID_TIME 0xFFFFFFFFU

/*
 * timestamp_us: instante de la muestra en el reloj de la placa, ya
 * desenrollado a 64 bits. Es común a todos los sensores, así que se puede
 * comparar entre ellos. Tras un reset de la placa vuelve a empezar desde 0
 * (ver board_resets).
 */
typedef struct {
  float pressure_kpa;
  float temperature_c;
  uint64_t timestamp_us;
  uint32_t count; /* tramas válidas recibidas */
} baro_data_t;

typedef struct {
  float accel_mps2[3]; /* X, Y, Z */
  float gyro_rps[3];   /* X, Y, Z */
  uint64_t timestamp_us;
  uint32_t count;
} imu_data_t;

typedef struct {
  float field_t[3]; /* X, Y, Z [T] */
  uint64_t timestamp_us;
  uint32_t count;
} mag_data_t;

typedef struct {
  uint32_t time_ms;       /* hora UTC en ms desde las 00:00 */
  int32_t latitude_e7;    /* 1e-7 grados, +N */
  int32_t longitude_e7;   /* 1e-7 grados, +E */
  int32_t altitude_mm;    /* sobre el nivel del mar */
  uint8_t fix_quality;    /* 0 = sin fix */
  uint8_t satellites;
  uint16_t hdop_x100;
  uint64_t timestamp_us;
  uint32_t count;
} gps_gga_data_t;

typedef struct {
  uint32_t time_ms;
  int32_t latitude_e7;
  int32_t longitude_e7;
  int32_t speed_mmps;
  uint16_t course_cdeg;
  uint16_t year;
  uint8_t month;
  uint8_t day;
  uint8_t valid;
  uint64_t timestamp_us;
  uint32_t count;
} gps_rmc_data_t;

typedef struct {
  baro_data_t baro[2]; /* [0] = 0x101, [1] = 0x102 */
  imu_data_t imu[2];   /* [0] = 0x103, [1] = 0x104 */
  mag_data_t mag;      /* 0x105 */
  gps_gga_data_t gps_gga; /* 0x106 */
  gps_rmc_data_t gps_rmc; /* 0x107 */

  uint32_t board_resets; /* resets de la placa detectados por el timestamp */

  uint32_t err_unknown_id;  /* ID fuera de la tabla */
  uint32_t err_short_frame; /* longitud menor que la del sensor */
  uint32_t err_id_mismatch; /* byte 0 != ID - 0x100 */
} sensor_board_data_t;

/*
 * Decodifica una trama y actualiza el último valor del sensor.
 * rx_tick_ms: HAL_GetTick() al recibir la trama. Solo se usa para detectar
 * resets de la placa, nunca como marca de tiempo de la muestra.
 * Devuelve 1 si la trama era válida, 0 si se ha descartado.
 */
uint8_t sensor_board_process_frame(uint32_t can_id, const uint8_t *data,
                                   uint32_t len, uint32_t rx_tick_ms);

const sensor_board_data_t *sensor_board_get_data(void);

#endif /* SENSOR_BOARD_H */
