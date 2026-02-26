#ifndef __POUS_H
#define __POUS_H

#include "accessor.h"
#include "iec_std_lib.h"

// PROGRAM SHUTDOWN_SEQ
// Data part
typedef struct {
  // PROGRAM Interface - IN, OUT, IN_OUT variables

  // PROGRAM private variables - TEMP, private and located variables
  __DECLARE_VAR(BOOL,SHUTDOWN_REQ)
  __DECLARE_VAR(INT,STAGE)
  __DECLARE_VAR(INT,VALVE_POS)
  TON_data__ COOLDOWN_TMR;
  TON_data__ DEPRESS_TMR;
  TON_data__ SAFE_TMR;
  __DECLARE_VAR(BOOL,HEATER_ON)
  __DECLARE_VAR(BOOL,PUMP_ON)
  __DECLARE_VAR(BOOL,VENT_OPEN)
  __DECLARE_VAR(BOOL,SAFE_STATE)

} SHUTDOWN_SEQ_data__;

void SHUTDOWN_SEQ_init__(SHUTDOWN_SEQ_data__ *data__, BOOL retain);
// Code part
void SHUTDOWN_SEQ_body__(SHUTDOWN_SEQ_data__ *data__);
#endif //__POUS_H
