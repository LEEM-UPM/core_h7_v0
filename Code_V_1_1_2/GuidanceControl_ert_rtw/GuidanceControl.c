/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: GuidanceControl.c
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

#include "GuidanceControl.h"
#include "rtwtypes.h"
#include "GuidanceControl_types.h"
#include <math.h>
#include "rt_nonfinite.h"
#include "GuidanceControl_private.h"

/* Exported block parameters */
Bus_GCTunables gc_tun = {
  3600.0F,
  1200.0F,
  300.0F,
  1.0F
} ;                                    /* Variable: gc_tun
                                        * Referenced by: '<Root>/GuidanceControl_Logic'
                                        */

/* Block states (default storage) */
DW_GuidanceControl_T GuidanceControl_DW;

/* External inputs (root inport signals with default storage) */
ExtU_GuidanceControl_T GuidanceControl_U;

/* External outputs (root outports fed by signals with default storage) */
ExtY_GuidanceControl_T GuidanceControl_Y;

/* Real-time model */
static RT_MODEL_GuidanceControl_T GuidanceControl_M_;
RT_MODEL_GuidanceControl_T *const GuidanceControl_M = &GuidanceControl_M_;

/* Forward declaration for local functions */
static real_T GuidanceControl_norm(const real_T x[3]);
static void GuidanceControl_isa_atmos(real_T alt_msl, real_T b_gc_data_g0,
  real_T b_gc_data_isa_T0, real_T b_gc_data_isa_p0, real_T b_gc_data_isa_L,
  real_T b_gc_data_isa_R, real_T b_gc_data_isa_gamma, real_T *rho, real_T *p,
  real_T *a_snd);
static real_T GuidanceControl_cd_lookup(real_T mach, real_T u, real_T
  b_gc_data_mach_min, real_T b_gc_data_mach_step, int32_T b_gc_data_n_mach,
  int32_T b_gc_data_n_deploy, const real32_T b_gc_data_cd_tbl[215], const
  real32_T b_gc_data_deploy_tbl[5]);
static void GuidanceControl_ballistic_deriv(real_T h_agl, const real_T v[3],
  real_T u, const struct_fab7xe5zbqKImI9YqNejpE *b_gc_data, real_T *dh, real_T
  dv[3]);
static void GuidanceContro_propagate_apogee(real_T alt_agl0, const real_T vel0[3],
  real_T u, const struct_fab7xe5zbqKImI9YqNejpE *b_gc_data, real_T *apogee_agl,
  real_T *t_to_apogee, uint8_T *pred_valid);
static real_T GuidanceControl_invert_cd(real_T cd_target, real_T mach, real_T
  b_gc_data_mach_min, real_T b_gc_data_mach_step, int32_T b_gc_data_n_mach,
  int32_T b_gc_data_n_deploy, const real32_T b_gc_data_cd_tbl[215], const
  real32_T b_gc_data_deploy_tbl[5]);

/* Function for MATLAB Function: '<Root>/GuidanceControl_Logic' */
static real_T GuidanceControl_norm(const real_T x[3])
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

real_T rt_powd_snf(real_T u0, real_T u1)
{
  real_T y;
  if (rtIsNaN(u0) || rtIsNaN(u1)) {
    y = (rtNaN);
  } else {
    real_T tmp;
    real_T tmp_0;
    tmp = fabs(u0);
    tmp_0 = fabs(u1);
    if (rtIsInf(u1)) {
      if (tmp == 1.0) {
        y = 1.0;
      } else if (tmp > 1.0) {
        if (u1 > 0.0) {
          y = (rtInf);
        } else {
          y = 0.0;
        }
      } else if (u1 > 0.0) {
        y = 0.0;
      } else {
        y = (rtInf);
      }
    } else if (tmp_0 == 0.0) {
      y = 1.0;
    } else if (tmp_0 == 1.0) {
      if (u1 > 0.0) {
        y = u0;
      } else {
        y = 1.0 / u0;
      }
    } else if (u1 == 2.0) {
      y = u0 * u0;
    } else if ((u1 == 0.5) && (u0 >= 0.0)) {
      y = sqrt(u0);
    } else if ((u0 < 0.0) && (u1 > floor(u1))) {
      y = (rtNaN);
    } else {
      y = pow(u0, u1);
    }
  }

  return y;
}

