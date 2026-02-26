/*
 * B3: CONVEYOR_COUNT — Conveyor with Part Counter benchmark
 *
 * Worst-case: R_TRIG edge active, CTU near threshold (CV=9, PV=10),
 * FOR loop 5 iterations, motor latch logic.
 */
#define BENCH_NAME "B3_CONVEYOR"

#include "iec_std_lib.h"
#include "iec_std_FB.h"
#include "POUS.h"
#include "POUS.c"

static CONVEYOR_COUNT_data__ plc_state;

void plc_init_wrapper(void) {
    CONVEYOR_COUNT_init__(&plc_state, 0);
}

void set_worst_case_inputs(void) {
    /* Inputs */
    __SET_VAR(plc_state.,PART_DETECT,,__BOOL_LITERAL(TRUE));
    __SET_VAR(plc_state.,BATCH_RESET,,__BOOL_LITERAL(FALSE));
    __SET_VAR(plc_state.,RUN_CMD,,__BOOL_LITERAL(TRUE));
    __SET_VAR(plc_state.,BATCH_SIZE,,10);

    /* R_TRIG PART_EDGE: M=FALSE with CLK going TRUE → rising edge */
    plc_state.PART_EDGE.M.value  = __BOOL_LITERAL(FALSE);
    plc_state.PART_EDGE.CLK.value = __BOOL_LITERAL(TRUE);

    /* CTU PART_COUNTER: CV near threshold (9 of PV=10) */
    plc_state.PART_COUNTER.CV.value  = 9;
    plc_state.PART_COUNTER.PV.value  = 10;
    plc_state.PART_COUNTER.CU.value  = __BOOL_LITERAL(TRUE);
    plc_state.PART_COUNTER.R.value   = __BOOL_LITERAL(FALSE);
    /* Internal R_TRIG for CU edge: M=FALSE → will fire edge */
    plc_state.PART_COUNTER.CU_T.M.value = __BOOL_LITERAL(FALSE);
}

void plc_body_wrapper(void) {
    CONVEYOR_COUNT_body__(&plc_state);
}

#include "../wrapper_template.c"
