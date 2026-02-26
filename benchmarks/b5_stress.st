(* B5: WCET Stress - Synthetic Benchmark
   Exercises ALL LowPLC constructs: SR, RS, CTU, CTD, CTUD,
   TON, TOF, R_TRIG, F_TRIG, FOR loop, nested IF/ELSIF/ELSE,
   all comparison ops: EQ, NE, LT, GT, LE, GE,
   all arithmetic ops: ADD, SUB, MUL,
   all boolean ops: AND, OR, NOT, XOR.
   Uses inp_ prefix to avoid IN keyword collision. *)

PROGRAM stress_test
  VAR
    (* Inputs — use inp_ prefix since IN is a keyword *)
    inp_a          : BOOL;
    inp_b          : BOOL;
    inp_c          : BOOL;
    inp_d          : BOOL;
    inp_e          : INT := 0;
    inp_f          : INT := 50;
    inp_g          : INT := 100;
    inp_h          : TIME := T#2s;

    (* FB instances — all 9 standard FB types *)
    fb_sr          : SR;
    fb_rs          : RS;
    fb_ctu         : CTU;
    fb_ctd         : CTD;
    fb_ctud        : CTUD;
    fb_ton         : TON;
    fb_tof         : TOF;
    fb_rtrig       : R_TRIG;
    fb_ftrig       : F_TRIG;

    (* Working variables *)
    bool_res       : BOOL;
    int_res        : INT := 0;
    sum_acc        : INT := 0;
    idx            : INT;
    cmp_eq         : BOOL;
    cmp_ne         : BOOL;
    cmp_lt         : BOOL;
    cmp_gt         : BOOL;
    cmp_le         : BOOL;
    cmp_ge         : BOOL;

    (* Outputs *)
    out_alarm      : BOOL;
    out_count      : INT;
    out_timer_q    : BOOL;
    out_result     : INT;
  END_VAR

  (* --- Edge detectors --- *)
  fb_rtrig(CLK := inp_a);
  fb_ftrig(CLK := inp_b);

  (* --- Bistable latches --- *)
  fb_sr(S1 := fb_rtrig.Q, R := inp_c);
  fb_rs(S := inp_a, R1 := inp_d);

  (* --- Counters --- *)
  fb_ctu(CU := fb_rtrig.Q, R := inp_c, PV := inp_f);
  fb_ctd(CD := fb_ftrig.Q, LD := inp_d, PV := inp_g);
  fb_ctud(CU := inp_a, CD := inp_b, R := inp_c, LD := inp_d, PV := inp_f);

  (* --- Timers --- *)
  fb_ton(IN := fb_sr.Q1, PT := inp_h);
  fb_tof(IN := fb_rs.Q1, PT := T#1s);

  (* --- All comparison operators --- *)
  cmp_eq := (inp_e = inp_f);
  cmp_ne := (inp_e <> inp_f);
  cmp_lt := (inp_e < inp_f);
  cmp_gt := (inp_e > inp_f);
  cmp_le := (inp_e <= inp_f);
  cmp_ge := (inp_e >= inp_f);

  (* --- All boolean operators --- *)
  bool_res := (inp_a AND inp_b) OR (NOT inp_c) XOR inp_d;

  (* --- Arithmetic operators --- *)
  int_res := (inp_e + inp_f) - inp_g;
  int_res := int_res * 2;

  (* --- FOR loop with accumulator --- *)
  sum_acc := 0;
  FOR idx := 0 TO 9 DO
    sum_acc := sum_acc + idx;
  END_FOR;

  (* --- Nested IF/ELSIF/ELSE --- *)
  IF fb_ton.Q AND cmp_gt THEN
    out_alarm := TRUE;
    out_result := sum_acc + int_res;
  ELSIF fb_ctu.Q OR cmp_eq THEN
    out_alarm := FALSE;
    out_result := fb_ctu.CV + fb_ctd.CV;
  ELSIF cmp_le AND NOT cmp_ne THEN
    out_alarm := bool_res;
    out_result := fb_ctud.CV;
  ELSE
    out_alarm := FALSE;
    out_result := 0;
  END_IF;

  out_count := fb_ctu.CV;
  out_timer_q := fb_ton.Q;

END_PROGRAM


CONFIGURATION stress_config
  RESOURCE stress_resource ON PLC
    TASK stress_task(INTERVAL := T#10ms, PRIORITY := 0);
    PROGRAM stress_inst WITH stress_task : stress_test;
  END_RESOURCE
END_CONFIGURATION
