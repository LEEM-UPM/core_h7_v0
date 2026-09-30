/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: Navigation.c
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

#include "Navigation.h"
#include "rtwtypes.h"
#include <math.h>
#include "rt_nonfinite.h"
#include <string.h>
#include "Navigation_private.h"
#include "Navigation_types.h"
#include "rt_defines.h"

/* Exported block parameters */
Bus_NavTunables nav_tun = {
  { 0.0F, 1.47480321F, -0.0F }
} ;                                    /* Variable: nav_tun
                                        * Referenced by: '<Root>/INS_Mechanization'
                                        */

/* Block states (default storage) */
DW_Navigation_T Navigation_DW;

/* External inputs (root inport signals with default storage) */
ExtU_Navigation_T Navigation_U;

/* External outputs (root outports fed by signals with default storage) */
ExtY_Navigation_T Navigation_Y;

/* Real-time model */
static RT_MODEL_Navigation_T Navigation_M_;
RT_MODEL_Navigation_T *const Navigation_M = &Navigation_M_;

/* Forward declaration for local functions */
static real_T Navigation_norm(const real_T x[3]);
static real_T Navigation_norm_am(const real_T x[4]);
static void Navigation_eye(real_T b_I[9]);
static void Navigation_mldivide(const real_T A[9], const real_T B[3], real_T Y[3]);
static void Navigation_mrdiv_f(real_T A[90], const real_T B[36]);
static boolean_T Navigation_all(const boolean_T x[4]);
static boolean_T Navigation_all_l(const boolean_T x[3]);

/* Function for MATLAB Function: '<Root>/EKF_Prediction' */
static real_T Navigation_norm(const real_T x[3])
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

