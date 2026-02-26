void STRESS_TEST_init__(STRESS_TEST_data__ *data__, BOOL retain) {
  __INIT_VAR(data__->INP_A,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->INP_B,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->INP_C,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->INP_D,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->INP_E,0,retain)
  __INIT_VAR(data__->INP_F,50,retain)
  __INIT_VAR(data__->INP_G,100,retain)
  __INIT_VAR(data__->INP_H,__time_to_timespec(1, 0, 2, 0, 0, 0),retain)
  SR_init__(&data__->FB_SR,retain);
  RS_init__(&data__->FB_RS,retain);
  CTU_init__(&data__->FB_CTU,retain);
  CTD_init__(&data__->FB_CTD,retain);
  CTUD_init__(&data__->FB_CTUD,retain);
  TON_init__(&data__->FB_TON,retain);
  TOF_init__(&data__->FB_TOF,retain);
  R_TRIG_init__(&data__->FB_RTRIG,retain);
  F_TRIG_init__(&data__->FB_FTRIG,retain);
  __INIT_VAR(data__->BOOL_RES,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->INT_RES,0,retain)
  __INIT_VAR(data__->SUM_ACC,0,retain)
  __INIT_VAR(data__->IDX,0,retain)
  __INIT_VAR(data__->CMP_EQ,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->CMP_NE,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->CMP_LT,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->CMP_GT,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->CMP_LE,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->CMP_GE,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->OUT_ALARM,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->OUT_COUNT,0,retain)
  __INIT_VAR(data__->OUT_TIMER_Q,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->OUT_RESULT,0,retain)
}

