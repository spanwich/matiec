(* B3: Conveyor with Part Counter
 * Constructs: CTU, R_TRIG, FOR loop (bounded), +, >, >=,
 *             boolean latch, INT arithmetic, BOOL assignment
 * Scenario: Conveyor counts parts via CTU. FOR loop computes batch totals.
 *)

PROGRAM conveyor_count
  VAR
    (* Inputs *)
    part_detect    : BOOL;
    batch_reset    : BOOL;
    run_cmd        : BOOL;
    batch_size     : INT := 10;

    (* Internal *)
    part_edge      : R_TRIG;
    part_counter   : CTU;
    motor_latch    : BOOL;
    batch_total    : INT := 0;
    idx            : INT;

    (* Outputs *)
    motor_on       : BOOL;
    batch_complete : BOOL;
    total_count    : INT;
  END_VAR

  (* Detect rising edge of part sensor *)
  part_edge(CLK := part_detect);

  (* Count parts: reset on batch_reset, target = batch_size *)
  part_counter(CU := part_edge.Q, R := batch_reset, PV := batch_size);

  (* Motor latch: start on run_cmd, stop on batch complete *)
  IF run_cmd THEN
    motor_latch := TRUE;
  END_IF;
  IF part_counter.Q THEN
    motor_latch := FALSE;
  END_IF;
  motor_on := motor_latch;

  (* Batch complete when counter reaches target *)
  batch_complete := part_counter.Q;
  total_count := part_counter.CV;

  (* Accumulate batch totals using FOR loop (5 batches) *)
  IF batch_complete THEN
    batch_total := 0;
    FOR idx := 1 TO 5 DO
      batch_total := batch_total + batch_size;
    END_FOR;
  END_IF;

END_PROGRAM


CONFIGURATION conveyor_config
  RESOURCE conveyor_resource ON PLC
    TASK conveyor_task(INTERVAL := T#10ms, PRIORITY := 0);
    PROGRAM conveyor_inst WITH conveyor_task : conveyor_count;
  END_RESOURCE
END_CONFIGURATION
