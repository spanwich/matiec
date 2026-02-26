/*
 * B5: STRESS_TEST — WCET Stress Test benchmark
 *
 * Worst-case: all 9 FBs active, all expression types exercised,
 * FOR loop 10 iterations, nested IF/ELSIF/ELSE.
 */
#define BENCH_NAME "B5_STRESS"

#include "iec_std_lib.h"
#include "iec_std_FB.h"
#include "POUS.h"
#include "POUS.c"

static STRESS_TEST_data__ plc_state;

void plc_init_wrapper(void) {
    STRESS_TEST_init__(&plc_state, 0);
}

void set_worst_case_inputs(void) {
    /* Boolean inputs */
    __SET_VAR(plc_state.,INP_A,,__BOOL_LITERAL(TRUE));
    __SET_VAR(plc_state.,INP_B,,__BOOL_LITERAL(TRUE));
    __SET_VAR(plc_state.,INP_C,,__BOOL_LITERAL(FALSE));
    __SET_VAR(plc_state.,INP_D,,__BOOL_LITERAL(TRUE));

    /* Integer inputs */
    __SET_VAR(plc_state.,INP_E,,50);
    __SET_VAR(plc_state.,INP_F,,50);
    __SET_VAR(plc_state.,INP_G,,100);

    /* Time input — use direct access since compound literal comma breaks macro */
    plc_state.INP_H.value = (TIME){2, 0};  /* T#2s */

    /* SR: set dominant active */
    plc_state.FB_SR.S1.value = __BOOL_LITERAL(TRUE);
    plc_state.FB_SR.R.value  = __BOOL_LITERAL(FALSE);

    /* RS: reset dominant active */
    plc_state.FB_RS.S.value  = __BOOL_LITERAL(TRUE);
    plc_state.FB_RS.R1.value = __BOOL_LITERAL(FALSE);

    /* R_TRIG: edge detect — M=FALSE, CLK going TRUE */
    plc_state.FB_RTRIG.M.value   = __BOOL_LITERAL(FALSE);
    plc_state.FB_RTRIG.CLK.value = __BOOL_LITERAL(TRUE);

    /* F_TRIG: edge detect — M=TRUE, CLK going FALSE */
    plc_state.FB_FTRIG.M.value   = __BOOL_LITERAL(TRUE);
    plc_state.FB_FTRIG.CLK.value = __BOOL_LITERAL(FALSE);

    /* CTU: edge active, near threshold */
    plc_state.FB_CTU.CU.value = __BOOL_LITERAL(TRUE);
    plc_state.FB_CTU.R.value  = __BOOL_LITERAL(FALSE);
    plc_state.FB_CTU.PV.value = 100;
    plc_state.FB_CTU.CV.value = 99;
    plc_state.FB_CTU.CU_T.M.value = __BOOL_LITERAL(FALSE);

    /* CTD: edge active, near threshold */
    plc_state.FB_CTD.CD.value = __BOOL_LITERAL(TRUE);
    plc_state.FB_CTD.LD.value = __BOOL_LITERAL(FALSE);
    plc_state.FB_CTD.PV.value = 100;
    plc_state.FB_CTD.CV.value = 1;
    plc_state.FB_CTD.CD_T.M.value = __BOOL_LITERAL(FALSE);

    /* CTUD: both edges active */
    plc_state.FB_CTUD.CU.value = __BOOL_LITERAL(TRUE);
    plc_state.FB_CTUD.CD.value = __BOOL_LITERAL(TRUE);
    plc_state.FB_CTUD.R.value  = __BOOL_LITERAL(FALSE);
    plc_state.FB_CTUD.LD.value = __BOOL_LITERAL(FALSE);
    plc_state.FB_CTUD.PV.value = 100;
    plc_state.FB_CTUD.CV.value = 50;
    plc_state.FB_CTUD.CU_T.M.value = __BOOL_LITERAL(FALSE);
    plc_state.FB_CTUD.CD_T.M.value = __BOOL_LITERAL(FALSE);

    /* TON: mid-count timing path (STATE=1) */
    plc_state.FB_TON.STATE.value = 1;
    plc_state.FB_TON.IN.value    = __BOOL_LITERAL(TRUE);
    plc_state.FB_TON.PREV_IN.value = __BOOL_LITERAL(TRUE);
    plc_state.FB_TON.START_TIME.value = (TIME){0, 0};
    plc_state.FB_TON.PT.value    = (TIME){2, 0};

    /* TOF: mid-count timing path (STATE=1) */
    plc_state.FB_TOF.STATE.value = 1;
    plc_state.FB_TOF.IN.value    = __BOOL_LITERAL(FALSE);
    plc_state.FB_TOF.PREV_IN.value = __BOOL_LITERAL(FALSE);
    plc_state.FB_TOF.START_TIME.value = (TIME){0, 0};
    plc_state.FB_TOF.PT.value    = (TIME){1, 0};

    __CURRENT_TIME = (TIME){0, 500000000};
}

void plc_body_wrapper(void) {
    STRESS_TEST_body__(&plc_state);
}

#include "../wrapper_template.c"
