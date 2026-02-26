(* B1: Emergency Shutdown Interlock
 * Constructs: AND, OR, NOT, IF/ELSIF/ELSE, SR, TON, BOOL assignment
 * Scenario: Three trip sensors trigger an SR-latched alarm with TON delay.
 *           Manual acknowledge resets the latch.
 *)

PROGRAM esd_interlock
  VAR
    (* Inputs *)
    pressure_high  : BOOL;
    temp_high      : BOOL;
    flow_low       : BOOL;
    manual_reset   : BOOL;

    (* Internal *)
    any_trip       : BOOL;
    alarm_latch    : SR;
    alarm_delay    : TON;
    alarm_active   : BOOL;
    latched        : BOOL;

    (* Outputs *)
    shutdown_cmd   : BOOL;
    alarm_out      : BOOL;
  END_VAR

  (* Combine trip conditions: any single trip is enough *)
  any_trip := pressure_high OR temp_high OR flow_low;

  (* SR latch: Set on trip, Reset on manual acknowledge *)
  alarm_latch(S1 := any_trip, R := manual_reset);
  latched := alarm_latch.Q1;

  (* Delay alarm output by 3 seconds to filter transients *)
  alarm_delay(IN := latched, PT := T#3s);

  (* Output logic *)
  IF latched AND alarm_delay.Q THEN
    (* Confirmed alarm: shutdown and alarm *)
    shutdown_cmd := TRUE;
    alarm_out := TRUE;
  ELSIF latched AND NOT alarm_delay.Q THEN
    (* Alarm pending confirmation *)
    shutdown_cmd := FALSE;
    alarm_out := FALSE;
  ELSE
    (* No alarm *)
    shutdown_cmd := FALSE;
    alarm_out := FALSE;
  END_IF;

END_PROGRAM


CONFIGURATION esd_config
  RESOURCE esd_resource ON PLC
    TASK esd_task(INTERVAL := T#10ms, PRIORITY := 0);
    PROGRAM esd_inst WITH esd_task : esd_interlock;
  END_RESOURCE
END_CONFIGURATION
