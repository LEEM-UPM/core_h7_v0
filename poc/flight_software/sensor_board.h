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

#define SENSOR_BOARD_CAN_ID_FIRST SENSOR_BOARD_CAN_ID_BARO1
#define SENSOR_BOARD_CAN_ID_LAST SENSOR_BOARD_CAN_ID_MAG

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
  baro_data_t baro[2]; /* [0] = 0x101, [1] = 0x102 */
  imu_data_t imu[2];   /* [0] = 0x103, [1] = 0x104 */
  mag_data_t mag;      /* 0x105 */

  uint32_t board_resets; /* resets de la placa detectados por el timestamp */

  uint32_t err_unknown_id;  /* ID fuera de la tabla */
  uint32_t err_short_frame; /* longitud < 5 + 4 * N */
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