/* Function for MATLAB Function: '<Root>/GuidanceControl_Logic' */
static void GuidanceControl_isa_atmos(real_T alt_msl, real_T b_gc_data_g0,
  real_T b_gc_data_isa_T0, real_T b_gc_data_isa_p0, real_T b_gc_data_isa_L,
  real_T b_gc_data_isa_R, real_T b_gc_data_isa_gamma, real_T *rho, real_T *p,
  real_T *a_snd)
{
  real_T T;
  T = b_gc_data_isa_L * alt_msl + b_gc_data_isa_T0;
  if (T < 216.65) {
    T = 216.65;
  }

  *p = rt_powd_snf(T / b_gc_data_isa_T0, -b_gc_data_g0 / (b_gc_data_isa_L *
    b_gc_data_isa_R)) * b_gc_data_isa_p0;
  *rho = *p / (b_gc_data_isa_R * T);
  *a_snd = sqrt(b_gc_data_isa_gamma * b_gc_data_isa_R * T);
}

/* Function for MATLAB Function: '<Root>/GuidanceControl_Logic' */
static real_T GuidanceControl_cd_lookup(real_T mach, real_T u, real_T
  b_gc_data_mach_min, real_T b_gc_data_mach_step, int32_T b_gc_data_n_mach,
  int32_T b_gc_data_n_deploy, const real32_T b_gc_data_cd_tbl[215], const
  real32_T b_gc_data_deploy_tbl[5])
{
  real_T cd_tmp;
  real_T i0;
  real_T uu;
  real_T x;
  int32_T ia;
  int32_T jd;
  int32_T k;
  real32_T tmp;
  x = fmin(fmax((mach - b_gc_data_mach_min) / b_gc_data_mach_step, 0.0), (real_T)
           b_gc_data_n_mach - 1.0);
  i0 = floor(x);
  if (i0 > (real_T)b_gc_data_n_mach - 2.0) {
    i0 = (real_T)b_gc_data_n_mach - 2.0;
  }

  x -= i0;
  if (i0 >= -2.147483648E+9) {
    ia = (int32_T)i0;
  } else {
    ia = MIN_int32_T;
  }

  uu = fmin(fmax(u, 0.0), 1.0);
  jd = 1;
  for (k = 0; k <= b_gc_data_n_deploy - 2; k++) {
    if (uu >= b_gc_data_deploy_tbl[k]) {
      if ((uint32_T)k + 1U < 2147483648U) {
        jd = (int32_T)((uint32_T)k + 1U);
      } else {
        jd = MAX_int32_T;
      }
    }
  }

  if ((real_T)b_gc_data_n_deploy - 1.0 >= -2.147483648E+9) {
    k = b_gc_data_n_deploy - 1;
  } else {
    k = MIN_int32_T;
  }

  if (jd > k) {
    if ((real_T)b_gc_data_n_deploy - 1.0 >= -2.147483648E+9) {
      jd = b_gc_data_n_deploy - 1;
    } else {
      jd = MIN_int32_T;
    }
  }

  i0 = 0.0;
  tmp = b_gc_data_deploy_tbl[jd - 1];
  if (b_gc_data_deploy_tbl[jd] > tmp) {
    i0 = (uu - tmp) / ((real_T)b_gc_data_deploy_tbl[jd] - tmp);
  }

  k = (jd - 1) * 43 + ia;
  uu = b_gc_data_cd_tbl[k];
  uu += (b_gc_data_cd_tbl[k + 1] - uu) * x;
  ia += 43 * jd;
  cd_tmp = b_gc_data_cd_tbl[ia];
  return (((b_gc_data_cd_tbl[ia + 1] - cd_tmp) * x + cd_tmp) - uu) * i0 + uu;
}

/* Function for MATLAB Function: '<Root>/GuidanceControl_Logic' */
static void GuidanceControl_ballistic_deriv(real_T h_agl, const real_T v[3],
  real_T u, const struct_fab7xe5zbqKImI9YqNejpE *b_gc_data, real_T *dh, real_T
  dv[3])
{
  real_T T;
  real_T sp;
  T = (b_gc_data->alt_ref_msl_m + h_agl) * b_gc_data->isa_L + b_gc_data->isa_T0;
  if (T < 216.65) {
    T = 216.65;
  }

  sp = GuidanceControl_norm(v);
  if (sp > 1.0E-6) {
    T = -(rt_powd_snf(T / b_gc_data->isa_T0, -b_gc_data->g0 / (b_gc_data->isa_L *
            b_gc_data->isa_R)) * b_gc_data->isa_p0 / (b_gc_data->isa_R * T) *
          0.5 * sp * b_gc_data->S_ref_m2 * GuidanceControl_cd_lookup(sp / sqrt
           (b_gc_data->isa_gamma * b_gc_data->isa_R * T), u, b_gc_data->mach_min,
           b_gc_data->mach_step, b_gc_data->n_mach, b_gc_data->n_deploy,
           b_gc_data->cd_tbl, b_gc_data->deploy_tbl) / b_gc_data->mass_dry_kg);
    dv[0] = T * v[0];
    dv[1] = T * v[1];
    T *= v[2];
  } else {
    dv[0] = 0.0;
    dv[1] = 0.0;
    T = 0.0;
  }

  dv[2] = T + b_gc_data->g0;
  *dh = -v[2];
}

