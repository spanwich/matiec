void SHUTDOWN_SEQ_init__(SHUTDOWN_SEQ_data__ *data__, BOOL retain) {
  __INIT_VAR(data__->SHUTDOWN_REQ,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->STAGE,0,retain)
  __INIT_VAR(data__->VALVE_POS,100,retain)
  TON_init__(&data__->COOLDOWN_TMR,retain);
  TON_init__(&data__->DEPRESS_TMR,retain);
  TON_init__(&data__->SAFE_TMR,retain);
  __INIT_VAR(data__->HEATER_ON,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->PUMP_ON,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->VENT_OPEN,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->SAFE_STATE,__BOOL_LITERAL(FALSE),retain)
}

// Code part
void SHUTDOWN_SEQ_body__(SHUTDOWN_SEQ_data__ *data__) {
  // Initialise TEMP variables

  __SET_VAR(data__->COOLDOWN_TMR.,IN,,(__GET_VAR(data__->STAGE,) == 1));
  __SET_VAR(data__->COOLDOWN_TMR.,PT,,__time_to_timespec(1, 0, 10, 0, 0, 0));
  TON_body__(&data__->COOLDOWN_TMR);
  __SET_VAR(data__->DEPRESS_TMR.,IN,,(__GET_VAR(data__->STAGE,) == 2));
  __SET_VAR(data__->DEPRESS_TMR.,PT,,__time_to_timespec(1, 0, 15, 0, 0, 0));
  TON_body__(&data__->DEPRESS_TMR);
  __SET_VAR(data__->SAFE_TMR.,IN,,(__GET_VAR(data__->STAGE,) == 3));
  __SET_VAR(data__->SAFE_TMR.,PT,,__time_to_timespec(1, 0, 5, 0, 0, 0));
  TON_body__(&data__->SAFE_TMR);
  if ((__GET_VAR(data__->STAGE,) == 0)) {
    __SET_VAR(data__->,HEATER_ON,,__BOOL_LITERAL(TRUE));
    __SET_VAR(data__->,PUMP_ON,,__BOOL_LITERAL(TRUE));
    __SET_VAR(data__->,VENT_OPEN,,__BOOL_LITERAL(FALSE));
    __SET_VAR(data__->,SAFE_STATE,,__BOOL_LITERAL(FALSE));
    if (__GET_VAR(data__->SHUTDOWN_REQ,)) {
      __SET_VAR(data__->,STAGE,,1);
      __SET_VAR(data__->,VALVE_POS,,100);
    };
  } else if ((__GET_VAR(data__->STAGE,) == 1)) {
    __SET_VAR(data__->,HEATER_ON,,__BOOL_LITERAL(FALSE));
    __SET_VAR(data__->,PUMP_ON,,__BOOL_LITERAL(TRUE));
    __SET_VAR(data__->,VENT_OPEN,,__BOOL_LITERAL(FALSE));
    __SET_VAR(data__->,VALVE_POS,,(__GET_VAR(data__->VALVE_POS,) - 10));
    if ((__GET_VAR(data__->VALVE_POS,) < 0)) {
      __SET_VAR(data__->,VALVE_POS,,0);
    };
    if (__GET_VAR(data__->COOLDOWN_TMR.Q,)) {
      __SET_VAR(data__->,STAGE,,2);
    };
  } else if ((__GET_VAR(data__->STAGE,) == 2)) {
    __SET_VAR(data__->,HEATER_ON,,__BOOL_LITERAL(FALSE));
    __SET_VAR(data__->,PUMP_ON,,__BOOL_LITERAL(FALSE));
    __SET_VAR(data__->,VENT_OPEN,,__BOOL_LITERAL(TRUE));
    __SET_VAR(data__->,VALVE_POS,,(__GET_VAR(data__->VALVE_POS,) - 10));
    if ((__GET_VAR(data__->VALVE_POS,) < 0)) {
      __SET_VAR(data__->,VALVE_POS,,0);
    };
    if (__GET_VAR(data__->DEPRESS_TMR.Q,)) {
      __SET_VAR(data__->,STAGE,,3);
    };
  } else if ((__GET_VAR(data__->STAGE,) == 3)) {
    __SET_VAR(data__->,HEATER_ON,,__BOOL_LITERAL(FALSE));
    __SET_VAR(data__->,PUMP_ON,,__BOOL_LITERAL(FALSE));
    __SET_VAR(data__->,VENT_OPEN,,__BOOL_LITERAL(FALSE));
    __SET_VAR(data__->,SAFE_STATE,,__BOOL_LITERAL(TRUE));
    if (__GET_VAR(data__->SAFE_TMR.Q,)) {
      __SET_VAR(data__->,STAGE,,3);
    };
  };

  goto __end;

__end:
  return;
} // SHUTDOWN_SEQ_body__() 