// Code part
void STRESS_TEST_body__(STRESS_TEST_data__ *data__) {
  // Initialise TEMP variables

  __SET_VAR(data__->FB_RTRIG.,CLK,,__GET_VAR(data__->INP_A,));
  R_TRIG_body__(&data__->FB_RTRIG);
  __SET_VAR(data__->FB_FTRIG.,CLK,,__GET_VAR(data__->INP_B,));
  F_TRIG_body__(&data__->FB_FTRIG);
  __SET_VAR(data__->FB_SR.,S1,,__GET_VAR(data__->FB_RTRIG.Q,));
  __SET_VAR(data__->FB_SR.,R,,__GET_VAR(data__->INP_C,));
  SR_body__(&data__->FB_SR);
  __SET_VAR(data__->FB_RS.,S,,__GET_VAR(data__->INP_A,));
  __SET_VAR(data__->FB_RS.,R1,,__GET_VAR(data__->INP_D,));
  RS_body__(&data__->FB_RS);
  __SET_VAR(data__->FB_CTU.,CU,,__GET_VAR(data__->FB_RTRIG.Q,));
  __SET_VAR(data__->FB_CTU.,R,,__GET_VAR(data__->INP_C,));
  __SET_VAR(data__->FB_CTU.,PV,,__GET_VAR(data__->INP_F,));
  CTU_body__(&data__->FB_CTU);
  __SET_VAR(data__->FB_CTD.,CD,,__GET_VAR(data__->FB_FTRIG.Q,));
  __SET_VAR(data__->FB_CTD.,LD,,__GET_VAR(data__->INP_D,));
  __SET_VAR(data__->FB_CTD.,PV,,__GET_VAR(data__->INP_G,));
  CTD_body__(&data__->FB_CTD);
  __SET_VAR(data__->FB_CTUD.,CU,,__GET_VAR(data__->INP_A,));
  __SET_VAR(data__->FB_CTUD.,CD,,__GET_VAR(data__->INP_B,));
  __SET_VAR(data__->FB_CTUD.,R,,__GET_VAR(data__->INP_C,));
  __SET_VAR(data__->FB_CTUD.,LD,,__GET_VAR(data__->INP_D,));
  __SET_VAR(data__->FB_CTUD.,PV,,__GET_VAR(data__->INP_F,));
  CTUD_body__(&data__->FB_CTUD);
  __SET_VAR(data__->FB_TON.,IN,,__GET_VAR(data__->FB_SR.Q1,));
  __SET_VAR(data__->FB_TON.,PT,,__GET_VAR(data__->INP_H,));
  TON_body__(&data__->FB_TON);
  __SET_VAR(data__->FB_TOF.,IN,,__GET_VAR(data__->FB_RS.Q1,));
  __SET_VAR(data__->FB_TOF.,PT,,__time_to_timespec(1, 0, 1, 0, 0, 0));
  TOF_body__(&data__->FB_TOF);
  __SET_VAR(data__->,CMP_EQ,,(__GET_VAR(data__->INP_E,) == __GET_VAR(data__->INP_F,)));
  __SET_VAR(data__->,CMP_NE,,(__GET_VAR(data__->INP_E,) != __GET_VAR(data__->INP_F,)));
  __SET_VAR(data__->,CMP_LT,,(__GET_VAR(data__->INP_E,) < __GET_VAR(data__->INP_F,)));
  __SET_VAR(data__->,CMP_GT,,(__GET_VAR(data__->INP_E,) > __GET_VAR(data__->INP_F,)));
  __SET_VAR(data__->,CMP_LE,,(__GET_VAR(data__->INP_E,) <= __GET_VAR(data__->INP_F,)));
  __SET_VAR(data__->,CMP_GE,,(__GET_VAR(data__->INP_E,) >= __GET_VAR(data__->INP_F,)));
  __SET_VAR(data__->,BOOL_RES,,((__GET_VAR(data__->INP_A,) && __GET_VAR(data__->INP_B,)) || ((!(__GET_VAR(data__->INP_C,)) && !__GET_VAR(data__->INP_D,)) || (!!(__GET_VAR(data__->INP_C,)) && __GET_VAR(data__->INP_D,)))));
  __SET_VAR(data__->,INT_RES,,((__GET_VAR(data__->INP_E,) + __GET_VAR(data__->INP_F,)) - __GET_VAR(data__->INP_G,)));
  __SET_VAR(data__->,INT_RES,,(__GET_VAR(data__->INT_RES,) * 2));
  __SET_VAR(data__->,SUM_ACC,,0);
  /* FOR ... */
  __SET_VAR(data__->,IDX,,0);
  {
    int __do_increment = 0;
    while(1) {
      if(__do_increment){
        /* BY ... (of FOR loop) */
        __SET_VAR(data__->,IDX,,(__GET_VAR(data__->IDX,) + 1));
      } else __do_increment = 1;
      if(__GET_VAR(data__->IDX,) <= 9      ){
        __SET_VAR(data__->,SUM_ACC,,(__GET_VAR(data__->SUM_ACC,) + __GET_VAR(data__->IDX,)));
      }else break;
    }
  } /* END_FOR */;
  if ((__GET_VAR(data__->FB_TON.Q,) && __GET_VAR(data__->CMP_GT,))) {
    __SET_VAR(data__->,OUT_ALARM,,__BOOL_LITERAL(TRUE));
    __SET_VAR(data__->,OUT_RESULT,,(__GET_VAR(data__->SUM_ACC,) + __GET_VAR(data__->INT_RES,)));
  } else if ((__GET_VAR(data__->FB_CTU.Q,) || __GET_VAR(data__->CMP_EQ,))) {
    __SET_VAR(data__->,OUT_ALARM,,__BOOL_LITERAL(FALSE));
    __SET_VAR(data__->,OUT_RESULT,,(__GET_VAR(data__->FB_CTU.CV,) + __GET_VAR(data__->FB_CTD.CV,)));
  } else if ((__GET_VAR(data__->CMP_LE,) && !(__GET_VAR(data__->CMP_NE,)))) {
    __SET_VAR(data__->,OUT_ALARM,,__GET_VAR(data__->BOOL_RES,));
    __SET_VAR(data__->,OUT_RESULT,,__GET_VAR(data__->FB_CTUD.CV,));
  } else {
    __SET_VAR(data__->,OUT_ALARM,,__BOOL_LITERAL(FALSE));
    __SET_VAR(data__->,OUT_RESULT,,0);
  };
  __SET_VAR(data__->,OUT_COUNT,,__GET_VAR(data__->FB_CTU.CV,));
  __SET_VAR(data__->,OUT_TIMER_Q,,__GET_VAR(data__->FB_TON.Q,));

  goto __end;

__end:
  return;
} // STRESS_TEST_body__() 