/* Function for MATLAB Function: '<Root>/GuidanceControl_Logic' */
static void GuidanceContro_propagate_apogee(real_T alt_agl0, const real_T vel0[3],
  real_T u, const struct_fab7xe5zbqKImI9YqNejpE *b_gc_data, real_T *apogee_agl,
  real_T *t_to_apogee, uint8_T *pred_valid)
{
  real_T dv1[3];
  real_T dv2[3];
  real_T v[3];
  real_T v_0[3];
  real_T den;
  real_T frac;
  real_T h;
  real_T h_old;
  real_T u_step;
  real_T vd_old;
  int32_T b_k;
  int32_T k;
  boolean_T b[3];
  boolean_T c[3];
  boolean_T exitg1;
  boolean_T exitg2;
  boolean_T y;
  h = alt_agl0;
  v[0] = vel0[0];
  v[1] = vel0[1];
  v[2] = vel0[2];
  *apogee_agl = alt_agl0;
  *t_to_apogee = 0.0;
  *pred_valid = 0U;
  if (-vel0[2] <= 0.0) {
    *pred_valid = 1U;
  } else {
    k = 0;
    exitg1 = false;
    while ((!exitg1) && (k <= b_gc_data->prop_max_steps - 1)) {
      h_old = h;
      vd_old = v[2];
      u_step = u;
      if (-v[2] <= b_gc_data->v_ctrl_min_mps) {
        u_step = 0.0;
      }

      GuidanceControl_ballistic_deriv(h, v, u_step, b_gc_data, &frac, dv1);
      v_0[0] = b_gc_data->prop_dt * dv1[0] + v[0];
      v_0[1] = b_gc_data->prop_dt * dv1[1] + v[1];
      v_0[2] = b_gc_data->prop_dt * dv1[2] + v[2];
      GuidanceControl_ballistic_deriv(h + b_gc_data->prop_dt * frac, v_0, u_step,
        b_gc_data, &den, dv2);
      u_step = 0.5 * b_gc_data->prop_dt;
      h += (frac + den) * u_step;
      v[0] += (dv1[0] + dv2[0]) * u_step;
      v[1] += (dv1[1] + dv2[1]) * u_step;
      v[2] += (dv1[2] + dv2[2]) * u_step;
      if (v[2] >= 0.0) {
        den = v[2] - vd_old;
        frac = 1.0;
        if (fabs(den) > 1.0E-9) {
          frac = fmin(fmax(-vd_old / den, 0.0), 1.0);
        }

        *apogee_agl = (h - h_old) * frac + h_old;
        *t_to_apogee = ((((real_T)k + 1.0) - 1.0) + frac) * b_gc_data->prop_dt;
        *pred_valid = 1U;
        exitg1 = true;
      } else if (rtIsInf(h) || rtIsNaN(h)) {
        *t_to_apogee = ((real_T)k + 1.0) * b_gc_data->prop_dt;
        exitg1 = true;
      } else {
        b[0] = !rtIsInf(v[0]);
        c[0] = !rtIsNaN(v[0]);
        b[1] = !rtIsInf(v[1]);
        c[1] = !rtIsNaN(v[1]);
        b[2] = !rtIsInf(v[2]);
        c[2] = !rtIsNaN(v[2]);
        y = true;
        b_k = 0;
        exitg2 = false;
        while ((!exitg2) && (b_k < 3)) {
          if ((!b[b_k]) || (!c[b_k])) {
            y = false;
            exitg2 = true;
          } else {
            b_k++;
          }
        }

        if (!y) {
          *t_to_apogee = ((real_T)k + 1.0) * b_gc_data->prop_dt;
          exitg1 = true;
        } else {
          *apogee_agl = h;
          *t_to_apogee = ((real_T)k + 1.0) * b_gc_data->prop_dt;
          k++;
        }
      }
    }
  }
}

