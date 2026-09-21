#include "sys_avionics.h"

#include <math.h>

#include "can_protocol/can_ids.h"
#include "can_protocol/can_pack.h"
#include "main.h"

/* AVIONICS_CALIBRATION (ICD, 6.9) */
#define CALIBRATION_LEN        32U
#define CALIBRATION_INVALID    ((int16_t)0x7FFF)
#define ACCEL_UNIT_MPS2        0.001f
#define GYRO_UNIT_RADPS        0.0001f

static void _clear_calibration(sys_avionics_calibration *cal);
static void _get_axes(const uint8_t *buf, float unit, float out[3]);

void sys_avionics_init(sys_state *state) {
  state->avionics_calibration_status = SYS_CALIBRATION_IDLE;
  state->avionics_calibration_tx_ms = 0U;
  _clear_calibration(&state->avionics_calibration);
}

void sys_avionics_update(sys_state *state) {
  if ((state->avionics_calibration_status == SYS_CALIBRATION_WAITING) &&
      ((HAL_GetTick() - state->avionics_calibration_tx_ms) >=
       SYS_CALIBRATION_TIMEOUT_MS)) {
    state->avionics_calibration_status = SYS_CALIBRATION_TIMEOUT;
  }
}

bool sys_avionics_process_frame(sys_state *state, const can_rx_frame_t *frame) {
  if (frame->id != CAN_ID_AVIONICS_CALIBRATION) {
    return false;
  }

  /* Una trama más corta se descarta entera: los campos quedarían descolocados */
  if (frame->len < CALIBRATION_LEN) {
    return true;
  }

  sys_avionics_calibration *cal = &state->avionics_calibration;
  const uint8_t *d = frame->data;

  _get_axes(&d[0], ACCEL_UNIT_MPS2, cal->imu1_accel_mps2);
  _get_axes(&d[6], GYRO_UNIT_RADPS, cal->imu1_gyro_radps);
  _get_axes(&d[12], ACCEL_UNIT_MPS2, cal->imu2_accel_mps2);
  _get_axes(&d[18], GYRO_UNIT_RADPS, cal->imu2_gyro_radps);
  cal->baro1_kpa = can_get_f32_le(&d[24]); /* NaN = no calibrado */
  cal->baro2_kpa = can_get_f32_le(&d[28]);

  /* Una respuesta tardía (tras el timeout) también se acepta */
  state->avionics_calibration_status = SYS_CALIBRATION_DONE;
  return true;
}

bool sys_avionics_calibrate(sys_state *state, float tilt_deg) {
  if (!state->avionics ||
      (state->avionics_calibration_status == SYS_CALIBRATION_WAITING)) {
    return false;
  }

  uint8_t data[4];
  can_put_f32_le(&data[0], tilt_deg);
  if (!can_driver_send(CAN_PORT_1, CAN_ID_CMD_AVIONICS_CALIBRATE, data,
                       sizeof(data))) {
    return false;
  }

  _clear_calibration(&state->avionics_calibration);
  state->avionics_calibration_tx_ms = HAL_GetTick();
  state->avionics_calibration_status = SYS_CALIBRATION_WAITING;
  return true;
}

static void _clear_calibration(sys_avionics_calibration *cal) {
  for (uint8_t axis = 0U; axis < 3U; axis++) {
    cal->imu1_accel_mps2[axis] = NAN;
    cal->imu1_gyro_radps[axis] = NAN;
    cal->imu2_accel_mps2[axis] = NAN;
    cal->imu2_gyro_radps[axis] = NAN;
  }
  cal->baro1_kpa = NAN;
  cal->baro2_kpa = NAN;
}

/* 3 x int16 en pasos de `unit` -> float; 0x7FFF -> NaN */
static void _get_axes(const uint8_t *buf, float unit, float out[3]) {
  for (uint8_t axis = 0U; axis < 3U; axis++) {
    int16_t raw = can_get_i16_le(&buf[2U * axis]);
    out[axis] = (raw == CALIBRATION_INVALID) ? NAN : ((float)raw * unit);
  }
}
