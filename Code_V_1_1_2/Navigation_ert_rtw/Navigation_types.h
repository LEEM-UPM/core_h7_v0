/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: Navigation_types.h
 *
 * Code generated for Simulink model 'Navigation'.
 *
 * Model version                  : 1.1
 * Simulink Coder version         : 25.2 (R2025b) 28-Jul-2025
 * C/C++ source code generated on : Fri Aug 21 00:02:46 2026
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: ARM Compatible->ARM Cortex-M
 * Code generation objectives: Unspecified
 * Validation result: Not run
 */

#ifndef Navigation_types_h_
#define Navigation_types_h_
#include "rtwtypes.h"
#ifndef DEFINED_TYPEDEF_FOR_Bus_IMU_LowG_
#define DEFINED_TYPEDEF_FOR_Bus_IMU_LowG_

typedef struct {
  real32_T accel_mps2[3];
  real32_T gyro_rps[3];
  real32_T temp_C;
  real_T timestamp_s;
  uint8_T data_valid;
} Bus_IMU_LowG;

#endif

#ifndef DEFINED_TYPEDEF_FOR_Bus_IMU_HighG_
#define DEFINED_TYPEDEF_FOR_Bus_IMU_HighG_

typedef struct {
  real32_T accel_mps2[3];
  real_T timestamp_s;
  uint8_T data_valid;
} Bus_IMU_HighG;

#endif

#ifndef DEFINED_TYPEDEF_FOR_Bus_GNSS_
#define DEFINED_TYPEDEF_FOR_Bus_GNSS_

typedef struct {
  real_T pos_lla[3];
  real32_T vel_ned[3];
  uint8_T fix_type;
  uint8_T num_sats;
  uint8_T data_valid;
} Bus_GNSS;

#endif

#ifndef DEFINED_TYPEDEF_FOR_nav_bus_in_
#define DEFINED_TYPEDEF_FOR_nav_bus_in_

typedef struct {
  Bus_IMU_LowG imu_lowg;
  Bus_IMU_HighG imu_highg;
  Bus_GNSS gnss;
} nav_bus_in;

#endif

#ifndef DEFINED_TYPEDEF_FOR_Bus_NavState_
#define DEFINED_TYPEDEF_FOR_Bus_NavState_

typedef struct {
  real32_T quat_nb[4];
  real32_T pos_ned[3];
  real32_T vel_ned[3];
  real32_T accel_ned[3];
  real32_T accel_body[3];
  real32_T gyro_body[3];
  real32_T accel_bias[3];
  real32_T gyro_bias[3];
  real32_T P[225];
  real32_T imu_blend_w;
  real_T origin_lla[3];
  uint8_T origin_set;
  uint16_T align_cnt;
  real32_T acc_sum[3];
  real32_T gyr_sum[3];
  real32_T gnss_age_s;
  uint16_T reject_cnt;
  uint8_T gnss_only;
  uint16_T descent_cnt;
  real_T timestamp_s;
  uint8_T init_done;
} Bus_NavState;

#endif

#ifndef DEFINED_TYPEDEF_FOR_nav_bus_out_
#define DEFINED_TYPEDEF_FOR_nav_bus_out_

typedef struct {
  real32_T pos_ned[3];
  real32_T vel_ned[3];
  real32_T quat_nb[4];
  real32_T euler_rpy[3];
  real32_T accel_ned[3];
  real32_T accel_body[3];
  real32_T gyro_body[3];
  real32_T accel_bias[3];
  real32_T gyro_bias[3];
  real32_T imu_blend_w;
  real32_T gnss_age_s;
  real_T timestamp_s;
  uint8_T nav_valid;
} nav_bus_out;

#endif

#ifndef DEFINED_TYPEDEF_FOR_Bus_NavTunables_
#define DEFINED_TYPEDEF_FOR_Bus_NavTunables_

typedef struct {
  real32_T init_euler_rad[3];
} Bus_NavTunables;

#endif

#ifndef DEFINED_TYPEDEF_FOR_struct_51YCxKSCmnqZ8M6ffYluUB_
#define DEFINED_TYPEDEF_FOR_struct_51YCxKSCmnqZ8M6ffYluUB_

typedef struct {
  real_T dt;
  real_T gnss_dt;
  real_T g0;
  real_T gravity_mps2;
  real_T lowg_range_g;
  real_T highg_range_g;
  real_T blend_lo_g;
  real_T blend_hi_g;
  real_T lowg_sat_frac;
  real_T sigma_gyro_arw;
  real_T sigma_accel_vrw_lg;
  real_T sigma_accel_vrw_hg;
  real_T sigma_gyro_bias_rw;
  real_T sigma_accel_bias_rw;
  real_T sigma_accel_sf;
  real_T sigma_gyro_sf;
  real32_T R_gnss_pos[9];
  real32_T R_gnss_vel[9];
  real_T gate_chi2_pos;
  real_T gate_chi2_vel;
  uint16_T gate_reject_max;
  real_T gnss_reset_age_s;
  uint8_T gnss_only_after_apogee;
  uint16_T descent_debounce_n;
  real_T accel_bias_max;
  real_T gyro_bias_max;
  real_T bias_freeze_accel_g;
  real32_T gate_reset_pos_var;
  real32_T gate_reset_vel_var;
  real32_T gate_reset_bias_var;
  uint8_T gnss_min_sats;
  uint8_T gnss_min_fix;
  real_T gnss_max_age_s;
  real_T align_samples;
  real_T align_accel_tol;
  real_T align_gyro_tol;
  real_T init_euler_rad[3];
  real_T init_att_sigma_rad[3];
  real_T rail_survey_err_deg;
  real_T align_att_check_rad;
  real32_T P0[225];
  real32_T lever_arm_gnss[3];
  uint8_T use_gnss_origin;
  real_T launch_lla[3];
  real32_T init_quat[4];
} struct_51YCxKSCmnqZ8M6ffYluUB;

#endif

/* Forward declaration for rtModel */
typedef struct tag_RTM_Navigation_T RT_MODEL_Navigation_T;

#endif                                 /* Navigation_types_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