/* Function for MATLAB Function: '<Root>/GuidanceControl_Logic' */
static real_T GuidanceControl_invert_cd(real_T cd_target, real_T mach, real_T
  b_gc_data_mach_min, real_T b_gc_data_mach_step, int32_T b_gc_data_n_mach,
  int32_T b_gc_data_n_deploy, const real32_T b_gc_data_cd_tbl[215], const
  real32_T b_gc_data_deploy_tbl[5])
{
  real_T i0;
  real_T u;
  real_T x;
  int32_T ia;
  x = fmin(fmax((mach - b_gc_data_mach_min) / b_gc_data_mach_step, 0.0), (real_T)
           b_gc_data_n_mach - 1.0);
  i0 = floor(x);
  if (i0 > (real_T)b_gc_data_n_mach - 2.0) {
    i0 = (real_T)b_gc_data_n_mach - 2.0;
  }

  x -= i0;
  if (i0 >= -2.147483648E+9) {
    ia = (int32_T)i0;
  } else {
    ia = MIN_int32_T;
  }

  i0 = ((real_T)b_gc_data_cd_tbl[ia + 1] - b_gc_data_cd_tbl[ia]) * x +
    b_gc_data_cd_tbl[ia];
  if (cd_target <= i0) {
    u = 0.0;
  } else {
    real_T prev_u;
    int32_T k;
    boolean_T exitg1;
    u = 1.0;
    prev_u = b_gc_data_deploy_tbl[0];
    k = 1;
    exitg1 = false;
    while ((!exitg1) && (k - 1 <= b_gc_data_n_deploy - 2)) {
      real_T cdk;
      int32_T cdk_tmp;
      cdk_tmp = 43 * k + ia;
      cdk = b_gc_data_cd_tbl[cdk_tmp];
      cdk += (b_gc_data_cd_tbl[cdk_tmp + 1] - cdk) * x;
      if (cd_target <= cdk) {
        if (cdk > i0) {
          u = (cd_target - i0) / (cdk - i0) * (b_gc_data_deploy_tbl[k] - prev_u)
            + prev_u;
        } else {
          u = b_gc_data_deploy_tbl[k];
        }

        exitg1 = true;
      } else {
        i0 = cdk;
        prev_u = b_gc_data_deploy_tbl[k];
        k++;
      }
    }
  }

  return u;
}

