#ifndef __POUS_H
#define __POUS_H

#include "accessor.h"
#include "iec_std_lib.h"

// PROGRAM BMS_PURGE
// Data part
typedef struct {
  // PROGRAM Interface - IN, OUT, IN_OUT variables

  // PROGRAM private variables - TEMP, private and located variables
  __DECLARE_VAR(BOOL,START_CMD)
  __DECLARE_VAR(BOOL,FLAME_SENSOR)
  __DECLARE_VAR(BOOL,PURGE_OK)
  __DECLARE_VAR(INT,STAGE)
  R_TRIG_data__ START_EDGE;
  F_TRIG_data__ FLAME_LOST;
  TON_data__ PURGE_TIMER;
  TON_data__ IGNITION_TIMER;
  __DECLARE_VAR(BOOL,FAN_ON)
  __DECLARE_VAR(BOOL,FUEL_VALVE)
  __DECLARE_VAR(BOOL,IGNITER_ON)
  __DECLARE_VAR(BOOL,ALARM_LOCKOUT)

} BMS_PURGE_data__;

void BMS_PURGE_init__(BMS_PURGE_data__ *data__, BOOL retain);
// Code part
void BMS_PURGE_body__(BMS_PURGE_data__ *data__);
#endif //__POUS_H
