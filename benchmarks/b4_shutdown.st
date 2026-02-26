(* B4: Multi-Stage Shutdown Sequence
 * Constructs: TON (x3), INT state machine, subtraction, < comparison,
 *             arithmetic (valve_pos - 10), IF/ELSIF chain
 * Scenario: 4-stage shutdown: running(0)->cooldown(1)->depressure(2)->safe(3)
 *           Each stage has a timer; valve position decrements per stage.
 *)

PROGRAM shutdown_seq
  VAR
    (* Inputs *)
    shutdown_req   : BOOL;

    (* State *)
    stage          : INT := 0;
    valve_pos      : INT := 100;

    (* Timers - one per stage *)
    cooldown_tmr   : TON;
    depress_tmr    : TON;
    safe_tmr       : TON;

    (* Outputs *)
    heater_on      : BOOL;
    pump_on        : BOOL;
    vent_open      : BOOL;
    safe_state     : BOOL;
  END_VAR

  (* Timer activations tied to current stage *)
  cooldown_tmr(IN := (stage = 1), PT := T#10s);
  depress_tmr(IN := (stage = 2), PT := T#15s);
  safe_tmr(IN := (stage = 3), PT := T#5s);

  IF stage = 0 THEN
    (* RUNNING: normal operation *)
    heater_on := TRUE;
    pump_on := TRUE;
    vent_open := FALSE;
    safe_state := FALSE;
    IF shutdown_req THEN
      stage := 1;
      valve_pos := 100;
    END_IF;

  ELSIF stage = 1 THEN
    (* COOLDOWN: heater off, pump still on *)
    heater_on := FALSE;
    pump_on := TRUE;
    vent_open := FALSE;
    valve_pos := valve_pos - 10;
    IF valve_pos < 0 THEN
      valve_pos := 0;
    END_IF;
    IF cooldown_tmr.Q THEN
      stage := 2;
    END_IF;

  ELSIF stage = 2 THEN
    (* DEPRESSURIZE: pump off, vent open *)
    heater_on := FALSE;
    pump_on := FALSE;
    vent_open := TRUE;
    valve_pos := valve_pos - 10;
    IF valve_pos < 0 THEN
      valve_pos := 0;
    END_IF;
    IF depress_tmr.Q THEN
      stage := 3;
    END_IF;

  ELSIF stage = 3 THEN
    (* SAFE: everything off *)
    heater_on := FALSE;
    pump_on := FALSE;
    vent_open := FALSE;
    safe_state := TRUE;
    IF safe_tmr.Q THEN
      (* Remain in safe state *)
      stage := 3;
    END_IF;

  END_IF;

END_PROGRAM


CONFIGURATION shutdown_config
  RESOURCE shutdown_resource ON PLC
    TASK shutdown_task(INTERVAL := T#50ms, PRIORITY := 0);
    PROGRAM shutdown_inst WITH shutdown_task : shutdown_seq;
  END_RESOURCE
END_CONFIGURATION
