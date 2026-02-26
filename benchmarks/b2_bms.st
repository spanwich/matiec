(* B2: Burner Management Purge Sequence
 * Constructs: R_TRIG, F_TRIG, TON (x2), INT state machine, nested ELSIF,
 *             INT comparison (=, <>), BOOL assignment
 * Scenario: 5-stage burner: idle(0)->purge(1)->ignite(2)->run(3)->lockout(4)
 *)

PROGRAM bms_purge
  VAR
    (* Inputs *)
    start_cmd      : BOOL;
    flame_sensor   : BOOL;
    purge_ok       : BOOL;

    (* State *)
    stage          : INT := 0;

    (* Edge detection *)
    start_edge     : R_TRIG;
    flame_lost     : F_TRIG;

    (* Timers *)
    purge_timer    : TON;
    ignition_timer : TON;

    (* Outputs *)
    fan_on         : BOOL;
    fuel_valve     : BOOL;
    igniter_on     : BOOL;
    alarm_lockout  : BOOL;
  END_VAR

  (* Detect start command rising edge *)
  start_edge(CLK := start_cmd);

  (* Detect flame loss falling edge *)
  flame_lost(CLK := flame_sensor);

  (* Purge timer: 30s air purge before ignition *)
  purge_timer(IN := (stage = 1), PT := T#30s);

  (* Ignition timer: 5s window to establish flame *)
  ignition_timer(IN := (stage = 2), PT := T#5s);

  (* State machine *)
  IF stage = 0 THEN
    (* IDLE *)
    fan_on := FALSE;
    fuel_valve := FALSE;
    igniter_on := FALSE;
    alarm_lockout := FALSE;
    IF start_edge.Q THEN
      stage := 1;
    END_IF;

  ELSIF stage = 1 THEN
    (* PURGE: fan on, wait 30s *)
    fan_on := TRUE;
    fuel_valve := FALSE;
    igniter_on := FALSE;
    IF purge_timer.Q AND purge_ok THEN
      stage := 2;
    END_IF;

  ELSIF stage = 2 THEN
    (* IGNITE: fan + fuel + igniter, 5s window *)
    fan_on := TRUE;
    fuel_valve := TRUE;
    igniter_on := TRUE;
    IF flame_sensor THEN
      stage := 3;
    ELSIF ignition_timer.Q AND NOT flame_sensor THEN
      stage := 4;
    END_IF;

  ELSIF stage = 3 THEN
    (* RUN: fan + fuel, monitor flame *)
    fan_on := TRUE;
    fuel_valve := TRUE;
    igniter_on := FALSE;
    IF flame_lost.Q THEN
      stage := 4;
    END_IF;

  ELSIF stage = 4 THEN
    (* LOCKOUT: everything off, alarm *)
    fan_on := FALSE;
    fuel_valve := FALSE;
    igniter_on := FALSE;
    alarm_lockout := TRUE;
    (* Require operator reset - not modeled here *)

  END_IF;

END_PROGRAM


CONFIGURATION bms_config
  RESOURCE bms_resource ON PLC
    TASK bms_task(INTERVAL := T#20ms, PRIORITY := 0);
    PROGRAM bms_inst WITH bms_task : bms_purge;
  END_RESOURCE
END_CONFIGURATION
