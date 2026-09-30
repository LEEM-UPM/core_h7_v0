/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: StateMachine.h
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

#ifndef StateMachine_h_
#define StateMachine_h_
#ifndef StateMachine_COMMON_INCLUDES_
#define StateMachine_COMMON_INCLUDES_
#include "rtwtypes.h"
#include "rt_nonfinite.h"
#include "math.h"
#endif                                 /* StateMachine_COMMON_INCLUDES_ */

#include "StateMachine_types.h"
#include "rtGetInf.h"

/* Macros for accessing real-time model data structure */
#ifndef rtmGetErrorStatus
#define rtmGetErrorStatus(rtm)         ((rtm)->errorStatus)
#endif

#ifndef rtmSetErrorStatus
#define rtmSetErrorStatus(rtm, val)    ((rtm)->errorStatus = (val))
#endif

/* Block states (default storage) for system '<Root>' */
typedef struct {
  Bus_SMState SMState_Memory_PreviousInput;/* '<Root>/SMState_Memory' */
} DW_StateMachine_T;

/* External inputs (root inport signals with default storage) */
typedef struct {
  sm_bus_in SMIn;                      /* '<Root>/SMIn' */
} ExtU_StateMachine_T;

/* External outputs (root outports fed by signals with default storage) */
typedef struct {
  sm_bus_out SMOut;                    /* '<Root>/SMOut' */
} ExtY_StateMachine_T;

/* Real-time Model Data Structure */
struct tag_RTM_StateMachine_T {
  const char_T * volatile errorStatus;
};

/* Block states (default storage) */
extern DW_StateMachine_T StateMachine_DW;

/* External inputs (root inport signals with default storage) */
extern ExtU_StateMachine_T StateMachine_U;

/* External outputs (root outports fed by signals with default storage) */
extern ExtY_StateMachine_T StateMachine_Y;

/*
 * Exported Global Parameters
 *
 * Note: Exported global parameters are tunable parameters with an exported
 * global storage class designation.  Code generation will declare the memory for
 * these parameters and exports their symbols.
 *
 */
extern Bus_SMTunables sm_tun;          /* Variable: sm_tun
                                        * Referenced by: '<Root>/StateMachine_Logic'
                                        */

/* Model entry point functions */
extern void StateMachine_initialize(void);
extern void StateMachine_step(void);
extern void StateMachine_terminate(void);

/* Real-time Model object */
extern RT_MODEL_StateMachine_T *const StateMachine_M;

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
 * '<Root>' : 'StateMachine'
 * '<S1>'   : 'StateMachine/StateMachine_Logic'
 */
#endif                                 /* StateMachine_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
