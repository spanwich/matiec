/*
 * B2: BMS_PURGE — Burner Management Purge Sequence benchmark
 *
 * Worst-case: STAGE=2 (ignite — longest ELSIF chain, 2 TON bodies,
 * R_TRIG + F_TRIG edge detection).  IGNITION_TIMER in mid-count.
 */
#define BENCH_NAME "B2_BMS"

#include "iec_std_lib.h"
#include "iec_std_FB.h"
#include "POUS.h"
#include "POUS.c"

static BMS_PURGE_data__ plc_state;

void plc_init_wrapper(void) {
    BMS_PURGE_init__(&plc_state, 0);
}

void set_worst_case_inputs(void) {
    /* Stage 2 = ignite (longest branch) */
    __SET_VAR(plc_state.,STAGE,,2);
    __SET_VAR(plc_state.,START_CMD,,__BOOL_LITERAL(TRUE));
    __SET_VAR(plc_state.,FLAME_SENSOR,,__BOOL_LITERAL(FALSE));
    __SET_VAR(plc_state.,PURGE_OK,,__BOOL_LITERAL(TRUE));

    /* R_TRIG START_EDGE: set M for edge detection */
    plc_state.START_EDGE.M.value  = __BOOL_LITERAL(FALSE);
    plc_state.START_EDGE.CLK.value = __BOOL_LITERAL(TRUE);

    /* F_TRIG FLAME_LOST: set M for edge detection */
    plc_state.FLAME_LOST.M.value  = __BOOL_LITERAL(TRUE);
    plc_state.FLAME_LOST.CLK.value = __BOOL_LITERAL(FALSE);

    /* PURGE_TIMER: stage != 1, so IN=FALSE → resets */
    plc_state.PURGE_TIMER.STATE.value = 0;
    plc_state.PURGE_TIMER.IN.value    = __BOOL_LITERAL(FALSE);
    plc_state.PURGE_TIMER.PREV_IN.value = __BOOL_LITERAL(FALSE);

    /* IGNITION_TIMER: stage=2, IN=TRUE → mid-count timing path */
    plc_state.IGNITION_TIMER.STATE.value = 1;
    plc_state.IGNITION_TIMER.IN.value    = __BOOL_LITERAL(TRUE);
    plc_state.IGNITION_TIMER.PREV_IN.value = __BOOL_LITERAL(TRUE);
    plc_state.IGNITION_TIMER.START_TIME.value = (TIME){0, 0};
    plc_state.IGNITION_TIMER.PT.value    = (TIME){5, 0};  /* 5 seconds */

    __CURRENT_TIME = (TIME){0, 500000000};
}

void plc_body_wrapper(void) {
    BMS_PURGE_body__(&plc_state);
}

#include "../wrapper_template.c"
