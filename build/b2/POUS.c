void BMS_PURGE_init__(BMS_PURGE_data__ *data__, BOOL retain) {
  __INIT_VAR(data__->START_CMD,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->FLAME_SENSOR,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->PURGE_OK,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->STAGE,0,retain)
  R_TRIG_init__(&data__->START_EDGE,retain);
  F_TRIG_init__(&data__->FLAME_LOST,retain);
  TON_init__(&data__->PURGE_TIMER,retain);
  TON_init__(&data__->IGNITION_TIMER,retain);
  __INIT_VAR(data__->FAN_ON,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->FUEL_VALVE,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->IGNITER_ON,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->ALARM_LOCKOUT,__BOOL_LITERAL(FALSE),retain)
}

// Code part
void BMS_PURGE_body__(BMS_PURGE_data__ *data__) {
  // Initialise TEMP variables

  __SET_VAR(data__->START_EDGE.,CLK,,__GET_VAR(data__->START_CMD,));
  R_TRIG_body__(&data__->START_EDGE);
  __SET_VAR(data__->FLAME_LOST.,CLK,,__GET_VAR(data__->FLAME_SENSOR,));
  F_TRIG_body__(&data__->FLAME_LOST);
  __SET_VAR(data__->PURGE_TIMER.,IN,,(__GET_VAR(data__->STAGE,) == 1));
  __SET_VAR(data__->PURGE_TIMER.,PT,,__time_to_timespec(1, 0, 30, 0, 0, 0));
  TON_body__(&data__->PURGE_TIMER);
  __SET_VAR(data__->IGNITION_TIMER.,IN,,(__GET_VAR(data__->STAGE,) == 2));
  __SET_VAR(data__->IGNITION_TIMER.,PT,,__time_to_timespec(1, 0, 5, 0, 0, 0));
  TON_body__(&data__->IGNITION_TIMER);
  if ((__GET_VAR(data__->STAGE,) == 0)) {
    __SET_VAR(data__->,FAN_ON,,__BOOL_LITERAL(FALSE));
    __SET_VAR(data__->,FUEL_VALVE,,__BOOL_LITERAL(FALSE));
    __SET_VAR(data__->,IGNITER_ON,,__BOOL_LITERAL(FALSE));
    __SET_VAR(data__->,ALARM_LOCKOUT,,__BOOL_LITERAL(FALSE));
    if (__GET_VAR(data__->START_EDGE.Q,)) {
      __SET_VAR(data__->,STAGE,,1);
    };
  } else if ((__GET_VAR(data__->STAGE,) == 1)) {
    __SET_VAR(data__->,FAN_ON,,__BOOL_LITERAL(TRUE));
    __SET_VAR(data__->,FUEL_VALVE,,__BOOL_LITERAL(FALSE));
    __SET_VAR(data__->,IGNITER_ON,,__BOOL_LITERAL(FALSE));
    if ((__GET_VAR(data__->PURGE_TIMER.Q,) && __GET_VAR(data__->PURGE_OK,))) {
      __SET_VAR(data__->,STAGE,,2);
    };
  } else if ((__GET_VAR(data__->STAGE,) == 2)) {
    __SET_VAR(data__->,FAN_ON,,__BOOL_LITERAL(TRUE));
    __SET_VAR(data__->,FUEL_VALVE,,__BOOL_LITERAL(TRUE));
    __SET_VAR(data__->,IGNITER_ON,,__BOOL_LITERAL(TRUE));
    if (__GET_VAR(data__->FLAME_SENSOR,)) {
      __SET_VAR(data__->,STAGE,,3);
    } else if ((__GET_VAR(data__->IGNITION_TIMER.Q,) && !(__GET_VAR(data__->FLAME_SENSOR,)))) {
      __SET_VAR(data__->,STAGE,,4);
    };
  } else if ((__GET_VAR(data__->STAGE,) == 3)) {
    __SET_VAR(data__->,FAN_ON,,__BOOL_LITERAL(TRUE));
    __SET_VAR(data__->,FUEL_VALVE,,__BOOL_LITERAL(TRUE));
    __SET_VAR(data__->,IGNITER_ON,,__BOOL_LITERAL(FALSE));
    if (__GET_VAR(data__->FLAME_LOST.Q,)) {
      __SET_VAR(data__->,STAGE,,4);
    };
  } else if ((__GET_VAR(data__->STAGE,) == 4)) {
    __SET_VAR(data__->,FAN_ON,,__BOOL_LITERAL(FALSE));
    __SET_VAR(data__->,FUEL_VALVE,,__BOOL_LITERAL(FALSE));
    __SET_VAR(data__->,IGNITER_ON,,__BOOL_LITERAL(FALSE));
    __SET_VAR(data__->,ALARM_LOCKOUT,,__BOOL_LITERAL(TRUE));
  };

  goto __end;

__end:
  return;
} // BMS_PURGE_body__() 





