#ifndef CAN_PROTOCOL_H
#define CAN_PROTOCOL_H

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define CAN_ID_IMU_HG   0x080U
#define CAN_ID_IMU_LG   0x081U
#define CAN_ID_BARO     0x082U
#define CAN_ID_GPS      0x083U

/*
 * IMU High-G e IMU Low-G comparten el mismo formato de trama.
 * DLC: 24 bytes (FDCAN_DLC_BYTES_24).
 */
typedef struct __attribute__((packed)) {
    float acc[3];    /* aceleración X,Y,Z en m/s²  */
    float omega[3];  /* velocidad angular X,Y,Z en rad/s */
} can_payload_imu_t;

_Static_assert(sizeof(can_payload_imu_t) == 24,
    "can_payload_imu_t debe ser exactamente 24 bytes");

/*
 * Barómetro.
 * DLC: 4 bytes (FDCAN_DLC_BYTES_4).
 */
typedef struct __attribute__((packed)) {
    float pressure_pa;  /* presión absoluta en Pascales */
} can_payload_baro_t;

_Static_assert(sizeof(can_payload_baro_t) == 4,
    "can_payload_baro_t debe ser exactamente 4 bytes");

/*
 * GPS.
 * DLC: 32 bytes (FDCAN_DLC_BYTES_32).
 *
 * Se usan float en lugar de double para caber en 32 bytes.
 * Para la prueba de validación la precisión de float es suficiente.
 * En producción este punto deberá revisarse con el equipo de GNC.
 *
 *   lat_deg  4
 *   lon_deg  4
 *   alt_m    4
 *   vel_ned  12
 *   fix      1
 *   _pad     7  → total 32
 */
typedef struct __attribute__((packed)) {
    float   lat_deg;     /* latitud en grados decimales WGS84  */
    float   lon_deg;     /* longitud en grados decimales WGS84 */
    float   alt_m;       /* altitud sobre el elipsoide, metros */
    float   vel_ned[3];  /* velocidad NED en m/s               */
    uint8_t fix;         /* 1 = fix válido                     */
    uint8_t _pad[7];     /* relleno hasta 32 bytes             */
} can_payload_gps_t;

_Static_assert(sizeof(can_payload_gps_t) == 32,
    "can_payload_gps_t debe ser exactamente 32 bytes");

/* 
 * memcpy evita undefined behavior por strict aliasing.
 * Con optimización el compilador lo convierte en MOV/STR directos.
 */

static inline void can_serialize_imu(const can_payload_imu_t *in,
                                     uint8_t *raw)
{
    memcpy(raw, in, sizeof(can_payload_imu_t));
}

static inline void can_serialize_baro(const can_payload_baro_t *in,
                                      uint8_t *raw)
{
    memcpy(raw, in, sizeof(can_payload_baro_t));
}

static inline void can_serialize_gps(const can_payload_gps_t *in,
                                     uint8_t *raw)
{
    memcpy(raw, in, sizeof(can_payload_gps_t));
}

static inline void can_deserialize_imu(const uint8_t *raw,
                                       can_payload_imu_t *out)
{
    memcpy(out, raw, sizeof(can_payload_imu_t));
}

static inline void can_deserialize_baro(const uint8_t *raw,
                                        can_payload_baro_t *out)
{
    memcpy(out, raw, sizeof(can_payload_baro_t));
}

static inline void can_deserialize_gps(const uint8_t *raw,
                                       can_payload_gps_t *out)
{
    memcpy(out, raw, sizeof(can_payload_gps_t));
}

#endif /* CAN_PROTOCOL_H */