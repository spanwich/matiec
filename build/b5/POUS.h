#ifndef __POUS_H
#define __POUS_H

#include "accessor.h"
#include "iec_std_lib.h"

// PROGRAM STRESS_TEST
// Data part
typedef struct {
  // PROGRAM Interface - IN, OUT, IN_OUT variables

  // PROGRAM private variables - TEMP, private and located variables
  __DECLARE_VAR(BOOL,INP_A)
  __DECLARE_VAR(BOOL,INP_B)
  __DECLARE_VAR(BOOL,INP_C)
  __DECLARE_VAR(BOOL,INP_D)
  __DECLARE_VAR(INT,INP_E)
  __DECLARE_VAR(INT,INP_F)
  __DECLARE_VAR(INT,INP_G)
  __DECLARE_VAR(TIME,INP_H)
  SR_data__ FB_SR;
  RS_data__ FB_RS;
  CTU_data__ FB_CTU;
  CTD_data__ FB_CTD;
  CTUD_data__ FB_CTUD;
  TON_data__ FB_TON;
  TOF_data__ FB_TOF;
  R_TRIG_data__ FB_RTRIG;
  F_TRIG_data__ FB_FTRIG;
  __DECLARE_VAR(BOOL,BOOL_RES)
  __DECLARE_VAR(INT,INT_RES)
  __DECLARE_VAR(INT,SUM_ACC)
  __DECLARE_VAR(INT,IDX)
  __DECLARE_VAR(BOOL,CMP_EQ)
  __DECLARE_VAR(BOOL,CMP_NE)
  __DECLARE_VAR(BOOL,CMP_LT)
  __DECLARE_VAR(BOOL,CMP_GT)
  __DECLARE_VAR(BOOL,CMP_LE)
  __DECLARE_VAR(BOOL,CMP_GE)
  __DECLARE_VAR(BOOL,OUT_ALARM)
  __DECLARE_VAR(INT,OUT_COUNT)
  __DECLARE_VAR(BOOL,OUT_TIMER_Q)
  __DECLARE_VAR(INT,OUT_RESULT)

} STRESS_TEST_data__;

void STRESS_TEST_init__(STRESS_TEST_data__ *data__, BOOL retain);
// Code part
void STRESS_TEST_body__(STRESS_TEST_data__ *data__);
#endif //__POUS_H
