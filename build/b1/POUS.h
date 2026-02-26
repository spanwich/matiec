#ifndef __POUS_H
#define __POUS_H

#include "accessor.h"
#include "iec_std_lib.h"

// PROGRAM ESD_INTERLOCK
// Data part
typedef struct {
  // PROGRAM Interface - IN, OUT, IN_OUT variables

  // PROGRAM private variables - TEMP, private and located variables
  __DECLARE_VAR(BOOL,PRESSURE_HIGH)
  __DECLARE_VAR(BOOL,TEMP_HIGH)
  __DECLARE_VAR(BOOL,FLOW_LOW)
  __DECLARE_VAR(BOOL,MANUAL_RESET)
  __DECLARE_VAR(BOOL,ANY_TRIP)
  SR_data__ ALARM_LATCH;
  TON_data__ ALARM_DELAY;
  __DECLARE_VAR(BOOL,ALARM_ACTIVE)
  __DECLARE_VAR(BOOL,LATCHED)
  __DECLARE_VAR(BOOL,SHUTDOWN_CMD)
  __DECLARE_VAR(BOOL,ALARM_OUT)

} ESD_INTERLOCK_data__;

void ESD_INTERLOCK_init__(ESD_INTERLOCK_data__ *data__, BOOL retain);
// Code part
void ESD_INTERLOCK_body__(ESD_INTERLOCK_data__ *data__);
#endif //__POUS_H
