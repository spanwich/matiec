#ifndef __POUS_H
#define __POUS_H

#include "accessor.h"
#include "iec_std_lib.h"

// PROGRAM CONVEYOR_COUNT
// Data part
typedef struct {
  // PROGRAM Interface - IN, OUT, IN_OUT variables

  // PROGRAM private variables - TEMP, private and located variables
  __DECLARE_VAR(BOOL,PART_DETECT)
  __DECLARE_VAR(BOOL,BATCH_RESET)
  __DECLARE_VAR(BOOL,RUN_CMD)
  __DECLARE_VAR(INT,BATCH_SIZE)
  R_TRIG_data__ PART_EDGE;
  CTU_data__ PART_COUNTER;
  __DECLARE_VAR(BOOL,MOTOR_LATCH)
  __DECLARE_VAR(INT,BATCH_TOTAL)
  __DECLARE_VAR(INT,IDX)
  __DECLARE_VAR(BOOL,MOTOR_ON)
  __DECLARE_VAR(BOOL,BATCH_COMPLETE)
  __DECLARE_VAR(INT,TOTAL_COUNT)

} CONVEYOR_COUNT_data__;

void CONVEYOR_COUNT_init__(CONVEYOR_COUNT_data__ *data__, BOOL retain);
// Code part
void CONVEYOR_COUNT_body__(CONVEYOR_COUNT_data__ *data__);
#endif //__POUS_H
