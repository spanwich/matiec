void ESD_INTERLOCK_init__(ESD_INTERLOCK_data__ *data__, BOOL retain) {
  __INIT_VAR(data__->PRESSURE_HIGH,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->TEMP_HIGH,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->FLOW_LOW,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->MANUAL_RESET,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->ANY_TRIP,__BOOL_LITERAL(FALSE),retain)
  SR_init__(&data__->ALARM_LATCH,retain);
  TON_init__(&data__->ALARM_DELAY,retain);
  __INIT_VAR(data__->ALARM_ACTIVE,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->LATCHED,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->SHUTDOWN_CMD,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->ALARM_OUT,__BOOL_LITERAL(FALSE),retain)
}

// Code part
void ESD_INTERLOCK_body__(ESD_INTERLOCK_data__ *data__) {
  // Initialise TEMP variables

  __SET_VAR(data__->,ANY_TRIP,,((__GET_VAR(data__->PRESSURE_HIGH,) || __GET_VAR(data__->TEMP_HIGH,)) || __GET_VAR(data__->FLOW_LOW,)));
  __SET_VAR(data__->ALARM_LATCH.,S1,,__GET_VAR(data__->ANY_TRIP,));
  __SET_VAR(data__->ALARM_LATCH.,R,,__GET_VAR(data__->MANUAL_RESET,));
  SR_body__(&data__->ALARM_LATCH);
  __SET_VAR(data__->,LATCHED,,__GET_VAR(data__->ALARM_LATCH.Q1,));
  __SET_VAR(data__->ALARM_DELAY.,IN,,__GET_VAR(data__->LATCHED,));
  __SET_VAR(data__->ALARM_DELAY.,PT,,__time_to_timespec(1, 0, 3, 0, 0, 0));
  TON_body__(&data__->ALARM_DELAY);
  if ((__GET_VAR(data__->LATCHED,) && __GET_VAR(data__->ALARM_DELAY.Q,))) {
    __SET_VAR(data__->,SHUTDOWN_CMD,,__BOOL_LITERAL(TRUE));
    __SET_VAR(data__->,ALARM_OUT,,__BOOL_LITERAL(TRUE));
  } else if ((__GET_VAR(data__->LATCHED,) && !(__GET_VAR(data__->ALARM_DELAY.Q,)))) {
    __SET_VAR(data__->,SHUTDOWN_CMD,,__BOOL_LITERAL(FALSE));
    __SET_VAR(data__->,ALARM_OUT,,__BOOL_LITERAL(FALSE));
  } else {
    __SET_VAR(data__->,SHUTDOWN_CMD,,__BOOL_LITERAL(FALSE));
    __SET_VAR(data__->,ALARM_OUT,,__BOOL_LITERAL(FALSE));
  };

  goto __end;

__end:
  return;
} // ESD_INTERLOCK_body__() 





