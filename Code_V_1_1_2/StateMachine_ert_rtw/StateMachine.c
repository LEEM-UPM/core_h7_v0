/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: StateMachine.c
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

#include "StateMachine.h"
#include "rtwtypes.h"
#include "rt_nonfinite.h"
#include <math.h>
#include "StateMachine_types.h"

/* Exported block parameters */
Bus_SMTunables sm_tun = {
  3.0F
} ;                                    /* Variable: sm_tun
                                        * Referenced by: '<Root>/StateMachine_Logic'
                                        */

/* Block states (default storage) */
DW_StateMachine_T StateMachine_DW;

/* External inputs (root inport signals with default storage) */
ExtU_StateMachine_T StateMachine_U;

/* External outputs (root outports fed by signals with default storage) */
ExtY_StateMachine_T StateMachine_Y;

/* Real-time model */
static RT_MODEL_StateMachine_T StateMachine_M_;
RT_MODEL_StateMachine_T *const StateMachine_M = &StateMachine_M_;

/* Forward declaration for local functions */
static real_T StateMachine_norm(const real_T x[3]);

/* Function for MATLAB Function: '<Root>/StateMachine_Logic' */
static real_T StateMachine_norm(const real_T x[3])
{
  real_T absxk;
  real_T scale;
  real_T t;
  real_T y;
  scale = 3.3121686421112381E-170;
  absxk = fabs(x[0]);
  if (absxk > 3.3121686421112381E-170) {
    y = 1.0;
    scale = absxk;
  } else {
    t = absxk / 3.3121686421112381E-170;
    y = t * t;
  }

  absxk = fabs(x[1]);
  if (absxk > scale) {
    t = scale / absxk;
    y = y * t * t + 1.0;
    scale = absxk;
  } else {
    t = absxk / scale;
    y += t * t;
  }

  absxk = fabs(x[2]);
  if (absxk > scale) {
    t = scale / absxk;
    y = y * t * t + 1.0;
    scale = absxk;
  } else {
    t = absxk / scale;
    y += t * t;
  }

  y = scale * sqrt(y);
  if (rtIsNaN(y)) {
    int32_T b_k;
    b_k = 0;
    int32_T exitg1;
    do {
      exitg1 = 0;
      if (b_k < 3) {
        if (rtIsNaN(x[b_k])) {
          exitg1 = 1;
        } else {
          b_k++;
        }
      } else {
        y = (rtInf);
        exitg1 = 1;
      }
    } while (exitg1 == 0);
  }

  return y;
}

