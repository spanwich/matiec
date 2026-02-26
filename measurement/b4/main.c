/*
 * B4: SHUTDOWN_SEQ — Multi-Stage Shutdown Sequence benchmark
 *
 * Worst-case: STAGE=1 (cooldown — valve subtraction + clamp),
 * COOLDOWN_TMR in mid-count, other TONs in reset state but still
 * execute their bodies.
 */
#define BENCH_NAME "B4_SHUTDOWN"

#include "iec_std_lib.h"
#include "iec_std_FB.h"
#include "POUS.h"
#include "POUS.c"

static SHUTDOWN_SEQ_data__ plc_state;

void plc_init_wrapper(void) {
    SHUTDOWN_SEQ_init__(&plc_state, 0);
}

void set_worst_case_inputs(void) {
    /* Inputs */
    __SET_VAR(plc_state.,SHUTDOWN_REQ,,__BOOL_LITERAL(TRUE));
    __SET_VAR(plc_state.,STAGE,,1);
    __SET_VAR(plc_state.,VALVE_POS,,100);

    /* COOLDOWN_TMR: STATE=1, mid-count timing path */
    plc_state.COOLDOWN_TMR.STATE.value = 1;
    plc_state.COOLDOWN_TMR.IN.value    = __BOOL_LITERAL(TRUE);
    plc_state.COOLDOWN_TMR.PREV_IN.value = __BOOL_LITERAL(TRUE);
    plc_state.COOLDOWN_TMR.START_TIME.value = (TIME){0, 0};
    plc_state.COOLDOWN_TMR.PT.value    = (TIME){10, 0};  /* 10 seconds */

    /* DEPRESS_TMR: STATE=0 (not active, but body still executes) */
    plc_state.DEPRESS_TMR.STATE.value = 0;
    plc_state.DEPRESS_TMR.IN.value    = __BOOL_LITERAL(FALSE);
    plc_state.DEPRESS_TMR.PREV_IN.value = __BOOL_LITERAL(FALSE);

    /* SAFE_TMR: STATE=0 */
    plc_state.SAFE_TMR.STATE.value = 0;
    plc_state.SAFE_TMR.IN.value    = __BOOL_LITERAL(FALSE);
    plc_state.SAFE_TMR.PREV_IN.value = __BOOL_LITERAL(FALSE);

    __CURRENT_TIME = (TIME){0, 500000000};
}

void plc_body_wrapper(void) {
    SHUTDOWN_SEQ_body__(&plc_state);
}

#include "../wrapper_template.c"