/* Function for MATLAB Function: '<Root>/EKF_Update' */
static real_T Navigation_norm_am(const real_T x[4])
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

  absxk = fabs(x[3]);
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
      if (b_k < 4) {
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

/* Function for MATLAB Function: '<Root>/EKF_Update' */
static void Navigation_eye(real_T b_I[9])
{
  memset(&b_I[0], 0, 9U * sizeof(real_T));
  b_I[0] = 1.0;
  b_I[4] = 1.0;
  b_I[8] = 1.0;
}

/* Function for MATLAB Function: '<Root>/EKF_Update' */
static void Navigation_mldivide(const real_T A[9], const real_T B[3], real_T Y[3])
{
  real_T b_A[9];
  real_T a21;
  real_T maxval;
  int32_T r1;
  int32_T r2;
  int32_T r3;
  memcpy(&b_A[0], &A[0], 9U * sizeof(real_T));
  r1 = 0;
  r2 = 1;
  r3 = 2;
  maxval = fabs(A[0]);
  a21 = fabs(A[1]);
  if (a21 > maxval) {
    maxval = a21;
    r1 = 1;
    r2 = 0;
  }

  if (fabs(A[2]) > maxval) {
    r1 = 2;
    r2 = 1;
    r3 = 0;
  }

  b_A[r2] = A[r2] / A[r1];
  b_A[r3] /= b_A[r1];
  b_A[r2 + 3] -= b_A[r1 + 3] * b_A[r2];
  b_A[r3 + 3] -= b_A[r1 + 3] * b_A[r3];
  b_A[r2 + 6] -= b_A[r1 + 6] * b_A[r2];
  b_A[r3 + 6] -= b_A[r1 + 6] * b_A[r3];
  if (fabs(b_A[r3 + 3]) > fabs(b_A[r2 + 3])) {
    int32_T rtemp;
    rtemp = r2;
    r2 = r3;
    r3 = rtemp;
  }

  b_A[r3 + 3] /= b_A[r2 + 3];
  b_A[r3 + 6] -= b_A[r3 + 3] * b_A[r2 + 6];
  Y[1] = B[r2] - B[r1] * b_A[r2];
  Y[2] = (B[r3] - B[r1] * b_A[r3]) - b_A[r3 + 3] * Y[1];
  Y[2] /= b_A[r3 + 6];
  Y[0] = B[r1] - b_A[r1 + 6] * Y[2];
  Y[1] -= b_A[r2 + 6] * Y[2];
  Y[1] /= b_A[r2 + 3];
  Y[0] -= b_A[r1 + 3] * Y[1];
  Y[0] /= b_A[r1];
}

/* Function for MATLAB Function: '<Root>/EKF_Update' */
static void Navigation_mrdiv_f(real_T A[90], const real_T B[36])
{
  real_T b_A[36];
  real_T smax;
  int32_T b_jBcol;
  int32_T d_j;
  int32_T jA;
  int32_T jBcol;
  int32_T jj;
  int32_T kBcol;
  int32_T n;
  int8_T ipiv[6];
  memcpy(&b_A[0], &B[0], 36U * sizeof(real_T));
  for (d_j = 0; d_j < 6; d_j++) {
    ipiv[d_j] = (int8_T)(d_j + 1);
  }

  for (d_j = 0; d_j < 5; d_j++) {
    jj = d_j * 7;
    n = 7 - d_j;
    b_jBcol = 0;
    smax = fabs(b_A[jj]);
    for (jBcol = 2; jBcol < n; jBcol++) {
      real_T s;
      s = fabs(b_A[(jj + jBcol) - 1]);
      if (s > smax) {
        b_jBcol = jBcol - 1;
        smax = s;
      }
    }

    if (b_A[jj + b_jBcol] != 0.0) {
      if (b_jBcol != 0) {
        n = d_j + b_jBcol;
        ipiv[d_j] = (int8_T)(n + 1);
        for (jBcol = 0; jBcol < 6; jBcol++) {
          b_jBcol = jBcol * 6 + d_j;
          smax = b_A[b_jBcol];
          jA = jBcol * 6 + n;
          b_A[b_jBcol] = b_A[jA];
          b_A[jA] = smax;
        }
      }

      n = (jj - d_j) + 6;
      for (jBcol = jj + 2; jBcol <= n; jBcol++) {
        b_A[jBcol - 1] /= b_A[jj];
      }
    }

    b_jBcol = 4 - d_j;
    jA = jj + 8;
    for (jBcol = 0; jBcol <= b_jBcol; jBcol++) {
      smax = b_A[(jBcol * 6 + jj) + 6];
      if (smax != 0.0) {
        kBcol = (jA - d_j) + 4;
        for (n = jA; n <= kBcol; n++) {
          b_A[n - 1] += b_A[((jj + n) - jA) + 1] * -smax;
        }
      }

      jA += 6;
    }
  }

  for (jj = 0; jj < 6; jj++) {
    jBcol = 15 * jj;
    jA = 6 * jj;
    for (n = 0; n < jj; n++) {
      kBcol = 15 * n;
      smax = b_A[n + jA];
      if (smax != 0.0) {
        for (b_jBcol = 0; b_jBcol < 15; b_jBcol++) {
          d_j = b_jBcol + jBcol;
          A[d_j] -= A[b_jBcol + kBcol] * smax;
        }
      }
    }

    smax = 1.0 / b_A[jj + jA];
    for (n = 0; n < 15; n++) {
      d_j = n + jBcol;
      A[d_j] *= smax;
    }
  }

  for (jj = 5; jj >= 0; jj--) {
    b_jBcol = 15 * jj;
    jA = 6 * jj - 1;
    for (jBcol = jj + 2; jBcol < 7; jBcol++) {
      kBcol = (jBcol - 1) * 15;
      smax = b_A[jBcol + jA];
      if (smax != 0.0) {
        for (n = 0; n < 15; n++) {
          d_j = n + b_jBcol;
          A[d_j] -= A[n + kBcol] * smax;
        }
      }
    }
  }

  for (jj = 4; jj >= 0; jj--) {
    int8_T ipiv_0;
    ipiv_0 = ipiv[jj];
    if (jj + 1 != ipiv_0) {
      for (jBcol = 0; jBcol < 15; jBcol++) {
        n = 15 * jj + jBcol;
        smax = A[n];
        d_j = (ipiv_0 - 1) * 15 + jBcol;
        A[n] = A[d_j];
        A[d_j] = smax;
      }
    }
  }
}

/* Function for MATLAB Function: '<Root>/EKF_Update' */
static boolean_T Navigation_all(const boolean_T x[4])
{
  int32_T k;
  boolean_T exitg1;
  boolean_T y;
  y = true;
  k = 0;
  exitg1 = false;
  while ((!exitg1) && (k < 4)) {
    if (!x[k]) {
      y = false;
      exitg1 = true;
    } else {
      k++;
    }
  }

  return y;
}

/* Function for MATLAB Function: '<Root>/EKF_Update' */
static boolean_T Navigation_all_l(const boolean_T x[3])
{
  int32_T k;
  boolean_T exitg1;
  boolean_T y;
  y = true;
  k = 0;
  exitg1 = false;
  while ((!exitg1) && (k < 3)) {
    if (!x[k]) {
      y = false;
      exitg1 = true;
    } else {
      k++;
    }
  }

  return y;
}

real_T rt_atan2d_snf(real_T u0, real_T u1)
{
  real_T y;
  if (rtIsNaN(u0) || rtIsNaN(u1)) {
    y = (rtNaN);
  } else if (rtIsInf(u0) && rtIsInf(u1)) {
    int32_T tmp;
    int32_T tmp_0;
    if (u0 > 0.0) {
      tmp = 1;
    } else {
      tmp = -1;
    }

    if (u1 > 0.0) {
      tmp_0 = 1;
    } else {
      tmp_0 = -1;
    }

    y = atan2(tmp, tmp_0);
  } else if (u1 == 0.0) {
    if (u0 > 0.0) {
      y = RT_PI / 2.0;
    } else if (u0 < 0.0) {
      y = -(RT_PI / 2.0);
    } else {
      y = 0.0;
    }
  } else {
    y = atan2(u0, u1);
  }

  return y;
}

/* Model step function */
void Navigation_step(void)
{
  real_T Fdt[225];
  real_T Fdt_0[225];
  real_T P_pred[225];
  real_T Phi[225];
  real_T Phi_0[225];
  real_T Phi_1[225];
  real_T Fdt_1[90];
  real_T H[90];
  real_T H_0[90];
  real_T K[90];
  real_T R[36];
  real_T S[36];
  real_T tmp_1[36];
  real_T dx[15];
  real_T Cbn[9];
  real_T H_tmp[9];
  real_T z[6];
  real_T dq[4];
  real_T q[4];
  real_T q_mid[4];
  real_T q_new[4];
  real_T q_new_0[4];
  real_T a_lg[3];
  real_T f_b[3];
  real_T omega_b[3];
  real_T theta[3];
  real_T cr;
  real_T cy;
  real_T dt;
  real_T dt_meas;
  real_T q_tmp;
  real_T q_tmp_0;
  real_T sp;
  real_T sy;
  real_T w;
  int32_T Fdt_tmp;
  int32_T Fdt_tmp_0;
  int32_T b_k;
  int32_T c_k;
  int32_T idx;
  real32_T rtb_state_out_gnss_age_s;
  real32_T rtb_state_out_imu_blend_w;
  uint16_T rtb_state_out_descent_cnt;
  uint16_T rtb_state_out_o_align_cnt;
  uint16_T rtb_state_out_reject_cnt;
  int8_T b_I[225];
  uint8_T rtb_state_out_gnss_only;
  uint8_T rtb_state_out_init_done;
  uint8_T rtb_state_out_origin_set;
  boolean_T tmp[4];
  boolean_T tmp_0[3];
  boolean_T hg_ok;
  boolean_T lg_ok;
  boolean_T use_pos;
  static const int8_T c_a[9] = { 1, 0, 0, 0, 1, 0, 0, 0, 1 };

  static const real_T c[9] = { 1.0E+12, 0.0, 0.0, 0.0, 1.0E+12, 0.0, 0.0, 0.0,
    1.0E+12 };

  real_T cy_tmp;
  real_T w_tmp;
  boolean_T exitg1;
  boolean_T guard1;

  /* MATLAB Function: '<Root>/INS_Mechanization' incorporates:
   *  BusCreator generated from: '<Root>/INS_Mechanization'
   *  Inport: '<Root>/NavIn'
   *  Memory: '<Root>/NavState_Memory'
   * */
  dt = 0.01;
  if (Navigation_U.NavIn.imu_lowg.data_valid == 1) {
    dt_meas = Navigation_U.NavIn.imu_lowg.timestamp_s -
      Navigation_DW.NavState_Memory_PreviousInput.timestamp_s;
    if ((dt_meas > 0.005) && (dt_meas < 0.02)) {
      dt = dt_meas;
    }
  }

  a_lg[0] = Navigation_U.NavIn.imu_lowg.accel_mps2[0];
  a_lg[1] = Navigation_U.NavIn.imu_lowg.accel_mps2[1];
  a_lg[2] = Navigation_U.NavIn.imu_lowg.accel_mps2[2];
  lg_ok = (Navigation_U.NavIn.imu_lowg.data_valid == 1);
  hg_ok = (Navigation_U.NavIn.imu_highg.data_valid == 1);
  w = fmin(fmax((Navigation_norm(a_lg) / 9.80665 - 12.0) / 4.0, 0.0), 1.0);
  theta[0] = fabs(Navigation_U.NavIn.imu_lowg.accel_mps2[0]);
  theta[1] = fabs(Navigation_U.NavIn.imu_lowg.accel_mps2[1]);
  theta[2] = fabs(Navigation_U.NavIn.imu_lowg.accel_mps2[2]);
  if (!rtIsNaN(theta[0])) {
    idx = 1;
  } else {
    idx = 0;
    b_k = 2;
    exitg1 = false;
    while ((!exitg1) && (b_k < 4)) {
      if (!rtIsNaN(theta[b_k - 1])) {
        idx = b_k;
        exitg1 = true;
      } else {
        b_k++;
      }
    }
  }

  if (idx == 0) {
    dt_meas = theta[0];
  } else {
    dt_meas = theta[idx - 1];
    for (c_k = idx + 1; c_k < 4; c_k++) {
      cy = theta[c_k - 1];
      if (dt_meas < cy) {
        dt_meas = cy;
      }
    }
  }

  if (dt_meas >= 153.768272) {
    w = 1.0;
  }

  use_pos = !lg_ok;
  if (use_pos && hg_ok) {
    w = 1.0;
  } else if (lg_ok && (!hg_ok)) {
    w = 0.0;
  }

  if (lg_ok || hg_ok) {
    a_lg[0] = (1.0 - w) * Navigation_U.NavIn.imu_lowg.accel_mps2[0] + w *
      Navigation_U.NavIn.imu_highg.accel_mps2[0];
    a_lg[1] = (1.0 - w) * Navigation_U.NavIn.imu_lowg.accel_mps2[1] + w *
      Navigation_U.NavIn.imu_highg.accel_mps2[1];
    a_lg[2] = (1.0 - w) * Navigation_U.NavIn.imu_lowg.accel_mps2[2] + w *
      Navigation_U.NavIn.imu_highg.accel_mps2[2];
  } else {
    a_lg[0] = Navigation_DW.NavState_Memory_PreviousInput.accel_body[0];
    a_lg[1] = Navigation_DW.NavState_Memory_PreviousInput.accel_body[1];
    a_lg[2] = Navigation_DW.NavState_Memory_PreviousInput.accel_body[2];
    w = Navigation_DW.NavState_Memory_PreviousInput.imu_blend_w;
  }

  f_b[0] = a_lg[0] - (1.0 - w) *
    Navigation_DW.NavState_Memory_PreviousInput.accel_bias[0];
  if (lg_ok) {
    omega_b[0] = (real_T)Navigation_U.NavIn.imu_lowg.gyro_rps[0] -
      Navigation_DW.NavState_Memory_PreviousInput.gyro_bias[0];
  } else {
    omega_b[0] = Navigation_DW.NavState_Memory_PreviousInput.gyro_body[0];
  }

  f_b[1] = a_lg[1] - (1.0 - w) *
    Navigation_DW.NavState_Memory_PreviousInput.accel_bias[1];
  if (lg_ok) {
    omega_b[1] = (real_T)Navigation_U.NavIn.imu_lowg.gyro_rps[1] -
      Navigation_DW.NavState_Memory_PreviousInput.gyro_bias[1];
  } else {
    omega_b[1] = Navigation_DW.NavState_Memory_PreviousInput.gyro_body[1];
  }

  f_b[2] = a_lg[2] - (1.0 - w) *
    Navigation_DW.NavState_Memory_PreviousInput.accel_bias[2];
  if (lg_ok) {
    omega_b[2] = (real_T)Navigation_U.NavIn.imu_lowg.gyro_rps[2] -
      Navigation_DW.NavState_Memory_PreviousInput.gyro_bias[2];
  } else {
    omega_b[2] = Navigation_DW.NavState_Memory_PreviousInput.gyro_body[2];
  }

  guard1 = false;
  if (Navigation_DW.NavState_Memory_PreviousInput.init_done == 0) {
    rtb_state_out_origin_set =
      Navigation_DW.NavState_Memory_PreviousInput.origin_set;
    rtb_state_out_reject_cnt =
      Navigation_DW.NavState_Memory_PreviousInput.reject_cnt;
    rtb_state_out_gnss_only =
      Navigation_DW.NavState_Memory_PreviousInput.gnss_only;
    rtb_state_out_descent_cnt =
      Navigation_DW.NavState_Memory_PreviousInput.descent_cnt;
    rtb_state_out_init_done = 0U;
    dt_meas = Navigation_DW.NavState_Memory_PreviousInput.timestamp_s + dt;
    rtb_state_out_imu_blend_w = (real32_T)w;
    Navigation_Y.NavOut.accel_body[0] = (real32_T)a_lg[0];
    Navigation_Y.NavOut.gyro_body[0] = (real32_T)omega_b[0];
    Navigation_Y.NavOut.accel_ned[0] = 0.0F;
    Navigation_Y.NavOut.accel_body[1] = (real32_T)a_lg[1];
    Navigation_Y.NavOut.gyro_body[1] = (real32_T)omega_b[1];
    Navigation_Y.NavOut.accel_ned[1] = 0.0F;
    Navigation_Y.NavOut.accel_body[2] = (real32_T)a_lg[2];
    Navigation_Y.NavOut.gyro_body[2] = (real32_T)omega_b[2];
    Navigation_Y.NavOut.accel_ned[2] = 0.0F;
    rtb_state_out_gnss_age_s = (real32_T)fmin
      (Navigation_DW.NavState_Memory_PreviousInput.gnss_age_s + dt, 10000.0);
    if (Navigation_norm(omega_b) > 0.069813170079773182) {
      lg_ok = true;
    } else {
      lg_ok = (fabs(Navigation_norm(a_lg) - 9.80665) > 1.5);
    }

    if (lg_ok || use_pos) {
      /* MATLAB Function: '<Root>/EKF_Update' */
      rtb_state_out_o_align_cnt = 0U;
      Navigation_DW.NavState_Memory_PreviousInput.acc_sum[0] = 0.0F;
      Navigation_DW.NavState_Memory_PreviousInput.gyr_sum[0] = 0.0F;
      Navigation_DW.NavState_Memory_PreviousInput.acc_sum[1] = 0.0F;
      Navigation_DW.NavState_Memory_PreviousInput.gyr_sum[1] = 0.0F;
      Navigation_DW.NavState_Memory_PreviousInput.acc_sum[2] = 0.0F;
      Navigation_DW.NavState_Memory_PreviousInput.gyr_sum[2] = 0.0F;

      /* MATLAB Function: '<Root>/EKF_Prediction' */
      memcpy(&Navigation_DW.NavState_Memory_PreviousInput.P[0],
             &Navigation_ConstP.pooled1.P0[0], 225U * sizeof(real32_T));
    } else {
      a_lg[0] += Navigation_DW.NavState_Memory_PreviousInput.acc_sum[0];
      omega_b[0] += Navigation_DW.NavState_Memory_PreviousInput.gyr_sum[0];
      a_lg[1] += Navigation_DW.NavState_Memory_PreviousInput.acc_sum[1];
      omega_b[1] += Navigation_DW.NavState_Memory_PreviousInput.gyr_sum[1];
      a_lg[2] += Navigation_DW.NavState_Memory_PreviousInput.acc_sum[2];
      omega_b[2] += Navigation_DW.NavState_Memory_PreviousInput.gyr_sum[2];
      if (Navigation_DW.NavState_Memory_PreviousInput.align_cnt + 1 >= 200) {
        a_lg[0] /= (real_T)Navigation_DW.NavState_Memory_PreviousInput.align_cnt
          + 1.0;
        a_lg[1] /= (real_T)Navigation_DW.NavState_Memory_PreviousInput.align_cnt
          + 1.0;
        a_lg[2] /= (real_T)Navigation_DW.NavState_Memory_PreviousInput.align_cnt
          + 1.0;
        dt = 0.5 * nav_tun.init_euler_rad[0];
        cr = cos(dt);
        dt = sin(dt);
        cy = 0.5 * nav_tun.init_euler_rad[1];
        w = cos(cy);
        sp = sin(cy);
        sy = 0.5 * nav_tun.init_euler_rad[2];
        cy = cos(sy);
        sy = sin(sy);
        q_tmp = cr * w;
        q_tmp_0 = dt * sp;
        q[0] = q_tmp * cy + q_tmp_0 * sy;
        cr *= sp;
        dt *= w;
        q[1] = dt * cy - cr * sy;
        q[2] = cr * cy + dt * sy;
        q[3] = q_tmp * sy - q_tmp_0 * cy;
        cr = Navigation_norm_am(q);
        q[0] /= cr;
        q[1] /= cr;
        q[2] /= cr;
        q[3] /= cr;
        cr = q[3] * q[3];
        cy = q[2] * q[2];
        Cbn[0] = -(1.0 - (cy + cr) * 2.0);
        sy = q[1] * q[2];
        q_tmp = q[0] * q[3];
        Cbn[1] = -((sy - q_tmp) * 2.0);
        q_tmp_0 = q[1] * q[3];
        sp = q[0] * q[2];
        Cbn[2] = -((q_tmp_0 + sp) * 2.0);
        Cbn[3] = -((sy + q_tmp) * 2.0);
        sy = q[1] * q[1];
        Cbn[4] = -(1.0 - (sy + cr) * 2.0);
        cr = q[2] * q[3];
        q_tmp = q[0] * q[1];
        Cbn[5] = -((cr - q_tmp) * 2.0);
        Cbn[6] = -((q_tmp_0 - sp) * 2.0);
        Cbn[7] = -((cr + q_tmp) * 2.0);
        Cbn[8] = -(1.0 - (sy + cy) * 2.0);
        theta[0] = 0.0;
        theta[1] = 0.0;
        theta[2] = 9.80665;
        cy = 0.0;
        dt = 0.0;
        w = 0.0;
        for (idx = 0; idx < 3; idx++) {
          cr = theta[idx];
          cy += Cbn[3 * idx] * cr;
          dt += Cbn[3 * idx + 1] * cr;
          w += Cbn[3 * idx + 2] * cr;
        }

        theta[2] = w;
        theta[1] = dt;
        theta[0] = cy;
        cr = Navigation_norm(a_lg);
        sy = Navigation_norm(theta);
        q_tmp = 1.0;
        if ((cr > 1.0E-6) && (sy > 1.0E-6)) {
          q_tmp = ((a_lg[0] * cy + a_lg[1] * dt) + a_lg[2] * w) / (cr * sy);
        }

        if (acos(fmin(fmax(q_tmp, -1.0), 1.0)) > 0.17453292519943295) {
          /* MATLAB Function: '<Root>/EKF_Update' */
          rtb_state_out_o_align_cnt = 0U;
          Navigation_DW.NavState_Memory_PreviousInput.acc_sum[0] = 0.0F;
          Navigation_DW.NavState_Memory_PreviousInput.gyr_sum[0] = 0.0F;
          Navigation_DW.NavState_Memory_PreviousInput.acc_sum[1] = 0.0F;
          Navigation_DW.NavState_Memory_PreviousInput.gyr_sum[1] = 0.0F;
          Navigation_DW.NavState_Memory_PreviousInput.acc_sum[2] = 0.0F;
          Navigation_DW.NavState_Memory_PreviousInput.gyr_sum[2] = 0.0F;

          /* MATLAB Function: '<Root>/EKF_Prediction' */
          memcpy(&Navigation_DW.NavState_Memory_PreviousInput.P[0],
                 &Navigation_ConstP.pooled1.P0[0], 225U * sizeof(real32_T));
        } else {
          Navigation_DW.NavState_Memory_PreviousInput.quat_nb[0] = (real32_T)q[0];
          Navigation_DW.NavState_Memory_PreviousInput.quat_nb[1] = (real32_T)q[1];
          Navigation_DW.NavState_Memory_PreviousInput.quat_nb[2] = (real32_T)q[2];
          Navigation_DW.NavState_Memory_PreviousInput.quat_nb[3] = (real32_T)q[3];
          Navigation_DW.NavState_Memory_PreviousInput.gyro_bias[0] = (real32_T)
            (omega_b[0] / ((real_T)
                           Navigation_DW.NavState_Memory_PreviousInput.align_cnt
                           + 1.0));
          Navigation_DW.NavState_Memory_PreviousInput.accel_bias[0] = (real32_T)
            (a_lg[0] - cy);
          Navigation_DW.NavState_Memory_PreviousInput.vel_ned[0] = 0.0F;
          Navigation_DW.NavState_Memory_PreviousInput.gyro_bias[1] = (real32_T)
            (omega_b[1] / ((real_T)
                           Navigation_DW.NavState_Memory_PreviousInput.align_cnt
                           + 1.0));
          Navigation_DW.NavState_Memory_PreviousInput.accel_bias[1] = (real32_T)
            (a_lg[1] - dt);
          Navigation_DW.NavState_Memory_PreviousInput.vel_ned[1] = 0.0F;
          Navigation_DW.NavState_Memory_PreviousInput.gyro_bias[2] = (real32_T)
            (omega_b[2] / ((real_T)
                           Navigation_DW.NavState_Memory_PreviousInput.align_cnt
                           + 1.0));
          Navigation_DW.NavState_Memory_PreviousInput.accel_bias[2] = (real32_T)
            (a_lg[2] - w);
          Navigation_DW.NavState_Memory_PreviousInput.vel_ned[2] = 0.0F;
          memcpy(&Navigation_DW.NavState_Memory_PreviousInput.P[0],
                 &Navigation_ConstP.pooled1.P0[0], 225U * sizeof(real32_T));
          rtb_state_out_init_done = 1U;

          /* MATLAB Function: '<Root>/EKF_Update' */
          rtb_state_out_o_align_cnt = 0U;
          Navigation_DW.NavState_Memory_PreviousInput.acc_sum[0] = 0.0F;
          Navigation_DW.NavState_Memory_PreviousInput.gyr_sum[0] = 0.0F;
          Navigation_DW.NavState_Memory_PreviousInput.acc_sum[1] = 0.0F;
          Navigation_DW.NavState_Memory_PreviousInput.gyr_sum[1] = 0.0F;
          Navigation_DW.NavState_Memory_PreviousInput.acc_sum[2] = 0.0F;
          Navigation_DW.NavState_Memory_PreviousInput.gyr_sum[2] = 0.0F;
          guard1 = true;
        }
      } else {
        if (Navigation_DW.NavState_Memory_PreviousInput.align_cnt + 1 < 65536) {
          /* MATLAB Function: '<Root>/EKF_Update' */
          rtb_state_out_o_align_cnt = (uint16_T)
            (Navigation_DW.NavState_Memory_PreviousInput.align_cnt + 1);
        } else {
          /* MATLAB Function: '<Root>/EKF_Update' */
          rtb_state_out_o_align_cnt = MAX_uint16_T;
        }

        /* MATLAB Function: '<Root>/EKF_Update' */
        Navigation_DW.NavState_Memory_PreviousInput.acc_sum[0] = (real32_T)a_lg
          [0];
        Navigation_DW.NavState_Memory_PreviousInput.gyr_sum[0] = (real32_T)
          omega_b[0];
        Navigation_DW.NavState_Memory_PreviousInput.acc_sum[1] = (real32_T)a_lg
          [1];
        Navigation_DW.NavState_Memory_PreviousInput.gyr_sum[1] = (real32_T)
          omega_b[1];
        Navigation_DW.NavState_Memory_PreviousInput.acc_sum[2] = (real32_T)a_lg
          [2];
        Navigation_DW.NavState_Memory_PreviousInput.gyr_sum[2] = (real32_T)
          omega_b[2];

        /* MATLAB Function: '<Root>/EKF_Prediction' */
        memcpy(&Navigation_DW.NavState_Memory_PreviousInput.P[0],
               &Navigation_ConstP.pooled1.P0[0], 225U * sizeof(real32_T));
      }
    }
  } else {
    if (Navigation_DW.NavState_Memory_PreviousInput.gnss_only == 1) {
      theta[0] = omega_b[0] * dt;
      theta[1] = omega_b[1] * dt;
      theta[2] = omega_b[2] * dt;
      dt_meas = Navigation_norm(theta);
      if (dt_meas > 1.0E-12) {
        cr = 0.5 * dt_meas;
        cy = sin(cr);
        dq[0] = cos(cr);
        dq[1] = theta[0] / dt_meas * cy;
        dq[2] = theta[1] / dt_meas * cy;
        dq[3] = theta[2] / dt_meas * cy;
      } else {
        dq[0] = 1.0;
        dq[1] = 0.5 * theta[0];
        dq[2] = 0.5 * theta[1];
        dq[3] = 0.5 * theta[2];
      }

      q_new[0] = ((Navigation_DW.NavState_Memory_PreviousInput.quat_nb[0] * dq[0]
                   - Navigation_DW.NavState_Memory_PreviousInput.quat_nb[1] *
                   dq[1]) - Navigation_DW.NavState_Memory_PreviousInput.quat_nb
                  [2] * dq[2]) -
        Navigation_DW.NavState_Memory_PreviousInput.quat_nb[3] * dq[3];
      q_new[1] = ((Navigation_DW.NavState_Memory_PreviousInput.quat_nb[0] * dq[1]
                   + dq[0] *
                   Navigation_DW.NavState_Memory_PreviousInput.quat_nb[1]) +
                  Navigation_DW.NavState_Memory_PreviousInput.quat_nb[2] * dq[3])
        - dq[2] * Navigation_DW.NavState_Memory_PreviousInput.quat_nb[3];
      q_new[2] = ((Navigation_DW.NavState_Memory_PreviousInput.quat_nb[0] * dq[2]
                   - Navigation_DW.NavState_Memory_PreviousInput.quat_nb[1] *
                   dq[3]) + dq[0] *
                  Navigation_DW.NavState_Memory_PreviousInput.quat_nb[2]) + dq[1]
        * Navigation_DW.NavState_Memory_PreviousInput.quat_nb[3];
      q_new[3] = ((Navigation_DW.NavState_Memory_PreviousInput.quat_nb[0] * dq[3]
                   + Navigation_DW.NavState_Memory_PreviousInput.quat_nb[1] *
                   dq[2]) - dq[1] *
                  Navigation_DW.NavState_Memory_PreviousInput.quat_nb[2]) + dq[0]
        * Navigation_DW.NavState_Memory_PreviousInput.quat_nb[3];
      rtb_state_out_origin_set =
        Navigation_DW.NavState_Memory_PreviousInput.origin_set;

      /* MATLAB Function: '<Root>/EKF_Update' */
      rtb_state_out_o_align_cnt =
        Navigation_DW.NavState_Memory_PreviousInput.align_cnt;
      rtb_state_out_reject_cnt =
        Navigation_DW.NavState_Memory_PreviousInput.reject_cnt;
      rtb_state_out_gnss_only = 1U;
      rtb_state_out_descent_cnt =
        Navigation_DW.NavState_Memory_PreviousInput.descent_cnt;
      rtb_state_out_init_done =
        Navigation_DW.NavState_Memory_PreviousInput.init_done;
      cr = Navigation_norm_am(q_new);
      Navigation_DW.NavState_Memory_PreviousInput.quat_nb[0] = (real32_T)(q_new
        [0] / cr);
      Navigation_DW.NavState_Memory_PreviousInput.quat_nb[1] = (real32_T)(q_new
        [1] / cr);
      Navigation_DW.NavState_Memory_PreviousInput.quat_nb[2] = (real32_T)(q_new
        [2] / cr);
      Navigation_DW.NavState_Memory_PreviousInput.quat_nb[3] = (real32_T)(q_new
        [3] / cr);
      Navigation_DW.NavState_Memory_PreviousInput.pos_ned[0] = (real32_T)
        (Navigation_DW.NavState_Memory_PreviousInput.vel_ned[0] * dt +
         Navigation_DW.NavState_Memory_PreviousInput.pos_ned[0]);
      Navigation_Y.NavOut.accel_ned[0] = 0.0F;
      Navigation_Y.NavOut.accel_body[0] = (real32_T)f_b[0];
      Navigation_Y.NavOut.gyro_body[0] = (real32_T)omega_b[0];
      Navigation_DW.NavState_Memory_PreviousInput.pos_ned[1] = (real32_T)
        (Navigation_DW.NavState_Memory_PreviousInput.vel_ned[1] * dt +
         Navigation_DW.NavState_Memory_PreviousInput.pos_ned[1]);
      Navigation_Y.NavOut.accel_ned[1] = 0.0F;
      Navigation_Y.NavOut.accel_body[1] = (real32_T)f_b[1];
      Navigation_Y.NavOut.gyro_body[1] = (real32_T)omega_b[1];
      Navigation_DW.NavState_Memory_PreviousInput.pos_ned[2] = (real32_T)
        (Navigation_DW.NavState_Memory_PreviousInput.vel_ned[2] * dt +
         Navigation_DW.NavState_Memory_PreviousInput.pos_ned[2]);
      Navigation_Y.NavOut.accel_ned[2] = 0.0F;
      Navigation_Y.NavOut.accel_body[2] = (real32_T)f_b[2];
      Navigation_Y.NavOut.gyro_body[2] = (real32_T)omega_b[2];
      rtb_state_out_imu_blend_w = (real32_T)w;
      dt_meas = Navigation_DW.NavState_Memory_PreviousInput.timestamp_s + dt;
      rtb_state_out_gnss_age_s = (real32_T)fmin
        (Navigation_DW.NavState_Memory_PreviousInput.gnss_age_s + dt, 10000.0);
    } else {
      theta[0] = omega_b[0] * dt;
      theta[1] = omega_b[1] * dt;
      theta[2] = omega_b[2] * dt;
      dt_meas = Navigation_norm(theta);
      if (dt_meas > 1.0E-12) {
        cy = 0.5 * dt_meas;
        cr = sin(cy);
        dq[0] = cos(cy);
        dq[1] = theta[0] / dt_meas * cr;
        dq[2] = theta[1] / dt_meas * cr;
        dq[3] = theta[2] / dt_meas * cr;
      } else {
        dq[0] = 1.0;
        dq[1] = 0.5 * theta[0];
        dq[2] = 0.5 * theta[1];
        dq[3] = 0.5 * theta[2];
      }

      q_new[0] = ((Navigation_DW.NavState_Memory_PreviousInput.quat_nb[0] * dq[0]
                   - Navigation_DW.NavState_Memory_PreviousInput.quat_nb[1] *
                   dq[1]) - Navigation_DW.NavState_Memory_PreviousInput.quat_nb
                  [2] * dq[2]) -
        Navigation_DW.NavState_Memory_PreviousInput.quat_nb[3] * dq[3];
      q_new[1] = ((Navigation_DW.NavState_Memory_PreviousInput.quat_nb[0] * dq[1]
                   + dq[0] *
                   Navigation_DW.NavState_Memory_PreviousInput.quat_nb[1]) +
                  Navigation_DW.NavState_Memory_PreviousInput.quat_nb[2] * dq[3])
        - dq[2] * Navigation_DW.NavState_Memory_PreviousInput.quat_nb[3];
      q_new[2] = ((Navigation_DW.NavState_Memory_PreviousInput.quat_nb[0] * dq[2]
                   - Navigation_DW.NavState_Memory_PreviousInput.quat_nb[1] *
                   dq[3]) + dq[0] *
                  Navigation_DW.NavState_Memory_PreviousInput.quat_nb[2]) + dq[1]
        * Navigation_DW.NavState_Memory_PreviousInput.quat_nb[3];
      q_new[3] = ((Navigation_DW.NavState_Memory_PreviousInput.quat_nb[0] * dq[3]
                   + Navigation_DW.NavState_Memory_PreviousInput.quat_nb[1] *
                   dq[2]) - dq[1] *
                  Navigation_DW.NavState_Memory_PreviousInput.quat_nb[2]) + dq[0]
        * Navigation_DW.NavState_Memory_PreviousInput.quat_nb[3];
      cr = Navigation_norm_am(q_new);
      q_new[0] /= cr;
      q_new[1] /= cr;
      q_new[2] /= cr;
      q_new[3] /= cr;
      if (dt_meas > 1.0E-12) {
        cr = 0.25 * dt_meas;
        cy = sin(cr);
        dq[0] = cos(cr);
        dq[1] = theta[0] / dt_meas * cy;
        dq[2] = theta[1] / dt_meas * cy;
        dq[3] = theta[2] / dt_meas * cy;
      } else {
        dq[0] = 1.0;
        dq[1] = 0.25 * theta[0];
        dq[2] = 0.25 * theta[1];
        dq[3] = 0.25 * theta[2];
      }

      q_mid[0] = ((Navigation_DW.NavState_Memory_PreviousInput.quat_nb[0] * dq[0]
                   - Navigation_DW.NavState_Memory_PreviousInput.quat_nb[1] *
                   dq[1]) - Navigation_DW.NavState_Memory_PreviousInput.quat_nb
                  [2] * dq[2]) -
        Navigation_DW.NavState_Memory_PreviousInput.quat_nb[3] * dq[3];
      q_mid[1] = ((Navigation_DW.NavState_Memory_PreviousInput.quat_nb[0] * dq[1]
                   + dq[0] *
                   Navigation_DW.NavState_Memory_PreviousInput.quat_nb[1]) +
                  Navigation_DW.NavState_Memory_PreviousInput.quat_nb[2] * dq[3])
        - dq[2] * Navigation_DW.NavState_Memory_PreviousInput.quat_nb[3];
      q_mid[2] = ((Navigation_DW.NavState_Memory_PreviousInput.quat_nb[0] * dq[2]
                   - Navigation_DW.NavState_Memory_PreviousInput.quat_nb[1] *
                   dq[3]) + dq[0] *
                  Navigation_DW.NavState_Memory_PreviousInput.quat_nb[2]) + dq[1]
        * Navigation_DW.NavState_Memory_PreviousInput.quat_nb[3];
      q_mid[3] = ((Navigation_DW.NavState_Memory_PreviousInput.quat_nb[0] * dq[3]
                   + Navigation_DW.NavState_Memory_PreviousInput.quat_nb[1] *
                   dq[2]) - dq[1] *
                  Navigation_DW.NavState_Memory_PreviousInput.quat_nb[2]) + dq[0]
        * Navigation_DW.NavState_Memory_PreviousInput.quat_nb[3];
      cr = Navigation_norm_am(q_mid);
      q[0] = q_mid[0] / cr;
      q[1] = q_mid[1] / cr;
      q[2] = q_mid[2] / cr;
      q[3] = q_mid[3] / cr;
      cr = q[3] * q[3];
      cy = q[2] * q[2];
      Cbn[0] = 1.0 - (cy + cr) * 2.0;
      sy = q[1] * q[2];
      q_tmp = q[0] * q[3];
      Cbn[3] = (sy - q_tmp) * 2.0;
      q_tmp_0 = q[1] * q[3];
      sp = q[0] * q[2];
      Cbn[6] = (q_tmp_0 + sp) * 2.0;
      Cbn[1] = (sy + q_tmp) * 2.0;
      sy = q[1] * q[1];
      Cbn[4] = 1.0 - (sy + cr) * 2.0;
      cr = q[2] * q[3];
      q_tmp = q[0] * q[1];
      Cbn[7] = (cr - q_tmp) * 2.0;
      Cbn[2] = (q_tmp_0 - sp) * 2.0;
      Cbn[5] = (cr + q_tmp) * 2.0;
      Cbn[8] = 1.0 - (sy + cy) * 2.0;
      dt_meas = 0.0;
      cy = 0.0;
      sy = 0.0;
      for (idx = 0; idx < 3; idx++) {
        cr = f_b[idx];
        dt_meas += Cbn[3 * idx] * cr;
        cy += Cbn[3 * idx + 1] * cr;
        sy += Cbn[3 * idx + 2] * cr;
      }

      rtb_state_out_origin_set =
        Navigation_DW.NavState_Memory_PreviousInput.origin_set;

      /* MATLAB Function: '<Root>/EKF_Update' */
      rtb_state_out_o_align_cnt =
        Navigation_DW.NavState_Memory_PreviousInput.align_cnt;
      rtb_state_out_reject_cnt =
        Navigation_DW.NavState_Memory_PreviousInput.reject_cnt;
      rtb_state_out_gnss_only =
        Navigation_DW.NavState_Memory_PreviousInput.gnss_only;
      rtb_state_out_descent_cnt =
        Navigation_DW.NavState_Memory_PreviousInput.descent_cnt;
      rtb_state_out_init_done =
        Navigation_DW.NavState_Memory_PreviousInput.init_done;
      Navigation_DW.NavState_Memory_PreviousInput.quat_nb[0] = (real32_T)q_new[0];
      Navigation_DW.NavState_Memory_PreviousInput.quat_nb[1] = (real32_T)q_new[1];
      Navigation_DW.NavState_Memory_PreviousInput.quat_nb[2] = (real32_T)q_new[2];
      Navigation_DW.NavState_Memory_PreviousInput.quat_nb[3] = (real32_T)q_new[3];
      Navigation_DW.NavState_Memory_PreviousInput.pos_ned[0] = (real32_T)(0.5 *
        dt_meas * dt * dt +
        (Navigation_DW.NavState_Memory_PreviousInput.vel_ned[0] * dt +
         Navigation_DW.NavState_Memory_PreviousInput.pos_ned[0]));
      Navigation_DW.NavState_Memory_PreviousInput.vel_ned[0] = (real32_T)
        (dt_meas * dt + Navigation_DW.NavState_Memory_PreviousInput.vel_ned[0]);
      Navigation_Y.NavOut.accel_ned[0] = (real32_T)dt_meas;
      Navigation_Y.NavOut.accel_body[0] = (real32_T)f_b[0];
      Navigation_Y.NavOut.gyro_body[0] = (real32_T)omega_b[0];
      Navigation_DW.NavState_Memory_PreviousInput.pos_ned[1] = (real32_T)(0.5 *
        cy * dt * dt + (Navigation_DW.NavState_Memory_PreviousInput.vel_ned[1] *
                        dt +
                        Navigation_DW.NavState_Memory_PreviousInput.pos_ned[1]));
      Navigation_DW.NavState_Memory_PreviousInput.vel_ned[1] = (real32_T)(cy *
        dt + Navigation_DW.NavState_Memory_PreviousInput.vel_ned[1]);
      Navigation_Y.NavOut.accel_ned[1] = (real32_T)cy;
      Navigation_Y.NavOut.accel_body[1] = (real32_T)f_b[1];
      Navigation_Y.NavOut.gyro_body[1] = (real32_T)omega_b[1];
      Navigation_DW.NavState_Memory_PreviousInput.pos_ned[2] = (real32_T)((sy +
        9.81) * 0.5 * dt * dt +
        (Navigation_DW.NavState_Memory_PreviousInput.vel_ned[2] * dt +
         Navigation_DW.NavState_Memory_PreviousInput.pos_ned[2]));
      Navigation_DW.NavState_Memory_PreviousInput.vel_ned[2] = (real32_T)((sy +
        9.81) * dt + Navigation_DW.NavState_Memory_PreviousInput.vel_ned[2]);
      Navigation_Y.NavOut.accel_ned[2] = (real32_T)(sy + 9.81);
      Navigation_Y.NavOut.accel_body[2] = (real32_T)f_b[2];
      Navigation_Y.NavOut.gyro_body[2] = (real32_T)omega_b[2];
      rtb_state_out_imu_blend_w = (real32_T)w;
      dt_meas = Navigation_DW.NavState_Memory_PreviousInput.timestamp_s + dt;
      rtb_state_out_gnss_age_s = (real32_T)fmin
        (Navigation_DW.NavState_Memory_PreviousInput.gnss_age_s + dt, 10000.0);
    }

    guard1 = true;
  }

  if (guard1) {
    /* MATLAB Function: '<Root>/EKF_Prediction' */
    if (rtb_state_out_gnss_only != 1) {
      cr = (real_T)Navigation_DW.NavState_Memory_PreviousInput.quat_nb[3] *
        Navigation_DW.NavState_Memory_PreviousInput.quat_nb[3];
      dt = (real_T)Navigation_DW.NavState_Memory_PreviousInput.quat_nb[2] *
        Navigation_DW.NavState_Memory_PreviousInput.quat_nb[2];
      Cbn[0] = 1.0 - (dt + cr) * 2.0;
      w = (real_T)Navigation_DW.NavState_Memory_PreviousInput.quat_nb[1] *
        Navigation_DW.NavState_Memory_PreviousInput.quat_nb[2];
      cy = (real_T)Navigation_DW.NavState_Memory_PreviousInput.quat_nb[0] *
        Navigation_DW.NavState_Memory_PreviousInput.quat_nb[3];
      Cbn[3] = (w - cy) * 2.0;
      sy = (real_T)Navigation_DW.NavState_Memory_PreviousInput.quat_nb[1] *
        Navigation_DW.NavState_Memory_PreviousInput.quat_nb[3];
      q_tmp = (real_T)Navigation_DW.NavState_Memory_PreviousInput.quat_nb[0] *
        Navigation_DW.NavState_Memory_PreviousInput.quat_nb[2];
      Cbn[6] = (sy + q_tmp) * 2.0;
      Cbn[1] = (w + cy) * 2.0;
      w = (real_T)Navigation_DW.NavState_Memory_PreviousInput.quat_nb[1] *
        Navigation_DW.NavState_Memory_PreviousInput.quat_nb[1];
      Cbn[4] = 1.0 - (w + cr) * 2.0;
      cr = (real_T)Navigation_DW.NavState_Memory_PreviousInput.quat_nb[2] *
        Navigation_DW.NavState_Memory_PreviousInput.quat_nb[3];
      cy = (real_T)Navigation_DW.NavState_Memory_PreviousInput.quat_nb[0] *
        Navigation_DW.NavState_Memory_PreviousInput.quat_nb[1];
      Cbn[7] = (cr - cy) * 2.0;
      Cbn[2] = (sy - q_tmp) * 2.0;
      Cbn[5] = (cr + cy) * 2.0;
      Cbn[8] = 1.0 - (w + dt) * 2.0;
      dt = 0.0;
      w = 0.0;
      cy = 0.0;
      for (idx = 0; idx < 3; idx++) {
        cr = Navigation_Y.NavOut.accel_body[idx];
        dt += Cbn[3 * idx] * cr;
        w += Cbn[3 * idx + 1] * cr;
        cy += Cbn[3 * idx + 2] * cr;
      }

      memset(&Fdt[0], 0, 225U * sizeof(real_T));
      Fdt[3] = -0.0;
      Fdt[18] = cy;
      Fdt[33] = -w;
      Fdt[4] = -cy;
      Fdt[19] = -0.0;
      Fdt[34] = dt;
      Fdt[5] = w;
      Fdt[20] = -dt;
      Fdt[35] = -0.0;
      for (idx = 0; idx < 3; idx++) {
        cr = Cbn[3 * idx];
        Fdt_tmp = (idx + 12) * 15;
        Fdt[Fdt_tmp] = -cr;
        c_k = (idx + 9) * 15;
        Fdt[c_k + 3] = cr;
        cr = Cbn[3 * idx + 1];
        Fdt[Fdt_tmp + 1] = -cr;
        Fdt[c_k + 4] = cr;
        cr = Cbn[3 * idx + 2];
        Fdt[Fdt_tmp + 2] = -cr;
        Fdt[c_k + 5] = cr;
      }

      memset(&Cbn[0], 0, 9U * sizeof(real_T));
      Cbn[0] = 1.0;
      Cbn[4] = 1.0;
      Cbn[8] = 1.0;
      for (idx = 0; idx < 3; idx++) {
        Fdt_tmp = (idx + 3) * 15;
        Fdt[Fdt_tmp + 6] = Cbn[3 * idx];
        Fdt[Fdt_tmp + 7] = Cbn[3 * idx + 1];
        Fdt[Fdt_tmp + 8] = Cbn[3 * idx + 2];
      }

      for (idx = 0; idx < 225; idx++) {
        Fdt[idx] *= 0.01;
        P_pred[idx] = 0.0;
      }

      for (b_k = 0; b_k < 15; b_k++) {
        P_pred[b_k + 15 * b_k] = 1.0;
        memset(&Fdt_0[b_k * 15], 0, 15U * sizeof(real_T));
        for (idx = 0; idx < 15; idx++) {
          cr = Fdt[15 * b_k + idx];
          for (c_k = 0; c_k < 15; c_k++) {
            Fdt_tmp = 15 * b_k + c_k;
            Fdt_0[Fdt_tmp] += Fdt[15 * idx + c_k] * cr;
          }
        }
      }

      theta[0] = Navigation_Y.NavOut.accel_body[0];
      theta[1] = Navigation_Y.NavOut.accel_body[1];
      theta[2] = Navigation_Y.NavOut.accel_body[2];
      dt = ((1.0 - rtb_state_out_imu_blend_w) * 0.02 + rtb_state_out_imu_blend_w
            * 0.35) + 0.01 * Navigation_norm(theta);
      theta[0] = Navigation_Y.NavOut.gyro_body[0];
      theta[1] = Navigation_Y.NavOut.gyro_body[1];
      theta[2] = Navigation_Y.NavOut.gyro_body[2];
      cr = 0.005 * Navigation_norm(theta) + 0.00052359877559829881;
      cr *= cr;
      cy = dt * dt;
      for (idx = 0; idx < 225; idx++) {
        Phi[idx] = (P_pred[idx] + Fdt[idx]) + 0.5 * Fdt_0[idx];
        Fdt[idx] = 0.0;
      }

      for (idx = 0; idx < 9; idx++) {
        Cbn[idx] = c_a[idx];
      }

      for (idx = 0; idx < 3; idx++) {
        b_k = (int32_T)Cbn[3 * idx];
        Fdt[15 * idx] = (real_T)b_k * cr;
        Fdt_tmp = (idx + 3) * 15;
        Fdt[Fdt_tmp + 3] = (real_T)b_k * cy;
        c_k = (idx + 9) * 15;
        Fdt[c_k + 9] = (real_T)b_k * 1.0E-8;
        Fdt_tmp_0 = (idx + 12) * 15;
        Fdt[Fdt_tmp_0 + 12] = (real_T)b_k * 1.0E-12;
        b_k = (int32_T)Cbn[3 * idx + 1];
        Fdt[15 * idx + 1] = (real_T)b_k * cr;
        Fdt[Fdt_tmp + 4] = (real_T)b_k * cy;
        Fdt[c_k + 10] = (real_T)b_k * 1.0E-8;
        Fdt[Fdt_tmp_0 + 13] = (real_T)b_k * 1.0E-12;
        b_k = (int32_T)Cbn[3 * idx + 2];
        Fdt[15 * idx + 2] = (real_T)b_k * cr;
        Fdt[Fdt_tmp + 5] = (real_T)b_k * cy;
        Fdt[c_k + 11] = (real_T)b_k * 1.0E-8;
        Fdt[Fdt_tmp_0 + 14] = (real_T)b_k * 1.0E-12;
      }

      for (idx = 0; idx < 15; idx++) {
        for (c_k = 0; c_k < 15; c_k++) {
          Fdt_tmp = 15 * idx + c_k;
          Fdt_0[Fdt_tmp] = Phi[15 * c_k + idx];
          Phi_0[Fdt_tmp] = 0.0;
        }

        for (c_k = 0; c_k < 15; c_k++) {
          b_k = 15 * idx + c_k;
          cr = Navigation_DW.NavState_Memory_PreviousInput.P[b_k];
          for (Fdt_tmp = 0; Fdt_tmp < 15; Fdt_tmp++) {
            Fdt_tmp_0 = 15 * idx + Fdt_tmp;
            Phi_0[Fdt_tmp_0] += Phi[15 * c_k + Fdt_tmp] * cr;
          }

          Phi_1[b_k] = 0.0;
        }

        for (c_k = 0; c_k < 15; c_k++) {
          cr = Fdt[15 * idx + c_k];
          for (b_k = 0; b_k < 15; b_k++) {
            Fdt_tmp_0 = 15 * idx + b_k;
            Phi_1[Fdt_tmp_0] += Phi[15 * c_k + b_k] * cr;
          }
        }
      }

      for (idx = 0; idx < 15; idx++) {
        for (c_k = 0; c_k < 15; c_k++) {
          cr = 0.0;
          cy = 0.0;
          for (b_k = 0; b_k < 15; b_k++) {
            dt = Fdt_0[15 * c_k + b_k];
            Fdt_tmp = 15 * b_k + idx;
            cr += Phi_1[Fdt_tmp] * dt;
            cy += Phi_0[Fdt_tmp] * dt;
          }

          Fdt_tmp = 15 * c_k + idx;
          P_pred[Fdt_tmp] = (Fdt[Fdt_tmp] + cr) * 0.5 * 0.01 + cy;
        }
      }

      for (idx = 0; idx < 15; idx++) {
        for (c_k = 0; c_k < 15; c_k++) {
          b_k = 15 * idx + c_k;
          Navigation_DW.NavState_Memory_PreviousInput.P[b_k] = (real32_T)
            ((P_pred[15 * c_k + idx] + P_pred[b_k]) * 0.5);
        }
      }
    }
  }

  /* End of MATLAB Function: '<Root>/INS_Mechanization' */

  /* Update for Memory: '<Root>/NavState_Memory' incorporates:
   *  MATLAB Function: '<Root>/EKF_Prediction'
   *  MATLAB Function: '<Root>/EKF_Update'
   */
  Navigation_DW.NavState_Memory_PreviousInput.reject_cnt =
    rtb_state_out_reject_cnt;
  Navigation_DW.NavState_Memory_PreviousInput.gnss_only =
    rtb_state_out_gnss_only;

  /* MATLAB Function: '<Root>/EKF_Update' incorporates:
   *  BusCreator generated from: '<Root>/EKF_Update'
   *  Inport: '<Root>/NavIn'
   *  MATLAB Function: '<Root>/EKF_Prediction'
   */
  if ((rtb_state_out_origin_set == 0) && ((Navigation_U.NavIn.gnss.data_valid ==
        1) && (Navigation_U.NavIn.gnss.fix_type >= 3))) {
    Navigation_DW.NavState_Memory_PreviousInput.origin_lla[0] =
      Navigation_U.NavIn.gnss.pos_lla[0];
    Navigation_DW.NavState_Memory_PreviousInput.origin_lla[1] =
      Navigation_U.NavIn.gnss.pos_lla[1];
    Navigation_DW.NavState_Memory_PreviousInput.origin_lla[2] =
      Navigation_U.NavIn.gnss.pos_lla[2];
    rtb_state_out_origin_set = 1U;
  }

  if ((rtb_state_out_init_done == 1) && (rtb_state_out_gnss_only == 0)) {
    if (Navigation_DW.NavState_Memory_PreviousInput.vel_ned[2] > 0.0F) {
      if (rtb_state_out_descent_cnt < 65535) {
        rtb_state_out_descent_cnt++;
      }
    } else {
      rtb_state_out_descent_cnt = 0U;
    }

    if (rtb_state_out_descent_cnt >= 50) {
      /* Update for Memory: '<Root>/NavState_Memory' */
      Navigation_DW.NavState_Memory_PreviousInput.gnss_only = 1U;
    }
  }

  if (rtb_state_out_gnss_only == 1) {
    if ((Navigation_U.NavIn.gnss.data_valid == 1) &&
        (Navigation_U.NavIn.gnss.fix_type >= 3) &&
        (Navigation_U.NavIn.gnss.num_sats >= 5) && (rtb_state_out_origin_set ==
         1)) {
      cr = sin(Navigation_DW.NavState_Memory_PreviousInput.origin_lla[0]);
      cr = 1.0 - 0.00669437999014 * cr * cr;
      dt = sqrt(cr);
      Navigation_DW.NavState_Memory_PreviousInput.pos_ned[0] = (real32_T)
        ((6.3354393272928288E+6 / (cr * dt) +
          Navigation_DW.NavState_Memory_PreviousInput.origin_lla[2]) *
         (Navigation_U.NavIn.gnss.pos_lla[0] -
          Navigation_DW.NavState_Memory_PreviousInput.origin_lla[0]));
      Navigation_DW.NavState_Memory_PreviousInput.pos_ned[1] = (real32_T)
        ((6.378137E+6 / dt +
          Navigation_DW.NavState_Memory_PreviousInput.origin_lla[2]) *
         (Navigation_U.NavIn.gnss.pos_lla[1] -
          Navigation_DW.NavState_Memory_PreviousInput.origin_lla[1]) * cos
         (Navigation_DW.NavState_Memory_PreviousInput.origin_lla[0]));
      Navigation_DW.NavState_Memory_PreviousInput.pos_ned[2] = (real32_T)
        -(Navigation_U.NavIn.gnss.pos_lla[2] -
          Navigation_DW.NavState_Memory_PreviousInput.origin_lla[2]);
      Navigation_DW.NavState_Memory_PreviousInput.vel_ned[0] =
        Navigation_U.NavIn.gnss.vel_ned[0];
      Navigation_DW.NavState_Memory_PreviousInput.vel_ned[1] =
        Navigation_U.NavIn.gnss.vel_ned[1];
      Navigation_DW.NavState_Memory_PreviousInput.vel_ned[2] =
        Navigation_U.NavIn.gnss.vel_ned[2];
      rtb_state_out_gnss_age_s = 0.0F;

      /* Update for Memory: '<Root>/NavState_Memory' incorporates:
       *  BusCreator generated from: '<Root>/EKF_Update'
       */
      Navigation_DW.NavState_Memory_PreviousInput.reject_cnt = 0U;
    }
  } else if ((Navigation_U.NavIn.gnss.data_valid == 1) &&
             (Navigation_U.NavIn.gnss.fix_type >= 3) &&
             (Navigation_U.NavIn.gnss.num_sats >= 5) &&
             (rtb_state_out_origin_set == 1) && (rtb_state_out_init_done == 1))
  {
    cr = (real_T)Navigation_DW.NavState_Memory_PreviousInput.quat_nb[3] *
      Navigation_DW.NavState_Memory_PreviousInput.quat_nb[3];
    dt = (real_T)Navigation_DW.NavState_Memory_PreviousInput.quat_nb[2] *
      Navigation_DW.NavState_Memory_PreviousInput.quat_nb[2];
    Cbn[0] = 1.0 - (dt + cr) * 2.0;
    w = (real_T)Navigation_DW.NavState_Memory_PreviousInput.quat_nb[1] *
      Navigation_DW.NavState_Memory_PreviousInput.quat_nb[2];
    cy = (real_T)Navigation_DW.NavState_Memory_PreviousInput.quat_nb[0] *
      Navigation_DW.NavState_Memory_PreviousInput.quat_nb[3];
    Cbn[3] = (w - cy) * 2.0;
    sy = (real_T)Navigation_DW.NavState_Memory_PreviousInput.quat_nb[1] *
      Navigation_DW.NavState_Memory_PreviousInput.quat_nb[3];
    q_tmp = (real_T)Navigation_DW.NavState_Memory_PreviousInput.quat_nb[0] *
      Navigation_DW.NavState_Memory_PreviousInput.quat_nb[2];
    Cbn[6] = (sy + q_tmp) * 2.0;
    Cbn[1] = (w + cy) * 2.0;
    w = (real_T)Navigation_DW.NavState_Memory_PreviousInput.quat_nb[1] *
      Navigation_DW.NavState_Memory_PreviousInput.quat_nb[1];
    Cbn[4] = 1.0 - (w + cr) * 2.0;
    cr = (real_T)Navigation_DW.NavState_Memory_PreviousInput.quat_nb[2] *
      Navigation_DW.NavState_Memory_PreviousInput.quat_nb[3];
    cy = (real_T)Navigation_DW.NavState_Memory_PreviousInput.quat_nb[0] *
      Navigation_DW.NavState_Memory_PreviousInput.quat_nb[1];
    Cbn[7] = (cr - cy) * 2.0;
    Cbn[2] = (sy - q_tmp) * 2.0;
    Cbn[5] = (cr + cy) * 2.0;
    Cbn[8] = 1.0 - (w + dt) * 2.0;
    a_lg[0] = 0.0;
    a_lg[1] = 0.0;
    a_lg[2] = 0.0;
    cr = 0.0 * Navigation_Y.NavOut.gyro_body[2];
    dt = Navigation_Y.NavOut.gyro_body[1] * 0.0;
    theta[0] = dt - cr;
    w = Navigation_Y.NavOut.gyro_body[0] * 0.0;
    theta[1] = cr - w;
    theta[2] = w - dt;
    dt = 0.0;
    w = 0.0;
    cy = 0.0;
    sy = 0.0;
    q_tmp = 0.0;
    q_tmp_0 = 0.0;
    for (idx = 0; idx < 3; idx++) {
      sp = Cbn[3 * idx];
      dt += sp * 0.0;
      w_tmp = Cbn[3 * idx + 1];
      w += w_tmp * 0.0;
      cy_tmp = Cbn[3 * idx + 2];
      cy += cy_tmp * 0.0;
      cr = theta[idx];
      sy += sp * cr;
      q_tmp += w_tmp * cr;
      q_tmp_0 += cy_tmp * cr;
    }

    cr = sin(Navigation_DW.NavState_Memory_PreviousInput.origin_lla[0]);
    cr = 1.0 - 0.00669437999014 * cr * cr;
    sp = sqrt(cr);
    z[0] = (6.3354393272928288E+6 / (cr * sp) +
            Navigation_DW.NavState_Memory_PreviousInput.origin_lla[2]) *
      (Navigation_U.NavIn.gnss.pos_lla[0] -
       Navigation_DW.NavState_Memory_PreviousInput.origin_lla[0]) -
      (Navigation_DW.NavState_Memory_PreviousInput.pos_ned[0] + dt);
    z[1] = (6.378137E+6 / sp +
            Navigation_DW.NavState_Memory_PreviousInput.origin_lla[2]) *
      (Navigation_U.NavIn.gnss.pos_lla[1] -
       Navigation_DW.NavState_Memory_PreviousInput.origin_lla[1]) * cos
      (Navigation_DW.NavState_Memory_PreviousInput.origin_lla[0]) -
      (Navigation_DW.NavState_Memory_PreviousInput.pos_ned[1] + w);
    z[2] = -(Navigation_U.NavIn.gnss.pos_lla[2] -
             Navigation_DW.NavState_Memory_PreviousInput.origin_lla[2]) -
      (Navigation_DW.NavState_Memory_PreviousInput.pos_ned[2] + cy);
    z[3] = Navigation_U.NavIn.gnss.vel_ned[0] -
      (Navigation_DW.NavState_Memory_PreviousInput.vel_ned[0] + sy);
    z[4] = Navigation_U.NavIn.gnss.vel_ned[1] -
      (Navigation_DW.NavState_Memory_PreviousInput.vel_ned[1] + q_tmp);
    z[5] = Navigation_U.NavIn.gnss.vel_ned[2] -
      (Navigation_DW.NavState_Memory_PreviousInput.vel_ned[2] + q_tmp_0);
    memset(&H[0], 0, 90U * sizeof(real_T));
    Navigation_eye(H_tmp);
    for (idx = 0; idx < 3; idx++) {
      cr = H_tmp[3 * idx];
      Fdt_tmp = (idx + 6) * 6;
      H[Fdt_tmp] = cr;
      c_k = (idx + 3) * 6;
      H[c_k + 3] = cr;
      cr = H_tmp[3 * idx + 1];
      H[Fdt_tmp + 1] = cr;
      H[c_k + 4] = cr;
      cr = H_tmp[3 * idx + 2];
      H[Fdt_tmp + 2] = cr;
      H[c_k + 5] = cr;
    }

    if (Navigation_norm(a_lg) > 1.0E-6) {
      H[0] = -0.0;
      H[6] = cy;
      H[12] = -w;
      H[1] = -cy;
      H[7] = -0.0;
      H[13] = dt;
      H[2] = w;
      H[8] = -dt;
      H[14] = -0.0;
      H[3] = -0.0;
      H[9] = q_tmp_0;
      H[15] = -q_tmp;
      H[4] = -q_tmp_0;
      H[10] = -0.0;
      H[16] = sy;
      H[5] = q_tmp;
      H[11] = -sy;
      H[17] = -0.0;
      for (idx = 0; idx < 3; idx++) {
        cr = 0.0;
        dt = 0.0;
        w = 0.0;
        for (c_k = 0; c_k < 3; c_k++) {
          cr += Cbn[3 * c_k] * 0.0;
          dt += Cbn[3 * c_k + 1] * 0.0;
          w += Cbn[3 * c_k + 2] * 0.0;
        }

        Fdt_tmp = (idx + 12) * 6;
        H[Fdt_tmp + 5] = w;
        H[Fdt_tmp + 4] = dt;
        H[Fdt_tmp + 3] = cr;
      }
    }

    memset(&R[0], 0, 36U * sizeof(real_T));
    for (idx = 0; idx < 3; idx++) {
      R[6 * idx] = Navigation_ConstP.pooled1.R_gnss_pos[3 * idx];
      c_k = (idx + 3) * 6;
      R[c_k + 3] = Navigation_ConstP.pooled1.R_gnss_vel[3 * idx];
      Fdt_tmp = 3 * idx + 1;
      R[6 * idx + 1] = Navigation_ConstP.pooled1.R_gnss_pos[Fdt_tmp];
      R[c_k + 4] = Navigation_ConstP.pooled1.R_gnss_vel[Fdt_tmp];
      Fdt_tmp = 3 * idx + 2;
      R[6 * idx + 2] = Navigation_ConstP.pooled1.R_gnss_pos[Fdt_tmp];
      R[c_k + 5] = Navigation_ConstP.pooled1.R_gnss_vel[Fdt_tmp];
    }

    for (idx = 0; idx < 225; idx++) {
      Fdt[idx] = Navigation_DW.NavState_Memory_PreviousInput.P[idx];
    }

    for (idx = 0; idx < 6; idx++) {
      for (c_k = 0; c_k < 15; c_k++) {
        K[c_k + 15 * idx] = H[6 * c_k + idx];
      }
    }

    for (idx = 0; idx < 15; idx++) {
      for (c_k = 0; c_k < 6; c_k++) {
        H_0[c_k + 6 * idx] = 0.0;
      }

      for (c_k = 0; c_k < 15; c_k++) {
        cr = Fdt[15 * idx + c_k];
        for (b_k = 0; b_k < 6; b_k++) {
          Fdt_tmp = 6 * idx + b_k;
          H_0[Fdt_tmp] += H[6 * c_k + b_k] * cr;
        }
      }
    }

    for (idx = 0; idx < 6; idx++) {
      for (c_k = 0; c_k < 6; c_k++) {
        cr = 0.0;
        for (b_k = 0; b_k < 15; b_k++) {
          cr += H_0[6 * b_k + idx] * K[15 * c_k + b_k];
        }

        Fdt_tmp = 6 * c_k + idx;
        S[Fdt_tmp] = R[Fdt_tmp] + cr;
      }
    }

    for (idx = 0; idx < 6; idx++) {
      for (c_k = 0; c_k < 6; c_k++) {
        b_k = 6 * idx + c_k;
        tmp_1[b_k] = (S[6 * c_k + idx] + S[b_k]) * 0.5;
      }
    }

    memcpy(&S[0], &tmp_1[0], 36U * sizeof(real_T));
    for (idx = 0; idx < 3; idx++) {
      Cbn[3 * idx] = S[6 * idx];
      Cbn[3 * idx + 1] = S[6 * idx + 1];
      Cbn[3 * idx + 2] = S[6 * idx + 2];
    }

    Navigation_mldivide(Cbn, &z[0], theta);
    use_pos = ((z[0] * theta[0] + z[1] * theta[1]) + z[2] * theta[2] <= 60.0);
    for (idx = 0; idx < 3; idx++) {
      Fdt_tmp = (idx + 3) * 6;
      Cbn[3 * idx] = S[Fdt_tmp + 3];
      Cbn[3 * idx + 1] = S[Fdt_tmp + 4];
      Cbn[3 * idx + 2] = S[Fdt_tmp + 5];
    }

    Navigation_mldivide(Cbn, &z[3], theta);
    lg_ok = ((theta[0] * z[3] + theta[1] * z[4]) + theta[2] * z[5] <= 60.0);
    hg_ok = false;
    if ((!use_pos) && (!lg_ok)) {
      if ((rtb_state_out_reject_cnt >= 20) || (rtb_state_out_gnss_age_s > 1.0F))
      {
        use_pos = true;
        lg_ok = true;
        hg_ok = true;
      } else {
        /* Update for Memory: '<Root>/NavState_Memory' */
        Navigation_DW.NavState_Memory_PreviousInput.reject_cnt = (uint16_T)
          (rtb_state_out_reject_cnt + 1);
      }
    } else {
      hg_ok = (rtb_state_out_gnss_age_s > 1.0F);
    }

    if (hg_ok) {
      memset(&Cbn[0], 0, 9U * sizeof(real_T));
      Cbn[0] = z[3] * z[3];
      Cbn[4] = z[4] * z[4];
      Cbn[8] = z[5] * z[5];
      for (idx = 0; idx < 9; idx++) {
        H_tmp[idx] = c_a[idx];
      }

      for (b_k = 0; b_k < 3; b_k++) {
        c_k = (b_k + 3) * 15;
        Fdt[c_k + 3] = (Fdt[c_k + 3] + Cbn[3 * b_k]) + H_tmp[3 * b_k] * 25.0;
        Fdt_tmp = 3 * b_k + 1;
        Fdt[c_k + 4] = (Fdt[c_k + 4] + Cbn[Fdt_tmp]) + H_tmp[Fdt_tmp] * 25.0;
        Fdt_tmp = 3 * b_k + 2;
        Fdt[c_k + 5] = (Fdt[c_k + 5] + Cbn[Fdt_tmp]) + H_tmp[Fdt_tmp] * 25.0;
        cr = z[b_k];
        theta[b_k] = cr * cr;
      }

      memset(&Cbn[0], 0, 9U * sizeof(real_T));
      Cbn[0] = theta[0];
      Cbn[4] = theta[1];
      Cbn[8] = theta[2];
      for (idx = 0; idx < 3; idx++) {
        Fdt_tmp = (int32_T)H_tmp[3 * idx];
        c_k = (idx + 6) * 15;
        Fdt[c_k + 6] = (Fdt[c_k + 6] + Cbn[3 * idx]) + (real_T)Fdt_tmp * 100.0;
        b_k = (idx + 9) * 15;
        Fdt[b_k + 9] += (real_T)Fdt_tmp * 0.25;
        Fdt_tmp_0 = 3 * idx + 1;
        Fdt_tmp = (int32_T)H_tmp[Fdt_tmp_0];
        Fdt[c_k + 7] = (Fdt[c_k + 7] + Cbn[Fdt_tmp_0]) + (real_T)Fdt_tmp * 100.0;
        Fdt[b_k + 10] += (real_T)Fdt_tmp * 0.25;
        Fdt_tmp_0 = 3 * idx + 2;
        Fdt_tmp = (int32_T)H_tmp[Fdt_tmp_0];
        Fdt[c_k + 8] = (Fdt[c_k + 8] + Cbn[Fdt_tmp_0]) + (real_T)Fdt_tmp * 100.0;
        Fdt[b_k + 11] += (real_T)Fdt_tmp * 0.25;
      }

      for (idx = 0; idx < 15; idx++) {
        for (c_k = 0; c_k < 15; c_k++) {
          b_k = 15 * idx + c_k;
          P_pred[b_k] = (Fdt[15 * c_k + idx] + Fdt[b_k]) * 0.5;
        }
      }

      memcpy(&Fdt[0], &P_pred[0], 225U * sizeof(real_T));
    }

    if (use_pos || lg_ok) {
      if (!use_pos) {
        for (idx = 0; idx < 3; idx++) {
          R[6 * idx] = c[3 * idx];
          R[6 * idx + 1] = c[3 * idx + 1];
          R[6 * idx + 2] = c[3 * idx + 2];
        }
      }

      if (!lg_ok) {
        for (idx = 0; idx < 3; idx++) {
          c_k = (idx + 3) * 6;
          R[c_k + 3] = c[3 * idx];
          R[c_k + 4] = c[3 * idx + 1];
          R[c_k + 5] = c[3 * idx + 2];
        }
      }

      for (idx = 0; idx < 15; idx++) {
        for (c_k = 0; c_k < 6; c_k++) {
          H_0[c_k + 6 * idx] = 0.0;
        }

        for (c_k = 0; c_k < 15; c_k++) {
          cr = Fdt[15 * idx + c_k];
          for (b_k = 0; b_k < 6; b_k++) {
            Fdt_tmp = 6 * idx + b_k;
            H_0[Fdt_tmp] += H[6 * c_k + b_k] * cr;
          }
        }
      }

      for (idx = 0; idx < 6; idx++) {
        for (c_k = 0; c_k < 6; c_k++) {
          cr = 0.0;
          for (b_k = 0; b_k < 15; b_k++) {
            cr += H_0[6 * b_k + idx] * K[15 * c_k + b_k];
          }

          Fdt_tmp = 6 * c_k + idx;
          S[Fdt_tmp] = R[Fdt_tmp] + cr;
        }

        memset(&Fdt_1[idx * 15], 0, 15U * sizeof(real_T));
        for (c_k = 0; c_k < 15; c_k++) {
          cr = K[15 * idx + c_k];
          for (b_k = 0; b_k < 15; b_k++) {
            Fdt_tmp = 15 * idx + b_k;
            Fdt_1[Fdt_tmp] += Fdt[15 * c_k + b_k] * cr;
          }
        }
      }

      memcpy(&K[0], &Fdt_1[0], 90U * sizeof(real_T));
      for (idx = 0; idx < 6; idx++) {
        for (c_k = 0; c_k < 6; c_k++) {
          b_k = 6 * idx + c_k;
          tmp_1[b_k] = (S[6 * c_k + idx] + S[b_k]) * 0.5;
        }
      }

      Navigation_mrdiv_f(K, tmp_1);
      memset(&dx[0], 0, 15U * sizeof(real_T));
      for (idx = 0; idx < 6; idx++) {
        cr = z[idx];
        for (c_k = 0; c_k < 15; c_k++) {
          dx[c_k] += K[15 * idx + c_k] * cr;
        }
      }

      memset(&b_I[0], 0, 225U * sizeof(int8_T));
      for (c_k = 0; c_k < 15; c_k++) {
        b_I[c_k + 15 * c_k] = 1;
      }

      for (idx = 0; idx < 15; idx++) {
        for (c_k = 0; c_k < 15; c_k++) {
          cr = 0.0;
          for (b_k = 0; b_k < 6; b_k++) {
            cr += K[15 * b_k + idx] * H[6 * c_k + b_k];
          }

          Fdt_tmp = 15 * c_k + idx;
          P_pred[Fdt_tmp] = (real_T)b_I[Fdt_tmp] - cr;
          Phi[c_k + 15 * idx] = 0.0;
        }
      }

      for (idx = 0; idx < 15; idx++) {
        for (c_k = 0; c_k < 15; c_k++) {
          cr = Fdt[15 * idx + c_k];
          for (b_k = 0; b_k < 15; b_k++) {
            Fdt_tmp = 15 * idx + b_k;
            Phi[Fdt_tmp] += P_pred[15 * c_k + b_k] * cr;
          }
        }
      }

      for (idx = 0; idx < 6; idx++) {
        memset(&H[idx * 15], 0, 15U * sizeof(real_T));
        for (c_k = 0; c_k < 6; c_k++) {
          cr = R[6 * idx + c_k];
          for (b_k = 0; b_k < 15; b_k++) {
            Fdt_tmp = 15 * idx + b_k;
            H[Fdt_tmp] += K[15 * c_k + b_k] * cr;
          }
        }
      }

      for (idx = 0; idx < 15; idx++) {
        memset(&Fdt_0[idx * 15], 0, 15U * sizeof(real_T));
        for (c_k = 0; c_k < 15; c_k++) {
          cr = P_pred[15 * c_k + idx];
          for (b_k = 0; b_k < 15; b_k++) {
            Fdt_tmp = 15 * idx + b_k;
            Fdt_0[Fdt_tmp] += Phi[15 * c_k + b_k] * cr;
          }

          Phi_0[c_k + 15 * idx] = 0.0;
        }

        for (c_k = 0; c_k < 6; c_k++) {
          cr = K[15 * c_k + idx];
          for (b_k = 0; b_k < 15; b_k++) {
            Fdt_tmp = 15 * idx + b_k;
            Phi_0[Fdt_tmp] += H[15 * c_k + b_k] * cr;
          }
        }
      }

      for (idx = 0; idx < 225; idx++) {
        Fdt[idx] = Fdt_0[idx] + Phi_0[idx];
      }

      a_lg[0] = dx[9];
      theta[0] = Navigation_Y.NavOut.accel_body[0];
      a_lg[1] = dx[10];
      theta[1] = Navigation_Y.NavOut.accel_body[1];
      a_lg[2] = dx[11];
      theta[2] = Navigation_Y.NavOut.accel_body[2];
      if (Navigation_norm(theta) > 19.6133) {
        a_lg[0] = 0.0;
        a_lg[1] = 0.0;
        a_lg[2] = 0.0;
      }

      dq[0] = 1.0;
      dq[1] = 0.5 * dx[0];
      dq[2] = 0.5 * dx[1];
      dq[3] = 0.5 * dx[2];
      cr = Navigation_norm_am(dq);
      q_new[0] = 1.0 / cr;
      q_new[1] = dq[1] / cr;
      q_new[2] = dq[2] / cr;
      q_new[3] = dq[3] / cr;
      q_new_0[0] = ((q_new[0] *
                     Navigation_DW.NavState_Memory_PreviousInput.quat_nb[0] -
                     q_new[1] *
                     Navigation_DW.NavState_Memory_PreviousInput.quat_nb[1]) -
                    q_new[2] *
                    Navigation_DW.NavState_Memory_PreviousInput.quat_nb[2]) -
        q_new[3] * Navigation_DW.NavState_Memory_PreviousInput.quat_nb[3];
      q_new_0[1] = ((q_new[0] *
                     Navigation_DW.NavState_Memory_PreviousInput.quat_nb[1] +
                     Navigation_DW.NavState_Memory_PreviousInput.quat_nb[0] *
                     q_new[1]) + q_new[2] *
                    Navigation_DW.NavState_Memory_PreviousInput.quat_nb[3]) -
        Navigation_DW.NavState_Memory_PreviousInput.quat_nb[2] * q_new[3];
      q_new_0[2] = ((q_new[0] *
                     Navigation_DW.NavState_Memory_PreviousInput.quat_nb[2] -
                     q_new[1] *
                     Navigation_DW.NavState_Memory_PreviousInput.quat_nb[3]) +
                    Navigation_DW.NavState_Memory_PreviousInput.quat_nb[0] *
                    q_new[2]) +
        Navigation_DW.NavState_Memory_PreviousInput.quat_nb[1] * q_new[3];
      q_new_0[3] = ((q_new[0] *
                     Navigation_DW.NavState_Memory_PreviousInput.quat_nb[3] +
                     q_new[1] *
                     Navigation_DW.NavState_Memory_PreviousInput.quat_nb[2]) -
                    Navigation_DW.NavState_Memory_PreviousInput.quat_nb[1] *
                    q_new[2]) +
        Navigation_DW.NavState_Memory_PreviousInput.quat_nb[0] * q_new[3];
      cr = Navigation_norm_am(q_new_0);
      Navigation_DW.NavState_Memory_PreviousInput.quat_nb[0] = (real32_T)
        (q_new_0[0] / cr);
      Navigation_DW.NavState_Memory_PreviousInput.quat_nb[1] = (real32_T)
        (q_new_0[1] / cr);
      Navigation_DW.NavState_Memory_PreviousInput.quat_nb[2] = (real32_T)
        (q_new_0[2] / cr);
      Navigation_DW.NavState_Memory_PreviousInput.quat_nb[3] = (real32_T)
        (q_new_0[3] / cr);
      Navigation_DW.NavState_Memory_PreviousInput.vel_ned[0] = (real32_T)
        (Navigation_DW.NavState_Memory_PreviousInput.vel_ned[0] + dx[3]);
      Navigation_DW.NavState_Memory_PreviousInput.pos_ned[0] = (real32_T)
        (Navigation_DW.NavState_Memory_PreviousInput.pos_ned[0] + dx[6]);
      Navigation_DW.NavState_Memory_PreviousInput.accel_bias[0] = (real32_T)fmin
        (fmax(Navigation_DW.NavState_Memory_PreviousInput.accel_bias[0] + a_lg[0],
              -1.5), 1.5);
      Navigation_DW.NavState_Memory_PreviousInput.gyro_bias[0] = (real32_T)fmin
        (fmax(Navigation_DW.NavState_Memory_PreviousInput.gyro_bias[0] + dx[12],
              -0.087266462599716474), 0.087266462599716474);
      Navigation_DW.NavState_Memory_PreviousInput.vel_ned[1] = (real32_T)
        (Navigation_DW.NavState_Memory_PreviousInput.vel_ned[1] + dx[4]);
      Navigation_DW.NavState_Memory_PreviousInput.pos_ned[1] = (real32_T)
        (Navigation_DW.NavState_Memory_PreviousInput.pos_ned[1] + dx[7]);
      Navigation_DW.NavState_Memory_PreviousInput.accel_bias[1] = (real32_T)fmin
        (fmax(Navigation_DW.NavState_Memory_PreviousInput.accel_bias[1] + a_lg[1],
              -1.5), 1.5);
      Navigation_DW.NavState_Memory_PreviousInput.gyro_bias[1] = (real32_T)fmin
        (fmax(Navigation_DW.NavState_Memory_PreviousInput.gyro_bias[1] + dx[13],
              -0.087266462599716474), 0.087266462599716474);
      Navigation_DW.NavState_Memory_PreviousInput.vel_ned[2] = (real32_T)
        (Navigation_DW.NavState_Memory_PreviousInput.vel_ned[2] + dx[5]);
      Navigation_DW.NavState_Memory_PreviousInput.pos_ned[2] = (real32_T)
        (Navigation_DW.NavState_Memory_PreviousInput.pos_ned[2] + dx[8]);
      Navigation_DW.NavState_Memory_PreviousInput.accel_bias[2] = (real32_T)fmin
        (fmax(Navigation_DW.NavState_Memory_PreviousInput.accel_bias[2] + a_lg[2],
              -1.5), 1.5);
      Navigation_DW.NavState_Memory_PreviousInput.gyro_bias[2] = (real32_T)fmin
        (fmax(Navigation_DW.NavState_Memory_PreviousInput.gyro_bias[2] + dx[14],
              -0.087266462599716474), 0.087266462599716474);

      /* Update for Memory: '<Root>/NavState_Memory' */
      for (idx = 0; idx < 15; idx++) {
        for (c_k = 0; c_k < 15; c_k++) {
          b_k = 15 * idx + c_k;
          Navigation_DW.NavState_Memory_PreviousInput.P[b_k] = (real32_T)((Fdt
            [15 * c_k + idx] + Fdt[b_k]) * 0.5);
        }
      }

      rtb_state_out_gnss_age_s = 0.0F;

      /* Update for Memory: '<Root>/NavState_Memory' */
      Navigation_DW.NavState_Memory_PreviousInput.reject_cnt = 0U;
    }
  }

  tmp[0] = ((!rtIsInf(Navigation_DW.NavState_Memory_PreviousInput.quat_nb[0])) &&
            (!rtIsNaN(Navigation_DW.NavState_Memory_PreviousInput.quat_nb[0])));
  tmp[1] = ((!rtIsInf(Navigation_DW.NavState_Memory_PreviousInput.quat_nb[1])) &&
            (!rtIsNaN(Navigation_DW.NavState_Memory_PreviousInput.quat_nb[1])));
  tmp[2] = ((!rtIsInf(Navigation_DW.NavState_Memory_PreviousInput.quat_nb[2])) &&
            (!rtIsNaN(Navigation_DW.NavState_Memory_PreviousInput.quat_nb[2])));
  tmp[3] = ((!rtIsInf(Navigation_DW.NavState_Memory_PreviousInput.quat_nb[3])) &&
            (!rtIsNaN(Navigation_DW.NavState_Memory_PreviousInput.quat_nb[3])));
  if (Navigation_all(tmp)) {
    tmp_0[0] = ((!rtIsInf(Navigation_DW.NavState_Memory_PreviousInput.pos_ned[0]))
                && (!rtIsNaN
                    (Navigation_DW.NavState_Memory_PreviousInput.pos_ned[0])));
    tmp_0[1] = ((!rtIsInf(Navigation_DW.NavState_Memory_PreviousInput.pos_ned[1]))
                && (!rtIsNaN
                    (Navigation_DW.NavState_Memory_PreviousInput.pos_ned[1])));
    tmp_0[2] = ((!rtIsInf(Navigation_DW.NavState_Memory_PreviousInput.pos_ned[2]))
                && (!rtIsNaN
                    (Navigation_DW.NavState_Memory_PreviousInput.pos_ned[2])));
    if (Navigation_all_l(tmp_0)) {
      tmp_0[0] = ((!rtIsInf(Navigation_DW.NavState_Memory_PreviousInput.vel_ned
                            [0])) && (!rtIsNaN
        (Navigation_DW.NavState_Memory_PreviousInput.vel_ned[0])));
      tmp_0[1] = ((!rtIsInf(Navigation_DW.NavState_Memory_PreviousInput.vel_ned
                            [1])) && (!rtIsNaN
        (Navigation_DW.NavState_Memory_PreviousInput.vel_ned[1])));
      tmp_0[2] = ((!rtIsInf(Navigation_DW.NavState_Memory_PreviousInput.vel_ned
                            [2])) && (!rtIsNaN
        (Navigation_DW.NavState_Memory_PreviousInput.vel_ned[2])));
      if (Navigation_all_l(tmp_0)) {
        tmp_0[0] = ((!rtIsInf
                     (Navigation_DW.NavState_Memory_PreviousInput.accel_bias[0]))
                    && (!rtIsNaN
                        (Navigation_DW.NavState_Memory_PreviousInput.accel_bias
                         [0])));
        tmp_0[1] = ((!rtIsInf
                     (Navigation_DW.NavState_Memory_PreviousInput.accel_bias[1]))
                    && (!rtIsNaN
                        (Navigation_DW.NavState_Memory_PreviousInput.accel_bias
                         [1])));
        tmp_0[2] = ((!rtIsInf
                     (Navigation_DW.NavState_Memory_PreviousInput.accel_bias[2]))
                    && (!rtIsNaN
                        (Navigation_DW.NavState_Memory_PreviousInput.accel_bias
                         [2])));
        if (Navigation_all_l(tmp_0)) {
          tmp_0[0] = ((!rtIsInf
                       (Navigation_DW.NavState_Memory_PreviousInput.gyro_bias[0]))
                      && (!rtIsNaN
                          (Navigation_DW.NavState_Memory_PreviousInput.gyro_bias[
                           0])));
          tmp_0[1] = ((!rtIsInf
                       (Navigation_DW.NavState_Memory_PreviousInput.gyro_bias[1]))
                      && (!rtIsNaN
                          (Navigation_DW.NavState_Memory_PreviousInput.gyro_bias[
                           1])));
          tmp_0[2] = ((!rtIsInf
                       (Navigation_DW.NavState_Memory_PreviousInput.gyro_bias[2]))
                      && (!rtIsNaN
                          (Navigation_DW.NavState_Memory_PreviousInput.gyro_bias[
                           2])));
          use_pos = Navigation_all_l(tmp_0);
        } else {
          use_pos = false;
        }
      } else {
        use_pos = false;
      }
    } else {
      use_pos = false;
    }
  } else {
    use_pos = false;
  }

  if (!use_pos) {
    Navigation_DW.NavState_Memory_PreviousInput.quat_nb[0] = 1.0F;
    Navigation_DW.NavState_Memory_PreviousInput.quat_nb[1] = 0.0F;
    Navigation_DW.NavState_Memory_PreviousInput.quat_nb[2] = 0.0F;
    Navigation_DW.NavState_Memory_PreviousInput.quat_nb[3] = 0.0F;
    Navigation_DW.NavState_Memory_PreviousInput.pos_ned[0] = 0.0F;
    Navigation_DW.NavState_Memory_PreviousInput.vel_ned[0] = 0.0F;
    Navigation_DW.NavState_Memory_PreviousInput.accel_bias[0] = 0.0F;
    Navigation_DW.NavState_Memory_PreviousInput.gyro_bias[0] = 0.0F;
    Navigation_DW.NavState_Memory_PreviousInput.pos_ned[1] = 0.0F;
    Navigation_DW.NavState_Memory_PreviousInput.vel_ned[1] = 0.0F;
    Navigation_DW.NavState_Memory_PreviousInput.accel_bias[1] = 0.0F;
    Navigation_DW.NavState_Memory_PreviousInput.gyro_bias[1] = 0.0F;
    Navigation_DW.NavState_Memory_PreviousInput.pos_ned[2] = 0.0F;
    Navigation_DW.NavState_Memory_PreviousInput.vel_ned[2] = 0.0F;
    Navigation_DW.NavState_Memory_PreviousInput.accel_bias[2] = 0.0F;
    Navigation_DW.NavState_Memory_PreviousInput.gyro_bias[2] = 0.0F;

    /* Update for Memory: '<Root>/NavState_Memory' */
    memcpy(&Navigation_DW.NavState_Memory_PreviousInput.P[0],
           &Navigation_ConstP.pooled1.P0[0], 225U * sizeof(real32_T));
    rtb_state_out_init_done = 0U;

    /* Update for Memory: '<Root>/NavState_Memory' */
    Navigation_DW.NavState_Memory_PreviousInput.reject_cnt = 0U;
    rtb_state_out_o_align_cnt = 0U;
    Navigation_DW.NavState_Memory_PreviousInput.acc_sum[0] = 0.0F;
    Navigation_DW.NavState_Memory_PreviousInput.gyr_sum[0] = 0.0F;
    Navigation_DW.NavState_Memory_PreviousInput.acc_sum[1] = 0.0F;
    Navigation_DW.NavState_Memory_PreviousInput.gyr_sum[1] = 0.0F;
    Navigation_DW.NavState_Memory_PreviousInput.acc_sum[2] = 0.0F;
    Navigation_DW.NavState_Memory_PreviousInput.gyr_sum[2] = 0.0F;
  }

  /* Outport: '<Root>/NavOut' incorporates:
   *  MATLAB Function: '<Root>/EKF_Update'
   */
  Navigation_Y.NavOut.pos_ned[0] =
    Navigation_DW.NavState_Memory_PreviousInput.pos_ned[0];
  Navigation_Y.NavOut.vel_ned[0] =
    Navigation_DW.NavState_Memory_PreviousInput.vel_ned[0];
  Navigation_Y.NavOut.pos_ned[1] =
    Navigation_DW.NavState_Memory_PreviousInput.pos_ned[1];
  Navigation_Y.NavOut.vel_ned[1] =
    Navigation_DW.NavState_Memory_PreviousInput.vel_ned[1];
  Navigation_Y.NavOut.pos_ned[2] =
    Navigation_DW.NavState_Memory_PreviousInput.pos_ned[2];
  Navigation_Y.NavOut.vel_ned[2] =
    Navigation_DW.NavState_Memory_PreviousInput.vel_ned[2];
  Navigation_Y.NavOut.quat_nb[0] =
    Navigation_DW.NavState_Memory_PreviousInput.quat_nb[0];
  Navigation_Y.NavOut.quat_nb[1] =
    Navigation_DW.NavState_Memory_PreviousInput.quat_nb[1];
  Navigation_Y.NavOut.quat_nb[2] =
    Navigation_DW.NavState_Memory_PreviousInput.quat_nb[2];
  Navigation_Y.NavOut.quat_nb[3] =
    Navigation_DW.NavState_Memory_PreviousInput.quat_nb[3];

  /* MATLAB Function: '<Root>/EKF_Update' */
  cr = (real_T)Navigation_DW.NavState_Memory_PreviousInput.quat_nb[2] *
    Navigation_DW.NavState_Memory_PreviousInput.quat_nb[2];

  /* Outport: '<Root>/NavOut' incorporates:
   *  MATLAB Function: '<Root>/EKF_Prediction'
   *  MATLAB Function: '<Root>/EKF_Update'
   */
  Navigation_Y.NavOut.euler_rpy[0] = (real32_T)rt_atan2d_snf(((real_T)
    Navigation_DW.NavState_Memory_PreviousInput.quat_nb[2] *
    Navigation_DW.NavState_Memory_PreviousInput.quat_nb[3] + (real_T)
    Navigation_DW.NavState_Memory_PreviousInput.quat_nb[0] *
    Navigation_DW.NavState_Memory_PreviousInput.quat_nb[1]) * 2.0, 1.0 -
    ((real_T)Navigation_DW.NavState_Memory_PreviousInput.quat_nb[1] *
     Navigation_DW.NavState_Memory_PreviousInput.quat_nb[1] + cr) * 2.0);
  Navigation_Y.NavOut.euler_rpy[1] = (real32_T)asin(fmax(fmin(-(((real_T)
    Navigation_DW.NavState_Memory_PreviousInput.quat_nb[1] *
    Navigation_DW.NavState_Memory_PreviousInput.quat_nb[3] - (real_T)
    Navigation_DW.NavState_Memory_PreviousInput.quat_nb[0] *
    Navigation_DW.NavState_Memory_PreviousInput.quat_nb[2]) * 2.0), 1.0), -1.0));
  Navigation_Y.NavOut.euler_rpy[2] = (real32_T)rt_atan2d_snf(((real_T)
    Navigation_DW.NavState_Memory_PreviousInput.quat_nb[1] *
    Navigation_DW.NavState_Memory_PreviousInput.quat_nb[2] + (real_T)
    Navigation_DW.NavState_Memory_PreviousInput.quat_nb[0] *
    Navigation_DW.NavState_Memory_PreviousInput.quat_nb[3]) * 2.0, 1.0 -
    ((real_T)Navigation_DW.NavState_Memory_PreviousInput.quat_nb[3] *
     Navigation_DW.NavState_Memory_PreviousInput.quat_nb[3] + cr) * 2.0);
  Navigation_Y.NavOut.imu_blend_w = rtb_state_out_imu_blend_w;
  Navigation_Y.NavOut.gnss_age_s = rtb_state_out_gnss_age_s;
  Navigation_Y.NavOut.timestamp_s = dt_meas;
  Navigation_Y.NavOut.nav_valid = rtb_state_out_init_done;
  Navigation_Y.NavOut.accel_bias[0] =
    Navigation_DW.NavState_Memory_PreviousInput.accel_bias[0];
  Navigation_Y.NavOut.gyro_bias[0] =
    Navigation_DW.NavState_Memory_PreviousInput.gyro_bias[0];

  /* Update for Memory: '<Root>/NavState_Memory' incorporates:
   *  MATLAB Function: '<Root>/EKF_Prediction'
   */
  Navigation_DW.NavState_Memory_PreviousInput.accel_ned[0] =
    Navigation_Y.NavOut.accel_ned[0];
  Navigation_DW.NavState_Memory_PreviousInput.accel_body[0] =
    Navigation_Y.NavOut.accel_body[0];
  Navigation_DW.NavState_Memory_PreviousInput.gyro_body[0] =
    Navigation_Y.NavOut.gyro_body[0];

  /* Outport: '<Root>/NavOut' incorporates:
   *  MATLAB Function: '<Root>/EKF_Update'
   */
  Navigation_Y.NavOut.accel_bias[1] =
    Navigation_DW.NavState_Memory_PreviousInput.accel_bias[1];
  Navigation_Y.NavOut.gyro_bias[1] =
    Navigation_DW.NavState_Memory_PreviousInput.gyro_bias[1];

  /* Update for Memory: '<Root>/NavState_Memory' incorporates:
   *  MATLAB Function: '<Root>/EKF_Prediction'
   */
  Navigation_DW.NavState_Memory_PreviousInput.accel_ned[1] =
    Navigation_Y.NavOut.accel_ned[1];
  Navigation_DW.NavState_Memory_PreviousInput.accel_body[1] =
    Navigation_Y.NavOut.accel_body[1];
  Navigation_DW.NavState_Memory_PreviousInput.gyro_body[1] =
    Navigation_Y.NavOut.gyro_body[1];

  /* Outport: '<Root>/NavOut' incorporates:
   *  MATLAB Function: '<Root>/EKF_Update'
   */
  Navigation_Y.NavOut.accel_bias[2] =
    Navigation_DW.NavState_Memory_PreviousInput.accel_bias[2];
  Navigation_Y.NavOut.gyro_bias[2] =
    Navigation_DW.NavState_Memory_PreviousInput.gyro_bias[2];

  /* Update for Memory: '<Root>/NavState_Memory' incorporates:
   *  MATLAB Function: '<Root>/EKF_Prediction'
   */
  Navigation_DW.NavState_Memory_PreviousInput.accel_ned[2] =
    Navigation_Y.NavOut.accel_ned[2];
  Navigation_DW.NavState_Memory_PreviousInput.accel_body[2] =
    Navigation_Y.NavOut.accel_body[2];
  Navigation_DW.NavState_Memory_PreviousInput.gyro_body[2] =
    Navigation_Y.NavOut.gyro_body[2];
  Navigation_DW.NavState_Memory_PreviousInput.imu_blend_w =
    rtb_state_out_imu_blend_w;
  Navigation_DW.NavState_Memory_PreviousInput.origin_set =
    rtb_state_out_origin_set;
  Navigation_DW.NavState_Memory_PreviousInput.align_cnt =
    rtb_state_out_o_align_cnt;
  Navigation_DW.NavState_Memory_PreviousInput.gnss_age_s =
    rtb_state_out_gnss_age_s;
  Navigation_DW.NavState_Memory_PreviousInput.descent_cnt =
    rtb_state_out_descent_cnt;
  Navigation_DW.NavState_Memory_PreviousInput.timestamp_s = dt_meas;
  Navigation_DW.NavState_Memory_PreviousInput.init_done =
    rtb_state_out_init_done;
}

/* Model initialize function */
void Navigation_initialize(void)
{
  /* InitializeConditions for Memory: '<Root>/NavState_Memory' */
  Navigation_DW.NavState_Memory_PreviousInput =
    Navigation_ConstP.NavState_Memory_InitialConditio;
}

/* Model terminate function */
void Navigation_terminate(void)
{
  /* (no terminate code required) */
}

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
