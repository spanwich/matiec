void CONVEYOR_COUNT_init__(CONVEYOR_COUNT_data__ *data__, BOOL retain) {
  __INIT_VAR(data__->PART_DETECT,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->BATCH_RESET,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->RUN_CMD,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->BATCH_SIZE,10,retain)
  R_TRIG_init__(&data__->PART_EDGE,retain);
  CTU_init__(&data__->PART_COUNTER,retain);
  __INIT_VAR(data__->MOTOR_LATCH,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->BATCH_TOTAL,0,retain)
  __INIT_VAR(data__->IDX,0,retain)
  __INIT_VAR(data__->MOTOR_ON,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->BATCH_COMPLETE,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->TOTAL_COUNT,0,retain)
}

// Code part
void CONVEYOR_COUNT_body__(CONVEYOR_COUNT_data__ *data__) {
  // Initialise TEMP variables

  __SET_VAR(data__->PART_EDGE.,CLK,,__GET_VAR(data__->PART_DETECT,));
  R_TRIG_body__(&data__->PART_EDGE);
  __SET_VAR(data__->PART_COUNTER.,CU,,__GET_VAR(data__->PART_EDGE.Q,));
  __SET_VAR(data__->PART_COUNTER.,R,,__GET_VAR(data__->BATCH_RESET,));
  __SET_VAR(data__->PART_COUNTER.,PV,,__GET_VAR(data__->BATCH_SIZE,));
  CTU_body__(&data__->PART_COUNTER);
  if (__GET_VAR(data__->RUN_CMD,)) {
    __SET_VAR(data__->,MOTOR_LATCH,,__BOOL_LITERAL(TRUE));
  };
  if (__GET_VAR(data__->PART_COUNTER.Q,)) {
    __SET_VAR(data__->,MOTOR_LATCH,,__BOOL_LITERAL(FALSE));
  };
  __SET_VAR(data__->,MOTOR_ON,,__GET_VAR(data__->MOTOR_LATCH,));
  __SET_VAR(data__->,BATCH_COMPLETE,,__GET_VAR(data__->PART_COUNTER.Q,));
  __SET_VAR(data__->,TOTAL_COUNT,,__GET_VAR(data__->PART_COUNTER.CV,));
  if (__GET_VAR(data__->BATCH_COMPLETE,)) {
    __SET_VAR(data__->,BATCH_TOTAL,,0);
    /* FOR ... */
    __SET_VAR(data__->,IDX,,1);
    {
      int __do_increment = 0;
      while(1) {
        if(__do_increment){
          /* BY ... (of FOR loop) */
          __SET_VAR(data__->,IDX,,(__GET_VAR(data__->IDX,) + 1));
        } else __do_increment = 1;
        if(__GET_VAR(data__->IDX,) <= 5        ){
          __SET_VAR(data__->,BATCH_TOTAL,,(__GET_VAR(data__->BATCH_TOTAL,) + __GET_VAR(data__->BATCH_SIZE,)));
        }else break;
      }
    } /* END_FOR */;
  };

  goto __end;

__end:
  return;
} // CONVEYOR_COUNT_body__() 





