#ifndef CAN_TEST_TYPES_H
#define CAN_TEST_TYPES_H

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

typedef struct {
    float   accel_mps2[3];
    float   gyro_rps[3];
    float   timestamp_s;
    uint8_t data_valid;
} imu_data_t;

typedef struct {
    float   lat_deg;
    float   lon_deg;
    float   alt_m;
    float   vel_ned[3];
    uint8_t fix;
    uint8_t data_valid;
} gps_data_t;

typedef struct {
    float   pressure_pa;
    uint8_t data_valid;
} baro_data_t;

/*
 * Contenedor global de todos los datos recibidos por CAN.
 * Protegido por el mutex de can_shared_data.
 */
typedef struct {
    imu_data_t  imu_hg;
    imu_data_t  imu_lg;
    gps_data_t  gps;
    baro_data_t baro;
    uint32_t    new_data_flags;
} can_rx_data_t;

/* Flags de dato nuevo */
#define CAN_NEW_DATA_IMU_HG   (1U << 0)
#define CAN_NEW_DATA_IMU_LG   (1U << 1)
#define CAN_NEW_DATA_BARO     (1U << 2)
#define CAN_NEW_DATA_GPS      (1U << 3)

#endif /* CAN_TEST_TYPES_H */