/* Model step function */
void StateMachine_step(void)
{
  real_T tmp[3];
  real_T accel_mag;
  real_T rtb_sm_state_out_t_entry_s;
  real_T t_in_state;
  int32_T k;
  int32_T new_state;
  int32_T rtb_sm_state_out_prev_state;
  int32_T rtb_sm_state_out_state;
  uint16_T rtb_sm_state_out_err_cnt;
  uint16_T rtb_sm_state_out_launch_cnt;
  uint16_T rtb_sm_state_out_stall_cnt;
  uint16_T tmp_0;
  uint8_T cmd;
  uint8_T fault_code;
  uint8_T rtb_sm_state_out_error_code;
  uint8_T rtb_sm_state_out_init_done;
  uint8_T rtb_sm_state_out_nav_ever_ok;
  boolean_T b[3];
  boolean_T c[3];
  boolean_T exitg1;
  boolean_T y;

  /* MATLAB Function: '<Root>/StateMachine_Logic' incorporates:
   *  BusCreator generated from: '<Root>/StateMachine_Logic'
   *  Inport: '<Root>/SMIn'
   *  Memory: '<Root>/SMState_Memory'
   */
  rtb_sm_state_out_state = StateMachine_DW.SMState_Memory_PreviousInput.state;
  rtb_sm_state_out_prev_state =
    StateMachine_DW.SMState_Memory_PreviousInput.prev_state;
  rtb_sm_state_out_t_entry_s =
    StateMachine_DW.SMState_Memory_PreviousInput.t_entry_s;
  rtb_sm_state_out_launch_cnt =
    StateMachine_DW.SMState_Memory_PreviousInput.launch_cnt;
  rtb_sm_state_out_err_cnt =
    StateMachine_DW.SMState_Memory_PreviousInput.err_cnt;
  rtb_sm_state_out_stall_cnt =
    StateMachine_DW.SMState_Memory_PreviousInput.stall_cnt;
  rtb_sm_state_out_nav_ever_ok =
    StateMachine_DW.SMState_Memory_PreviousInput.nav_ever_ok;
  rtb_sm_state_out_error_code =
    StateMachine_DW.SMState_Memory_PreviousInput.error_code;
  t_in_state = StateMachine_DW.SMState_Memory_PreviousInput.t_prev_s;
  rtb_sm_state_out_init_done =
    StateMachine_DW.SMState_Memory_PreviousInput.init_done;
  if (StateMachine_DW.SMState_Memory_PreviousInput.init_done == 0) {
    rtb_sm_state_out_state = 0;
    rtb_sm_state_out_prev_state = 0;
    rtb_sm_state_out_t_entry_s = StateMachine_U.SMIn.timestamp_s;
    rtb_sm_state_out_launch_cnt = 0U;
    rtb_sm_state_out_err_cnt = 0U;
    rtb_sm_state_out_stall_cnt = 0U;
    rtb_sm_state_out_nav_ever_ok = 0U;
    rtb_sm_state_out_error_code = 0U;
    t_in_state = StateMachine_U.SMIn.timestamp_s;
    rtb_sm_state_out_init_done = 1U;
  }

  new_state = rtb_sm_state_out_state;
  tmp[0] = StateMachine_U.SMIn.accel_body[0];
  b[0] = !rtIsInf(StateMachine_U.SMIn.pos_ned[0]);
  c[0] = !rtIsNaN(StateMachine_U.SMIn.pos_ned[0]);
  tmp[1] = StateMachine_U.SMIn.accel_body[1];
  b[1] = !rtIsInf(StateMachine_U.SMIn.pos_ned[1]);
  c[1] = !rtIsNaN(StateMachine_U.SMIn.pos_ned[1]);
  tmp[2] = StateMachine_U.SMIn.accel_body[2];
  b[2] = !rtIsInf(StateMachine_U.SMIn.pos_ned[2]);
  c[2] = !rtIsNaN(StateMachine_U.SMIn.pos_ned[2]);
  accel_mag = StateMachine_norm(tmp);
  y = true;
  k = 0;
  exitg1 = false;
  while ((!exitg1) && (k < 3)) {
    if ((!b[k]) || (!c[k])) {
      y = false;
      exitg1 = true;
    } else {
      k++;
    }
  }

  if (y) {
    b[0] = !rtIsInf(StateMachine_U.SMIn.vel_ned[0]);
    c[0] = !rtIsNaN(StateMachine_U.SMIn.vel_ned[0]);
    b[1] = !rtIsInf(StateMachine_U.SMIn.vel_ned[1]);
    c[1] = !rtIsNaN(StateMachine_U.SMIn.vel_ned[1]);
    b[2] = !rtIsInf(StateMachine_U.SMIn.vel_ned[2]);
    c[2] = !rtIsNaN(StateMachine_U.SMIn.vel_ned[2]);
    y = true;
    k = 0;
    exitg1 = false;
    while ((!exitg1) && (k < 3)) {
      if ((!b[k]) || (!c[k])) {
        y = false;
        exitg1 = true;
      } else {
        k++;
      }
    }

    if (y) {
      b[0] = !rtIsInf(StateMachine_U.SMIn.accel_body[0]);
      c[0] = !rtIsNaN(StateMachine_U.SMIn.accel_body[0]);
      b[1] = !rtIsInf(StateMachine_U.SMIn.accel_body[1]);
      c[1] = !rtIsNaN(StateMachine_U.SMIn.accel_body[1]);
      b[2] = !rtIsInf(StateMachine_U.SMIn.accel_body[2]);
      c[2] = !rtIsNaN(StateMachine_U.SMIn.accel_body[2]);
      y = true;
      k = 0;
      exitg1 = false;
      while ((!exitg1) && (k < 3)) {
        if ((!b[k]) || (!c[k])) {
          y = false;
          exitg1 = true;
        } else {
          k++;
        }
      }

      y = (y && ((!rtIsInf(StateMachine_U.SMIn.timestamp_s)) && (!rtIsNaN
             (StateMachine_U.SMIn.timestamp_s))));
    } else {
      y = false;
    }
  } else {
    y = false;
  }

  cmd = 0U;
  if (StateMachine_U.SMIn.telemetry_cmd.cmd_valid == 1) {
    cmd = StateMachine_U.SMIn.telemetry_cmd.cmd_id;
  }

  if (StateMachine_U.SMIn.nav_valid == 0) {
    if (rtb_sm_state_out_err_cnt < 65535) {
      rtb_sm_state_out_err_cnt++;
    }
  } else {
    rtb_sm_state_out_err_cnt = 0U;
    rtb_sm_state_out_nav_ever_ok = 1U;
  }

  if (StateMachine_U.SMIn.timestamp_s <= t_in_state) {
    if (rtb_sm_state_out_stall_cnt < 65535) {
      rtb_sm_state_out_stall_cnt++;
    }
  } else {
    rtb_sm_state_out_stall_cnt = 0U;
  }

  fault_code = 0U;
  if (!y) {
    fault_code = 3U;
  } else if (rtb_sm_state_out_stall_cnt >= 50) {
    fault_code = 2U;
  } else {
    if (rtb_sm_state_out_nav_ever_ok == 1) {
      tmp_0 = 100U;
    } else {
      tmp_0 = 6000U;
    }

    if (rtb_sm_state_out_err_cnt >= tmp_0) {
      fault_code = 1U;
    }
  }

  if (accel_mag / 9.80665 >= sm_tun.launch_accel_g_thr) {
    if (rtb_sm_state_out_launch_cnt < 65535) {
      rtb_sm_state_out_launch_cnt++;
    }
  } else {
    rtb_sm_state_out_launch_cnt = 0U;
  }

  if ((StateMachine_U.SMIn.nav_valid == 1) && (StateMachine_U.SMIn.gnss_age_s <=
       3.0F)) {
    if (fabs(accel_mag - 9.80665) <= 1.5) {
      tmp[0] = StateMachine_U.SMIn.gyro_body[0];
      tmp[1] = StateMachine_U.SMIn.gyro_body[1];
      tmp[2] = StateMachine_U.SMIn.gyro_body[2];
      if (StateMachine_norm(tmp) <= 0.05235987755982989) {
        tmp[0] = StateMachine_U.SMIn.vel_ned[0];
        tmp[1] = StateMachine_U.SMIn.vel_ned[1];
        tmp[2] = StateMachine_U.SMIn.vel_ned[2];
        y = (StateMachine_norm(tmp) <= 2.0);
      } else {
        y = false;
      }
    } else {
      y = false;
    }
  } else {
    y = false;
  }

  t_in_state = StateMachine_U.SMIn.timestamp_s - rtb_sm_state_out_t_entry_s;
  switch (rtb_sm_state_out_state) {
   case 0:
    if (rtb_sm_state_out_launch_cnt >= 6) {
      new_state = 3;
    } else if (cmd == 255) {
      new_state = 4;
      rtb_sm_state_out_error_code = 4U;
    } else if (fault_code != 0) {
      new_state = 4;
      rtb_sm_state_out_error_code = fault_code;
    } else if (cmd == 1) {
      new_state = 1;
    } else if ((cmd == 3) && (t_in_state >= 2.0) && y) {
      new_state = 2;
    }
    break;

   case 1:
    if (cmd == 255) {
      new_state = 4;
      rtb_sm_state_out_error_code = 4U;
    } else if (cmd == 2) {
      new_state = 0;
    }
    break;

   case 2:
    if (rtb_sm_state_out_launch_cnt >= 6) {
      new_state = 3;
    } else if (cmd == 255) {
      new_state = 4;
      rtb_sm_state_out_error_code = 4U;
    } else if (fault_code != 0) {
      new_state = 4;
      rtb_sm_state_out_error_code = fault_code;
    } else if (cmd == 4) {
      new_state = 0;
    }
    break;

   case 3:
    if ((fault_code != 0) && (rtb_sm_state_out_error_code == 0)) {
      rtb_sm_state_out_error_code = fault_code;
    }
    break;

   case 4:
    if (rtb_sm_state_out_launch_cnt >= 6) {
      new_state = 3;
    } else if (cmd == 5) {
      new_state = 0;
      rtb_sm_state_out_error_code = 0U;
      rtb_sm_state_out_err_cnt = 0U;
      rtb_sm_state_out_stall_cnt = 0U;
    }
    break;

   default:
    new_state = 4;
    rtb_sm_state_out_error_code = 3U;
    break;
  }

  if (new_state != rtb_sm_state_out_state) {
    rtb_sm_state_out_prev_state = rtb_sm_state_out_state;
    rtb_sm_state_out_state = new_state;
    rtb_sm_state_out_t_entry_s = StateMachine_U.SMIn.timestamp_s;
    rtb_sm_state_out_launch_cnt = 0U;
    t_in_state = 0.0;
  }

  /* Outport: '<Root>/SMOut' incorporates:
   *  BusCreator generated from: '<Root>/StateMachine_Logic'
   *  Inport: '<Root>/SMIn'
   *  MATLAB Function: '<Root>/StateMachine_Logic'
   */
  StateMachine_Y.SMOut.rocket_state = rtb_sm_state_out_state;
  StateMachine_Y.SMOut.prev_state = rtb_sm_state_out_prev_state;
  StateMachine_Y.SMOut.time_in_state_s = (real32_T)t_in_state;
  StateMachine_Y.SMOut.error_code = rtb_sm_state_out_error_code;
  StateMachine_Y.SMOut.timestamp_s = StateMachine_U.SMIn.timestamp_s;

  /* Update for Memory: '<Root>/SMState_Memory' incorporates:
   *  BusCreator generated from: '<Root>/StateMachine_Logic'
   *  Inport: '<Root>/SMIn'
   */
  StateMachine_DW.SMState_Memory_PreviousInput.state = rtb_sm_state_out_state;
  StateMachine_DW.SMState_Memory_PreviousInput.prev_state =
    rtb_sm_state_out_prev_state;
  StateMachine_DW.SMState_Memory_PreviousInput.t_entry_s =
    rtb_sm_state_out_t_entry_s;
  StateMachine_DW.SMState_Memory_PreviousInput.launch_cnt =
    rtb_sm_state_out_launch_cnt;
  StateMachine_DW.SMState_Memory_PreviousInput.err_cnt =
    rtb_sm_state_out_err_cnt;
  StateMachine_DW.SMState_Memory_PreviousInput.stall_cnt =
    rtb_sm_state_out_stall_cnt;
  StateMachine_DW.SMState_Memory_PreviousInput.nav_ever_ok =
    rtb_sm_state_out_nav_ever_ok;
  StateMachine_DW.SMState_Memory_PreviousInput.error_code =
    rtb_sm_state_out_error_code;
  StateMachine_DW.SMState_Memory_PreviousInput.t_prev_s =
    StateMachine_U.SMIn.timestamp_s;
  StateMachine_DW.SMState_Memory_PreviousInput.init_done =
    rtb_sm_state_out_init_done;
}

/* Model initialize function */
void StateMachine_initialize(void)
{
  /* (no initialization code required) */
}

/* Model terminate function */
void StateMachine_terminate(void)
{
  /* (no terminate code required) */
}

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
