/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: GuidanceControl_types.h
 *
 * Code generated for Simulink model 'GuidanceControl'.
 *
 * Model version                  : 1.1
 * Simulink Coder version         : 25.2 (R2025b) 28-Jul-2025
 * C/C++ source code generated on : Fri Aug 21 00:04:24 2026
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: ARM Compatible->ARM Cortex-M
 * Code generation objectives: Unspecified
 * Validation result: Not run
 */

#ifndef GuidanceControl_types_h_
#define GuidanceControl_types_h_
#include "rtwtypes.h"
#ifndef DEFINED_TYPEDEF_FOR_Bus_TelemetryCmd_
#define DEFINED_TYPEDEF_FOR_Bus_TelemetryCmd_

typedef struct {
  uint8_T cmd_id;
  real32_T cmd_param;
  uint8_T cmd_valid;
} Bus_TelemetryCmd;

#endif

#ifndef DEFINED_TYPEDEF_FOR_gc_bus_in_
#define DEFINED_TYPEDEF_FOR_gc_bus_in_

typedef struct {
  real32_T pos_ned[3];
  real32_T vel_ned[3];
  real32_T accel_ned[3];
  real32_T accel_body[3];
  real32_T gnss_age_s;
  uint8_T nav_valid;
  real_T timestamp_s;
  int32_T rocket_state;
  Bus_TelemetryCmd telemetry_cmd;
} gc_bus_in;

#endif

#ifndef DEFINED_TYPEDEF_FOR_Bus_GCTunables_
#define DEFINED_TYPEDEF_FOR_Bus_GCTunables_

typedef struct {
  real32_T apogee_target_agl_m;
  real32_T min_ctrl_alt_agl_m;
  real32_T main_deploy_alt_agl_m;
  real32_T drogue_delay_s;
} Bus_GCTunables;

#endif

#ifndef DEFINED_TYPEDEF_FOR_Bus_GCState_
#define DEFINED_TYPEDEF_FOR_Bus_GCState_

typedef struct {
  int32_T phase;
  real32_T u_cmd;
  real32_T integ;
  real32_T apogee_pred_m;
  real32_T max_alt_m;
  real32_T ground_alt_m;
  uint8_T ground_set;
  real_T t_phase_s;
  real_T t_prev_s;
  real_T t_apogee_s;
  real_T t_drogue_s;
  real_T t_main_s;
  uint8_T apogee_detected;
  uint8_T drogue_armed;
  uint8_T main_armed;
  uint16_T burnout_cnt;
  uint16_T nav_ok_cnt;
  uint16_T apogee_cnt;
  uint16_T land_cnt;
  uint8_T init_done;
} Bus_GCState;

#endif

#ifndef DEFINED_TYPEDEF_FOR_gc_bus_out_
#define DEFINED_TYPEDEF_FOR_gc_bus_out_

typedef struct {
  real32_T actuator_cmd;
  real32_T airbrake_deg;
  real32_T apogee_pred_m;
  real32_T apogee_error_m;
  real32_T time_to_apogee_s;
  uint8_T drogue_deploy_flag;
  uint8_T main_deploy_flag;
  int32_T flight_phase;
  uint8_T pred_valid;
  real_T timestamp_s;
} gc_bus_out;

#endif

#ifndef DEFINED_TYPEDEF_FOR_struct_fab7xe5zbqKImI9YqNejpE_
#define DEFINED_TYPEDEF_FOR_struct_fab7xe5zbqKImI9YqNejpE_

typedef struct {
  real_T dt;
  real_T g0;
  real_T apogee_target_agl_m;
  real_T mass_dry_kg;
  real_T S_ref_m2;
  real_T alt_ref_msl_m;
  real_T mach_min;
  real_T mach_step;
  int32_T n_mach;
  int32_T n_deploy;
  real32_T cd_tbl[215];
  real32_T deploy_tbl[5];
  real_T prop_dt;
  int32_T prop_max_steps;
  real_T isa_T0;
  real_T isa_p0;
  real_T isa_L;
  real_T isa_R;
  real_T isa_gamma;
  real_T ki;
  real_T integ_max;
  real_T u_min;
  real_T u_max;
  real_T u_rate_max;
  real_T deadband_m;
  real_T airbrake_max_deg;
  real_T tgo_min_s;
  real_T q_min_ctrl_pa;
  real_T min_boost_s;
  real_T burnout_decel_thr;
  uint16_T burnout_debounce_n;
  real_T coast_lockout_s;
  real_T min_ctrl_alt_agl_m;
  real_T nav_ready_gnss_age_s;
  uint16_T nav_settle_n;
  real_T mach_ctrl_max;
  real_T v_ctrl_min_mps;
  uint16_T apogee_debounce_n;
  real_T apogee_alt_drop_m;
  real_T drogue_delay_s;
  real_T main_deploy_alt_agl_m;
  real_T pyro_pulse_s;
  real_T main_backup_delay_s;
  real_T land_speed_thr_mps;
  real_T land_alt_thr_m;
  uint16_T land_debounce_n;
} struct_fab7xe5zbqKImI9YqNejpE;

#endif

/* Forward declaration for rtModel */
typedef struct tag_RTM_GuidanceControl_T RT_MODEL_GuidanceControl_T;

#endif                                 /* GuidanceControl_types_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
