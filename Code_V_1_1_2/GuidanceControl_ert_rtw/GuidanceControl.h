/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: GuidanceControl.h
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

#ifndef GuidanceControl_h_
#define GuidanceControl_h_
#ifndef GuidanceControl_COMMON_INCLUDES_
#define GuidanceControl_COMMON_INCLUDES_
#include "rtwtypes.h"
#include "rt_nonfinite.h"
#include "math.h"
#endif                                 /* GuidanceControl_COMMON_INCLUDES_ */

#include "GuidanceControl_types.h"
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
  Bus_GCState GCState_Memory_PreviousInput;/* '<Root>/GCState_Memory' */
} DW_GuidanceControl_T;

/* Constant parameters (default storage) */
typedef struct {
  /* Expression: gc_data
   * Referenced by: '<Root>/GuidanceControl_Logic'
   */
  struct_fab7xe5zbqKImI9YqNejpE GuidanceControl_Logic_gc_data;
} ConstP_GuidanceControl_T;

/* External inputs (root inport signals with default storage) */
typedef struct {
  gc_bus_in GCIn;                      /* '<Root>/GCIn' */
} ExtU_GuidanceControl_T;

/* External outputs (root outports fed by signals with default storage) */
typedef struct {
  gc_bus_out GCOut;                    /* '<Root>/GCOut' */
} ExtY_GuidanceControl_T;

/* Real-time Model Data Structure */
struct tag_RTM_GuidanceControl_T {
  const char_T * volatile errorStatus;
};

/* Block states (default storage) */
extern DW_GuidanceControl_T GuidanceControl_DW;

/* External inputs (root inport signals with default storage) */
extern ExtU_GuidanceControl_T GuidanceControl_U;

/* External outputs (root outports fed by signals with default storage) */
extern ExtY_GuidanceControl_T GuidanceControl_Y;

/* Constant parameters (default storage) */
extern const ConstP_GuidanceControl_T GuidanceControl_ConstP;

/*
 * Exported Global Parameters
 *
 * Note: Exported global parameters are tunable parameters with an exported
 * global storage class designation.  Code generation will declare the memory for
 * these parameters and exports their symbols.
 *
 */
extern Bus_GCTunables gc_tun;          /* Variable: gc_tun
                                        * Referenced by: '<Root>/GuidanceControl_Logic'
                                        */

/* Model entry point functions */
extern void GuidanceControl_initialize(void);
extern void GuidanceControl_step(void);
extern void GuidanceControl_terminate(void);

/* Real-time Model object */
extern RT_MODEL_GuidanceControl_T *const GuidanceControl_M;

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
 * '<Root>' : 'GuidanceControl'
 * '<S1>'   : 'GuidanceControl/GuidanceControl_Logic'
 */
#endif                                 /* GuidanceControl_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
