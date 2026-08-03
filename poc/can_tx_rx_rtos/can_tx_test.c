#include "can_tx_test.h"
#include "can_protocol.h"
#include "can_shared_data.h"
#include "stm32h7xx_hal_def.h"

#include <math.h>
#include <string.h>

static const can_payload_imu_t _ref_imu_hg = {.acc = {1.0f, 2.0f, 9.81f},
                                              .omega = {0.0f, 0.0f, 0.0f}};

static const can_payload_imu_t _ref_imu_lg = {.acc = {0.1f, -0.2f, 9.80f},
                                              .omega = {0.01f, 0.02f, 0.03f}};

static const can_payload_baro_t _ref_baro = {.pressure_pa = 95500.0f};

static const can_payload_gps_t _ref_gps = {.lat_deg = 40.4168f,
                                           .lon_deg = -3.7038f,
                                           .alt_m = 650.0f,
                                           .vel_ned = {10.0f, 2.0f, -0.5f},
                                           .fix = 1U,
                                           ._pad = {0}};

#define FLOAT_EPSILON 0.01f

static TX_THREAD _tx_thread;
static FDCAN_HandleTypeDef *_hfdcan_tx = NULL;
static volatile uint32_t _test_errors = 0U;
static volatile bool _test_done = false;

static bool _float_ok(float a, float b) { return fabsf(a - b) < FLOAT_EPSILON; }

static bool _send_frame(FDCAN_HandleTypeDef *hfdcan, uint32_t id,
                        uint32_t dlc_hal, const uint8_t *data) {
  FDCAN_TxHeaderTypeDef hdr = {.Identifier = id,
                               .IdType = FDCAN_STANDARD_ID,
                               .TxFrameType = FDCAN_DATA_FRAME,
                               .DataLength = dlc_hal,
                               .ErrorStateIndicator = FDCAN_ESI_ACTIVE,
                               .BitRateSwitch = FDCAN_BRS_OFF,
                               .FDFormat = FDCAN_FD_CAN,
                               .TxEventFifoControl = FDCAN_NO_TX_EVENTS,
                               .MessageMarker = 0U};

  uint32_t t0 = HAL_GetTick();
  while (HAL_FDCAN_GetTxFifoFreeLevel(hfdcan) == 0U) {
    if ((HAL_GetTick() - t0) > 10U)
      return false;
  }

  HAL_StatusTypeDef status =
      HAL_FDCAN_AddMessageToTxFifoQ(hfdcan, &hdr, (uint8_t *)data);

  return status == HAL_OK;
}

static uint32_t _verify_imu_hg(void) {
  uint32_t errs = 0U;
  can_shared_data_lock();
  imu_data_t d = g_can_shared.imu_hg;
  can_shared_data_unlock();

  if (!d.data_valid)
    errs++;
  if (!_float_ok(d.accel_mps2[0], _ref_imu_hg.acc[0]))
    errs++;
  if (!_float_ok(d.accel_mps2[1], _ref_imu_hg.acc[1]))
    errs++;
  if (!_float_ok(d.accel_mps2[2], _ref_imu_hg.acc[2]))
    errs++;
  if (!_float_ok(d.gyro_rps[0], _ref_imu_hg.omega[0]))
    errs++;
  if (!_float_ok(d.gyro_rps[1], _ref_imu_hg.omega[1]))
    errs++;
  if (!_float_ok(d.gyro_rps[2], _ref_imu_hg.omega[2]))
    errs++;
  return errs;
}

static uint32_t _verify_imu_lg(void) {
  uint32_t errs = 0U;
  can_shared_data_lock();
  imu_data_t d = g_can_shared.imu_lg;
  can_shared_data_unlock();

  if (!d.data_valid)
    errs++;
  if (!_float_ok(d.accel_mps2[0], _ref_imu_lg.acc[0]))
    errs++;
  if (!_float_ok(d.accel_mps2[1], _ref_imu_lg.acc[1]))
    errs++;
  if (!_float_ok(d.accel_mps2[2], _ref_imu_lg.acc[2]))
    errs++;
  if (!_float_ok(d.gyro_rps[0], _ref_imu_lg.omega[0]))
    errs++;
  if (!_float_ok(d.gyro_rps[1], _ref_imu_lg.omega[1]))
    errs++;
  if (!_float_ok(d.gyro_rps[2], _ref_imu_lg.omega[2]))
    errs++;
  return errs;
}

