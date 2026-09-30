/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: Navigation.h
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

#ifndef Navigation_h_
#define Navigation_h_
#ifndef Navigation_COMMON_INCLUDES_
#define Navigation_COMMON_INCLUDES_
#include "rtwtypes.h"
#include "rt_nonfinite.h"
#include "math.h"
#endif                                 /* Navigation_COMMON_INCLUDES_ */

#include "Navigation_types.h"
#include "rtGetInf.h"
#include "rtGetNaN.h"

/* Macros for accessing real-time model data structure */
#ifndef rtmGetErrorStatus
#define rtmGetErrorStatus(rtm)         ((rtm)->errorStatus)
#endif

#ifndef rtmSetErrorStatus
#define rtmSetErrorStatus(rtm, val)    ((rtm)->errorStatus = (val))
#endif

/* Block states (default storage) for system '<Root>' */
typedef struct {
  Bus_NavState NavState_Memory_PreviousInput;/* '<Root>/NavState_Memory' */
} DW_Navigation_T;

/* Constant parameters (default storage) */
typedef struct {
  /* Pooled Parameter (Expression: nav_data)
   * Referenced by:
   *   '<Root>/EKF_Prediction'
   *   '<Root>/EKF_Update'
   *   '<Root>/INS_Mechanization'
   */
  struct_51YCxKSCmnqZ8M6ffYluUB pooled1;

  /* Expression: nav_state0
   * Referenced by: '<Root>/NavState_Memory'
   */
  Bus_NavState NavState_Memory_InitialConditio;
} ConstP_Navigation_T;

/* External inputs (root inport signals with default storage) */
typedef struct {
  nav_bus_in NavIn;                    /* '<Root>/NavIn' */
} ExtU_Navigation_T;

/* External outputs (root outports fed by signals with default storage) */
typedef struct {
  nav_bus_out NavOut;                  /* '<Root>/NavOut' */
} ExtY_Navigation_T;

/* Real-time Model Data Structure */
struct tag_RTM_Navigation_T {
  const char_T * volatile errorStatus;
};

/* Block states (default storage) */
extern DW_Navigation_T Navigation_DW;

/* External inputs (root inport signals with default storage) */
extern ExtU_Navigation_T Navigation_U;

/* External outputs (root outports fed by signals with default storage) */
extern ExtY_Navigation_T Navigation_Y;

/* Constant parameters (default storage) */
extern const ConstP_Navigation_T Navigation_ConstP;

/*
 * Exported Global Parameters
 *
 * Note: Exported global parameters are tunable parameters with an exported
 * global storage class designation.  Code generation will declare the memory for
 * these parameters and exports their symbols.
 *
 */
extern Bus_NavTunables nav_tun;        /* Variable: nav_tun
                                        * Referenced by: '<Root>/INS_Mechanization'
                                        */

/* Model entry point functions */
extern void Navigation_initialize(void);
extern void Navigation_step(void);
extern void Navigation_terminate(void);

/* Real-time Model object */
extern RT_MODEL_Navigation_T *const Navigation_M;

/*-
 * The generated code includes comments that allow you to trace directly
 * back to the appropriate location in the model.  The basic format
 * is <system>/block_name, where system is the system number (uniquely
 * assigned by Simulink) and block_name is the name of the block.
 *
 * Use the MATLAB hilite_system command to trace the generated code back
 * to the model.  For example,
 *
 * hilite_system('<S3>')    - opens system 3
 * hilite_system('<S3>/Kp') - opens and selects block Kp which resides in S3
 *
 * Here is the system hierarchy for this model
 *
 * '<Root>' : 'Navigation'
 * '<S1>'   : 'Navigation/EKF_Prediction'
 * '<S2>'   : 'Navigation/EKF_Update'
 * '<S3>'   : 'Navigation/INS_Mechanization'
 */
#endif                                 /* Navigation_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