/* Model step function */
void GuidanceControl_step(void)
{
  real_T vel[3];
  real_T a__3;
  real_T a__4;
  real_T alt_agl;
  real_T apogee_err;
  real_T cmd_param;
  real_T dt;
  real_T dt_meas;
  real_T mach;
  real_T rtb_gc_state_out_t_apogee_s;
  real_T rtb_gc_state_out_t_drogue_s;
  real_T rtb_gc_state_out_t_main_s;
  real_T rtb_gc_state_out_t_phase_s;
  real_T speed;
  real_T t_to_apogee;
  real_T u_target;
  int32_T new_phase;
  int32_T phase;
  int32_T rtb_gc_state_out_phase;
  real32_T rtb_gc_state_out_ground_alt_m;
  real32_T rtb_gc_state_out_integ;
  real32_T rtb_gc_state_out_max_alt_m;
  uint16_T rtb_gc_state_out_apogee_cnt;
  uint16_T rtb_gc_state_out_burnout_cnt;
  uint16_T rtb_gc_state_out_land_cnt;
  uint16_T rtb_gc_state_out_nav_ok_cnt;
  uint8_T cmd;
  uint8_T rtb_gc_state_out_apogee_detecte;
  uint8_T rtb_gc_state_out_drogue_armed;
  uint8_T rtb_gc_state_out_ground_set;
  uint8_T rtb_gc_state_out_init_done;
  uint8_T rtb_gc_state_out_main_armed;
  boolean_T reset_ctrl;

  /* MATLAB Function: '<Root>/GuidanceControl_Logic' incorporates:
   *  BusCreator generated from: '<Root>/GuidanceControl_Logic'
   *  Inport: '<Root>/GCIn'
   *  Memory: '<Root>/GCState_Memory'
   */
  rtb_gc_state_out_phase = GuidanceControl_DW.GCState_Memory_PreviousInput.phase;
  rtb_gc_state_out_integ = GuidanceControl_DW.GCState_Memory_PreviousInput.integ;
  rtb_gc_state_out_max_alt_m =
    GuidanceControl_DW.GCState_Memory_PreviousInput.max_alt_m;
  rtb_gc_state_out_ground_alt_m =
    GuidanceControl_DW.GCState_Memory_PreviousInput.ground_alt_m;
  rtb_gc_state_out_ground_set =
    GuidanceControl_DW.GCState_Memory_PreviousInput.ground_set;
  rtb_gc_state_out_t_phase_s =
    GuidanceControl_DW.GCState_Memory_PreviousInput.t_phase_s;
  rtb_gc_state_out_t_apogee_s =
    GuidanceControl_DW.GCState_Memory_PreviousInput.t_apogee_s;
  rtb_gc_state_out_t_drogue_s =
    GuidanceControl_DW.GCState_Memory_PreviousInput.t_drogue_s;
  rtb_gc_state_out_t_main_s =
    GuidanceControl_DW.GCState_Memory_PreviousInput.t_main_s;
  rtb_gc_state_out_apogee_detecte =
    GuidanceControl_DW.GCState_Memory_PreviousInput.apogee_detected;
  rtb_gc_state_out_drogue_armed =
    GuidanceControl_DW.GCState_Memory_PreviousInput.drogue_armed;
  rtb_gc_state_out_main_armed =
    GuidanceControl_DW.GCState_Memory_PreviousInput.main_armed;
  rtb_gc_state_out_burnout_cnt =
    GuidanceControl_DW.GCState_Memory_PreviousInput.burnout_cnt;
  rtb_gc_state_out_nav_ok_cnt =
    GuidanceControl_DW.GCState_Memory_PreviousInput.nav_ok_cnt;
  rtb_gc_state_out_apogee_cnt =
    GuidanceControl_DW.GCState_Memory_PreviousInput.apogee_cnt;
  rtb_gc_state_out_land_cnt =
    GuidanceControl_DW.GCState_Memory_PreviousInput.land_cnt;
  rtb_gc_state_out_init_done =
    GuidanceControl_DW.GCState_Memory_PreviousInput.init_done;
  dt = 0.01;
  if (GuidanceControl_DW.GCState_Memory_PreviousInput.init_done == 1) {
    dt_meas = GuidanceControl_U.GCIn.timestamp_s -
      GuidanceControl_DW.GCState_Memory_PreviousInput.t_prev_s;
    if ((dt_meas > 0.002) && (dt_meas < 0.05)) {
      dt = dt_meas;
    }
  }

  if (GuidanceControl_DW.GCState_Memory_PreviousInput.init_done == 0) {
    rtb_gc_state_out_phase = 0;
    rtb_gc_state_out_t_phase_s = GuidanceControl_U.GCIn.timestamp_s;
    rtb_gc_state_out_init_done = 1U;
  }

  phase = rtb_gc_state_out_phase;
  new_phase = rtb_gc_state_out_phase;
  vel[0] = GuidanceControl_U.GCIn.vel_ned[0];
  vel[1] = GuidanceControl_U.GCIn.vel_ned[1];
  vel[2] = GuidanceControl_U.GCIn.vel_ned[2];
  speed = GuidanceControl_norm(vel);
  if (((GuidanceControl_U.GCIn.rocket_state == 0) ||
       (GuidanceControl_U.GCIn.rocket_state == 2)) &&
      (GuidanceControl_U.GCIn.nav_valid == 1)) {
    rtb_gc_state_out_ground_alt_m = GuidanceControl_U.GCIn.pos_ned[2];
    rtb_gc_state_out_ground_set = 1U;
  }

  alt_agl = -((real_T)GuidanceControl_U.GCIn.pos_ned[2] -
              rtb_gc_state_out_ground_alt_m);
  GuidanceControl_isa_atmos(alt_agl + 165.0, 9.80665, 288.15, 101325.0, -0.0065,
    287.05287, 1.4, &dt_meas, &apogee_err, &t_to_apogee);
  mach = speed / t_to_apogee;
  if (alt_agl > GuidanceControl_DW.GCState_Memory_PreviousInput.max_alt_m) {
    rtb_gc_state_out_max_alt_m = (real32_T)alt_agl;
  }

  cmd = 0U;
  cmd_param = 0.0;
  if (GuidanceControl_U.GCIn.telemetry_cmd.cmd_valid == 1) {
    cmd = GuidanceControl_U.GCIn.telemetry_cmd.cmd_id;
    cmd_param = GuidanceControl_U.GCIn.telemetry_cmd.cmd_param;
  }

  u_target = 0.0;
  dt_meas = GuidanceControl_DW.GCState_Memory_PreviousInput.apogee_pred_m;
  apogee_err = 0.0;
  t_to_apogee = 0.0;

  /* Outport: '<Root>/GCOut' incorporates:
   *  MATLAB Function: '<Root>/GuidanceControl_Logic'
   */
  GuidanceControl_Y.GCOut.pred_valid = 1U;
  GuidanceControl_Y.GCOut.drogue_deploy_flag = 0U;
  GuidanceControl_Y.GCOut.main_deploy_flag = 0U;

  /* MATLAB Function: '<Root>/GuidanceControl_Logic' incorporates:
   *  BusCreator generated from: '<Root>/GuidanceControl_Logic'
   *  Inport: '<Root>/GCIn'
   *  Memory: '<Root>/GCState_Memory'
   */
  reset_ctrl = true;
  switch (GuidanceControl_U.GCIn.rocket_state) {
   case 0:
    new_phase = 0;
    break;

   case 1:
    new_phase = 0;
    if (cmd == 6) {
      u_target = fmin(fmax(cmd_param, 0.0), 1.0);
    }
    break;

   case 2:
    new_phase = 0;
    rtb_gc_state_out_max_alt_m = 0.0F;
    rtb_gc_state_out_apogee_detecte = 0U;
    rtb_gc_state_out_drogue_armed = 0U;
    rtb_gc_state_out_main_armed = 0U;
    rtb_gc_state_out_burnout_cnt = 0U;
    rtb_gc_state_out_apogee_cnt = 0U;
    rtb_gc_state_out_land_cnt = 0U;
    break;

   case 3:
    if (rtb_gc_state_out_phase == 0) {
      phase = 1;
      new_phase = 1;
    }

    switch (phase) {
     case 1:
      alt_agl = 0.0;
      if (speed > 5.0) {
        alt_agl = (((real_T)GuidanceControl_U.GCIn.accel_ned[0] *
                    GuidanceControl_U.GCIn.vel_ned[0] + (real_T)
                    GuidanceControl_U.GCIn.accel_ned[1] *
                    GuidanceControl_U.GCIn.vel_ned[1]) + (real_T)
                   GuidanceControl_U.GCIn.accel_ned[2] *
                   GuidanceControl_U.GCIn.vel_ned[2]) / speed;
      }

      speed = GuidanceControl_U.GCIn.timestamp_s - rtb_gc_state_out_t_phase_s;
      if ((speed >= 1.0) && (alt_agl <= -2.0)) {
        if (GuidanceControl_DW.GCState_Memory_PreviousInput.burnout_cnt < 65535)
        {
          rtb_gc_state_out_burnout_cnt = (uint16_T)
            (GuidanceControl_DW.GCState_Memory_PreviousInput.burnout_cnt + 1);
        }
      } else {
        rtb_gc_state_out_burnout_cnt = 0U;
      }

      if (rtb_gc_state_out_burnout_cnt >= 10) {
        new_phase = 2;
      }

      if ((speed >= 1.0) && (-(real_T)GuidanceControl_U.GCIn.vel_ned[2] <= 0.0))
      {
        new_phase = 4;
      }
      break;

     case 2:
      if (GuidanceControl_U.GCIn.gnss_age_s <= 0.5F) {
        if (GuidanceControl_DW.GCState_Memory_PreviousInput.nav_ok_cnt < 65535)
        {
          rtb_gc_state_out_nav_ok_cnt = (uint16_T)
            (GuidanceControl_DW.GCState_Memory_PreviousInput.nav_ok_cnt + 1);
        }
      } else {
        rtb_gc_state_out_nav_ok_cnt = 0U;
      }

      if (-(real_T)GuidanceControl_U.GCIn.vel_ned[2] <= 0.0) {
        new_phase = 4;
      } else if ((GuidanceControl_U.GCIn.timestamp_s -
                  rtb_gc_state_out_t_phase_s >= 1.0) && (alt_agl >=
                  gc_tun.min_ctrl_alt_agl_m) && (mach <= 1.05) && (-(real_T)
                  GuidanceControl_U.GCIn.vel_ned[2] >= 30.0)) {
        new_phase = 3;
      }
      break;

     case 3:
      GuidanceContro_propagate_apogee(alt_agl, vel, (real_T)
        GuidanceControl_DW.GCState_Memory_PreviousInput.u_cmd,
        &GuidanceControl_ConstP.GuidanceControl_Logic_gc_data, &dt_meas,
        &t_to_apogee, &cmd);

      /* Outport: '<Root>/GCOut' */
      GuidanceControl_Y.GCOut.pred_valid = cmd;
      apogee_err = dt_meas - gc_tun.apogee_target_agl_m;
      u_target = 0.0;
      if (apogee_err > 5.0) {
        u_target = apogee_err - 5.0;
      } else if (apogee_err < -5.0) {
        u_target = apogee_err + 5.0;
      }

      GuidanceControl_isa_atmos(alt_agl + 165.0, 9.80665, 288.15, 101325.0,
        -0.0065, 287.05287, 1.4, &cmd_param, &a__3, &a__4);
      alt_agl = GuidanceControl_DW.GCState_Memory_PreviousInput.u_cmd;
      if ((cmd == 1) && (0.5 * cmd_param * speed * speed > 500.0) && (-(real_T)
           GuidanceControl_U.GCIn.vel_ned[2] > 1.0)) {
        alt_agl = fmax(t_to_apogee, 1.5);
        alt_agl = GuidanceControl_invert_cd(2.0 * u_target / (alt_agl * alt_agl)
          * 18.968 * speed / -(real_T)GuidanceControl_U.GCIn.vel_ned[2] * 2.0 /
          (cmd_param * speed * speed * 0.013) + GuidanceControl_cd_lookup(mach,
          (real_T)GuidanceControl_DW.GCState_Memory_PreviousInput.u_cmd, 0.0,
          0.05, 43, 5,
          &GuidanceControl_ConstP.GuidanceControl_Logic_gc_data.cd_tbl[0],
          &GuidanceControl_ConstP.GuidanceControl_Logic_gc_data.deploy_tbl[0]),
          mach, 0.0, 0.05, 43, 5,
          &GuidanceControl_ConstP.GuidanceControl_Logic_gc_data.cd_tbl[0],
          &GuidanceControl_ConstP.GuidanceControl_Logic_gc_data.deploy_tbl[0]);
      }

      if (cmd == 0) {
        u_target = 0.0;
      }

      speed = fmin(fmax(u_target * dt +
                        GuidanceControl_DW.GCState_Memory_PreviousInput.integ,
                        -3000.0), 3000.0);
      mach = 0.0003 * speed + alt_agl;
      if (((mach > 1.0) && (u_target > 0.0)) || ((mach < 0.0) && (u_target < 0.0)))
      {
        rtb_gc_state_out_integ =
          GuidanceControl_DW.GCState_Memory_PreviousInput.integ;
        mach = 0.0003 * GuidanceControl_DW.GCState_Memory_PreviousInput.integ +
          alt_agl;
      } else {
        rtb_gc_state_out_integ = (real32_T)speed;
      }

      u_target = fmin(fmax(mach, 0.0), 1.0);
      reset_ctrl = false;
      if (-(real_T)GuidanceControl_U.GCIn.vel_ned[2] <= 30.0) {
        new_phase = 4;
      }
      break;

     case 4:
      if (GuidanceControl_DW.GCState_Memory_PreviousInput.apogee_detected == 0)
      {
        if ((-(real_T)GuidanceControl_U.GCIn.vel_ned[2] <= 0.0) ||
            (rtb_gc_state_out_max_alt_m - alt_agl >= 5.0)) {
          if (GuidanceControl_DW.GCState_Memory_PreviousInput.apogee_cnt < 65535)
          {
            rtb_gc_state_out_apogee_cnt = (uint16_T)
              (GuidanceControl_DW.GCState_Memory_PreviousInput.apogee_cnt + 1);
          }
        } else {
          rtb_gc_state_out_apogee_cnt = 0U;
        }

        if (rtb_gc_state_out_apogee_cnt >= 10) {
          rtb_gc_state_out_apogee_detecte = 1U;
          rtb_gc_state_out_t_apogee_s = GuidanceControl_U.GCIn.timestamp_s;
        }
      }

      if ((rtb_gc_state_out_apogee_detecte == 1) &&
          (GuidanceControl_U.GCIn.timestamp_s - rtb_gc_state_out_t_apogee_s >=
           gc_tun.drogue_delay_s)) {
        new_phase = 5;
        rtb_gc_state_out_drogue_armed = 1U;
        rtb_gc_state_out_t_drogue_s = GuidanceControl_U.GCIn.timestamp_s;
      }
      break;

     case 5:
      speed = GuidanceControl_U.GCIn.timestamp_s -
        GuidanceControl_DW.GCState_Memory_PreviousInput.t_drogue_s;
      if (speed < 1.0) {
        /* Outport: '<Root>/GCOut' */
        GuidanceControl_Y.GCOut.drogue_deploy_flag = 1U;
      }

      if ((speed >= 120.0) || ((alt_agl <= gc_tun.main_deploy_alt_agl_m) &&
           (-(real_T)GuidanceControl_U.GCIn.vel_ned[2] < 0.0))) {
        new_phase = 6;
        rtb_gc_state_out_main_armed = 1U;
        rtb_gc_state_out_t_main_s = GuidanceControl_U.GCIn.timestamp_s;
      }
      break;

     case 6:
      if (GuidanceControl_U.GCIn.timestamp_s -
          GuidanceControl_DW.GCState_Memory_PreviousInput.t_main_s < 1.0) {
        /* Outport: '<Root>/GCOut' */
        GuidanceControl_Y.GCOut.main_deploy_flag = 1U;
      }

      if ((speed <= 2.0) && (alt_agl <= 30.0)) {
        if (GuidanceControl_DW.GCState_Memory_PreviousInput.land_cnt < 65535) {
          rtb_gc_state_out_land_cnt = (uint16_T)
            (GuidanceControl_DW.GCState_Memory_PreviousInput.land_cnt + 1);
        }
      } else {
        rtb_gc_state_out_land_cnt = 0U;
      }

      if (rtb_gc_state_out_land_cnt >= 500) {
        new_phase = 7;
      }
      break;

     case 7:
      break;

     default:
      new_phase = 4;
      break;
    }
    break;

   case 4:
    new_phase = 0;
    break;

   default:
    new_phase = 0;
    break;
  }

  dt *= 4.0;
  dt = fmin(fmax(fmin(fmax(u_target -
    GuidanceControl_DW.GCState_Memory_PreviousInput.u_cmd, -dt), dt) +
                 GuidanceControl_DW.GCState_Memory_PreviousInput.u_cmd, 0.0),
            1.0);
  if (reset_ctrl) {
    rtb_gc_state_out_integ = 0.0F;
  }

  if (new_phase != rtb_gc_state_out_phase) {
    rtb_gc_state_out_phase = new_phase;
    rtb_gc_state_out_t_phase_s = GuidanceControl_U.GCIn.timestamp_s;
    rtb_gc_state_out_burnout_cnt = 0U;
  }

  /* Outport: '<Root>/GCOut' incorporates:
   *  BusCreator generated from: '<Root>/GuidanceControl_Logic'
   *  Inport: '<Root>/GCIn'
   *  MATLAB Function: '<Root>/GuidanceControl_Logic'
   */
  GuidanceControl_Y.GCOut.actuator_cmd = (real32_T)dt;
  GuidanceControl_Y.GCOut.airbrake_deg = (real32_T)(dt * 45.0);
  GuidanceControl_Y.GCOut.apogee_pred_m = (real32_T)dt_meas;
  GuidanceControl_Y.GCOut.apogee_error_m = (real32_T)apogee_err;
  GuidanceControl_Y.GCOut.time_to_apogee_s = (real32_T)t_to_apogee;
  GuidanceControl_Y.GCOut.flight_phase = rtb_gc_state_out_phase;
  GuidanceControl_Y.GCOut.timestamp_s = GuidanceControl_U.GCIn.timestamp_s;

  /* Update for Memory: '<Root>/GCState_Memory' incorporates:
   *  BusCreator generated from: '<Root>/GuidanceControl_Logic'
   *  Inport: '<Root>/GCIn'
   *  MATLAB Function: '<Root>/GuidanceControl_Logic'
   */
  GuidanceControl_DW.GCState_Memory_PreviousInput.phase = rtb_gc_state_out_phase;
  GuidanceControl_DW.GCState_Memory_PreviousInput.u_cmd = (real32_T)dt;
  GuidanceControl_DW.GCState_Memory_PreviousInput.integ = rtb_gc_state_out_integ;
  GuidanceControl_DW.GCState_Memory_PreviousInput.apogee_pred_m = (real32_T)
    dt_meas;
  GuidanceControl_DW.GCState_Memory_PreviousInput.max_alt_m =
    rtb_gc_state_out_max_alt_m;
  GuidanceControl_DW.GCState_Memory_PreviousInput.ground_alt_m =
    rtb_gc_state_out_ground_alt_m;
  GuidanceControl_DW.GCState_Memory_PreviousInput.ground_set =
    rtb_gc_state_out_ground_set;
  GuidanceControl_DW.GCState_Memory_PreviousInput.t_phase_s =
    rtb_gc_state_out_t_phase_s;
  GuidanceControl_DW.GCState_Memory_PreviousInput.t_prev_s =
    GuidanceControl_U.GCIn.timestamp_s;
  GuidanceControl_DW.GCState_Memory_PreviousInput.t_apogee_s =
    rtb_gc_state_out_t_apogee_s;
  GuidanceControl_DW.GCState_Memory_PreviousInput.t_drogue_s =
    rtb_gc_state_out_t_drogue_s;
  GuidanceControl_DW.GCState_Memory_PreviousInput.t_main_s =
    rtb_gc_state_out_t_main_s;
  GuidanceControl_DW.GCState_Memory_PreviousInput.apogee_detected =
    rtb_gc_state_out_apogee_detecte;
  GuidanceControl_DW.GCState_Memory_PreviousInput.drogue_armed =
    rtb_gc_state_out_drogue_armed;
  GuidanceControl_DW.GCState_Memory_PreviousInput.main_armed =
    rtb_gc_state_out_main_armed;
  GuidanceControl_DW.GCState_Memory_PreviousInput.burnout_cnt =
    rtb_gc_state_out_burnout_cnt;
  GuidanceControl_DW.GCState_Memory_PreviousInput.nav_ok_cnt =
    rtb_gc_state_out_nav_ok_cnt;
  GuidanceControl_DW.GCState_Memory_PreviousInput.apogee_cnt =
    rtb_gc_state_out_apogee_cnt;
  GuidanceControl_DW.GCState_Memory_PreviousInput.land_cnt =
    rtb_gc_state_out_land_cnt;
  GuidanceControl_DW.GCState_Memory_PreviousInput.init_done =
    rtb_gc_state_out_init_done;
}

/* Model initialize function */
void GuidanceControl_initialize(void)
{
  /* (no initialization code required) */
}

/* Model terminate function */
void GuidanceControl_terminate(void)
{
  /* (no terminate code required) */
}

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