static uint32_t _verify_baro(void) {
  uint32_t errs = 0U;
  can_shared_data_lock();
  baro_data_t d = g_can_shared.baro;
  can_shared_data_unlock();

  if (!d.data_valid)
    errs++;
  if (!_float_ok(d.pressure_pa, _ref_baro.pressure_pa))
    errs++;
  return errs;
}

static uint32_t _verify_gps(void) {
  uint32_t errs = 0U;
  can_shared_data_lock();
  gps_data_t d = g_can_shared.gps;
  can_shared_data_unlock();

  if (!d.data_valid)
    errs++;
  if (!_float_ok(d.lat_deg, _ref_gps.lat_deg))
    errs++;
  if (!_float_ok(d.lon_deg, _ref_gps.lon_deg))
    errs++;
  if (!_float_ok(d.alt_m, _ref_gps.alt_m))
    errs++;
  if (!_float_ok(d.vel_ned[0], _ref_gps.vel_ned[0]))
    errs++;
  if (!_float_ok(d.vel_ned[1], _ref_gps.vel_ned[1]))
    errs++;
  if (!_float_ok(d.vel_ned[2], _ref_gps.vel_ned[2]))
    errs++;
  if (d.fix != _ref_gps.fix)
    errs++;
  return errs;
}

static void _can_tx_test_entry(ULONG arg) {
  (void)arg;
  uint8_t buf[64];
  uint32_t total_errors = 0U;

  for (uint32_t i = 0U; i < CAN_TX_TEST_ITERATIONS; i++) {

    can_serialize_imu(&_ref_imu_hg, buf);
    _send_frame(_hfdcan_tx, CAN_ID_IMU_HG, FDCAN_DLC_BYTES_24, buf);

    can_serialize_imu(&_ref_imu_lg, buf);
    _send_frame(_hfdcan_tx, CAN_ID_IMU_LG, FDCAN_DLC_BYTES_24, buf);

    can_serialize_baro(&_ref_baro, buf);
    _send_frame(_hfdcan_tx, CAN_ID_BARO, FDCAN_DLC_BYTES_4, buf);

    can_serialize_gps(&_ref_gps, buf);
    _send_frame(_hfdcan_tx, CAN_ID_GPS, FDCAN_DLC_BYTES_32, buf);

    tx_thread_sleep(CAN_TX_PERIOD_TICKS);

    total_errors += _verify_imu_hg();
    total_errors += _verify_imu_lg();
    total_errors += _verify_baro();
    total_errors += _verify_gps();
  }

  _test_errors = total_errors;
  _test_done = true;
  tx_thread_suspend(tx_thread_identify());
}

UINT can_tx_test_create(TX_BYTE_POOL *byte_pool,
                        FDCAN_HandleTypeDef *hfdcan_tx) {
  UINT status;
  CHAR *stack_ptr;

  _hfdcan_tx = hfdcan_tx;

  status = tx_byte_allocate(byte_pool, (VOID **)&stack_ptr,
                            CAN_TX_THREAD_STACK_SIZE, TX_NO_WAIT);
  if (status != TX_SUCCESS)
    return status;

  status = tx_thread_create(&_tx_thread, "CAN TX test", _can_tx_test_entry, 0U,
                            stack_ptr, CAN_TX_THREAD_STACK_SIZE,
                            CAN_TX_THREAD_PRIO, CAN_TX_THREAD_PREEMPT_THR,
                            TX_NO_TIME_SLICE, TX_AUTO_START);
  if (status != TX_SUCCESS)
    return status;

  if (HAL_FDCAN_Start(hfdcan_tx) != HAL_OK) {
    return TX_START_ERROR;
  }

  return TX_SUCCESS;
}

uint32_t can_tx_test_get_errors(void) { return _test_errors; }
bool can_tx_test_is_done(void) { return _test_done; }