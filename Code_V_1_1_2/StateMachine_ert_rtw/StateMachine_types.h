/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: StateMachine_types.h
 *
 * Code generated for Simulink model 'StateMachine'.
 *
 * Model version                  : 1.1
 * Simulink Coder version         : 25.2 (R2025b) 28-Jul-2025
 * C/C++ source code generated on : Fri Aug 21 00:03:37 2026
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: ARM Compatible->ARM Cortex-M
 * Code generation objectives: Unspecified
 * Validation result: Not run
 */

#ifndef StateMachine_types_h_
#define StateMachine_types_h_
#include "rtwtypes.h"
#ifndef DEFINED_TYPEDEF_FOR_Bus_TelemetryCmd_
#define DEFINED_TYPEDEF_FOR_Bus_TelemetryCmd_

typedef struct {
  uint8_T cmd_id;
  real32_T cmd_param;
  uint8_T cmd_valid;
} Bus_TelemetryCmd;

#endif

#ifndef DEFINED_TYPEDEF_FOR_sm_bus_in_
#define DEFINED_TYPEDEF_FOR_sm_bus_in_

typedef struct {
  real32_T pos_ned[3];
  real32_T vel_ned[3];
  real32_T accel_ned[3];
  real32_T accel_body[3];
  real32_T gyro_body[3];
  real32_T gnss_age_s;
  uint8_T nav_valid;
  real_T timestamp_s;
  Bus_TelemetryCmd telemetry_cmd;
} sm_bus_in;

#endif

#ifndef DEFINED_TYPEDEF_FOR_Bus_SMTunables_
#define DEFINED_TYPEDEF_FOR_Bus_SMTunables_

typedef struct {
  real32_T launch_accel_g_thr;
} Bus_SMTunables;

#endif

#ifndef DEFINED_TYPEDEF_FOR_Bus_SMState_
#define DEFINED_TYPEDEF_FOR_Bus_SMState_

typedef struct {
  int32_T state;
  int32_T prev_state;
  real_T t_entry_s;
  uint16_T launch_cnt;
  uint16_T err_cnt;
  uint16_T stall_cnt;
  uint8_T nav_ever_ok;
  uint8_T error_code;
  real_T t_prev_s;
  uint8_T init_done;
} Bus_SMState;

#endif

#ifndef DEFINED_TYPEDEF_FOR_sm_bus_out_
#define DEFINED_TYPEDEF_FOR_sm_bus_out_

typedef struct {
  int32_T rocket_state;
  int32_T prev_state;
  real32_T time_in_state_s;
  uint8_T error_code;
  real_T timestamp_s;
} sm_bus_out;

#endif

#ifndef DEFINED_TYPEDEF_FOR_struct_aXVKLoRAWGaqZzyFBeDeCE_
#define DEFINED_TYPEDEF_FOR_struct_aXVKLoRAWGaqZzyFBeDeCE_

typedef struct {
  real_T dt;
  real_T poweron_min_s;
  real_T ready_accel_tol_mps2;
  real_T ready_gyro_tol_rps;
  real_T ready_speed_tol_mps;
  real_T ready_gnss_age_s;
  real_T g0;
  real_T launch_accel_g_thr;
  real_T launch_debounce_ms;
  uint16_T launch_debounce_n;
  real_T nav_timeout_s;
  uint16_T nav_timeout_n;
  real_T nav_boot_timeout_s;
  uint16_T nav_boot_timeout_n;
  real_T stall_timeout_s;
  uint16_T stall_timeout_n;
  uint8_T check_nav_in_maint;
} struct_aXVKLoRAWGaqZzyFBeDeCE;

#endif

/* Forward declaration for rtModel */
typedef struct tag_RTM_StateMachine_T RT_MODEL_StateMachine_T;

#endif                                 /* StateMachine_types_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
