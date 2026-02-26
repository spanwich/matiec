/*
 * B1: ESD_INTERLOCK — Emergency Shutdown Interlock benchmark
 *
 * Worst-case: all trip sensors TRUE, TON in mid-count (STATE=1, timer
 * running at 500ms of 1s PT), LATCHED=TRUE, exercising full IF/ELSIF/ELSE.
 */
#define BENCH_NAME "B1_ESD"

#include "iec_std_lib.h"
#include "iec_std_FB.h"
#include "POUS.h"
#include "POUS.c"

static ESD_INTERLOCK_data__ plc_state;

void plc_init_wrapper(void) {
    ESD_INTERLOCK_init__(&plc_state, 0);
}

void set_worst_case_inputs(void) {
    /* All trip sensors active */
    __SET_VAR(plc_state.,PRESSURE_HIGH,,__BOOL_LITERAL(TRUE));
    __SET_VAR(plc_state.,TEMP_HIGH,,__BOOL_LITERAL(TRUE));
    __SET_VAR(plc_state.,FLOW_LOW,,__BOOL_LITERAL(TRUE));
    __SET_VAR(plc_state.,MANUAL_RESET,,__BOOL_LITERAL(FALSE));

    /* TON ALARM_DELAY: mid-count at 500ms of 1s, longest timing path */
    plc_state.ALARM_DELAY.STATE.value = 1;
    plc_state.ALARM_DELAY.IN.value    = __BOOL_LITERAL(TRUE);
    plc_state.ALARM_DELAY.PREV_IN.value = __BOOL_LITERAL(TRUE);
    plc_state.ALARM_DELAY.START_TIME.value = (TIME){0, 0};
    plc_state.ALARM_DELAY.PT.value    = (TIME){1, 0};  /* 1 second */

    /* Simulated current time: 500ms into the timer */
    __CURRENT_TIME = (TIME){0, 500000000};
}

void plc_body_wrapper(void) {
    ESD_INTERLOCK_body__(&plc_state);
}

#include "../wrapper_template.c"
