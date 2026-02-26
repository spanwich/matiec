# DELIVERABLE 4: Annotated Trace — test_timer.st (TON Timer)

## Input Source

```iec
PROGRAM test_timer
  VAR
    start_signal : BOOL;
    delay_time   : TIME := T#5s;
    timer1       : TON;
    done         : BOOL;
    elapsed      : TIME;
  END_VAR

  (* Invoke TON timer *)
  timer1(IN := start_signal, PT := delay_time);

  (* Read outputs *)
  done := timer1.Q;
  elapsed := timer1.ET;

  (* Reset when done *)
  IF done THEN
    start_signal := FALSE;
  END_IF;

END_PROGRAM

CONFIGURATION timer_config
  RESOURCE timer_resource ON PLC
    TASK timer_task(INTERVAL := T#10ms, PRIORITY := 0);
    PROGRAM timer_inst WITH timer_task : test_timer;
  END_RESOURCE
END_CONFIGURATION
```

## AST Representation (Key Nodes)

```
program_declaration_c("test_timer")
├── var_declarations_list_c
│   ├── var1_init_decl_c: start_signal : BOOL
│   ├── var1_init_decl_c: delay_time : TIME := T#5s
│   │   └── simple_spec_init_c
│   │       ├── time_type_name_c
│   │       └── duration_c("T#5s")  → const_value = {tv_sec=5, tv_nsec=0}
│   ├── fb_name_decl_c: timer1 : TON
│   │   └── identifier_c("TON")
│   ├── var1_init_decl_c: done : BOOL
│   └── var1_init_decl_c: elapsed : TIME
│
└── statement_list_c
    ├── fb_invocation_c                              // timer1(IN:=..., PT:=...)
    │   ├── fb_name: identifier_c("timer1")
    │   ├── formal_param_list: param_assignment_list_c
    │   │   ├── input_variable_param_assignment_c
    │   │   │   ├── variable_name: identifier_c("IN")
    │   │   │   └── expression: symbolic_variable_c("start_signal")
    │   │   └── input_variable_param_assignment_c
    │   │       ├── variable_name: identifier_c("PT")
    │   │       └── expression: symbolic_variable_c("delay_time")
    │   └── called_fb_declaration → (points to TON FB declaration)
    │
    ├── assignment_statement_c                       // done := timer1.Q
    │   ├── l_exp: symbolic_variable_c("done")
    │   └── r_exp: structured_variable_c
    │       ├── record_variable: symbolic_variable_c("timer1")
    │       └── field_selector: identifier_c("Q")
    │
    ├── assignment_statement_c                       // elapsed := timer1.ET
    │   ├── l_exp: symbolic_variable_c("elapsed")
    │   └── r_exp: structured_variable_c
    │       ├── record_variable: symbolic_variable_c("timer1")
    │       └── field_selector: identifier_c("ET")
    │
    └── if_statement_c                               // IF done THEN ...
        ├── expression: symbolic_variable_c("done")
        ├── statement_list: statement_list_c
        │   └── assignment_statement_c               // start_signal := FALSE
        ├── elseif_statement_list: NULL
        └── else_statement_list: NULL
```

## Generated C — Program Structure (POUS.h)

```c
#ifndef __POUS_H
#define __POUS_H

#include "accessor.h"
#include "iec_std_lib.h"

// PROGRAM TEST_TIMER
// Data part
typedef struct {
  // PROGRAM Interface - IN, OUT, IN_OUT variables

  // PROGRAM private variables - TEMP, private and located variables
  __DECLARE_VAR(BOOL,START_SIGNAL)
  __DECLARE_VAR(TIME,DELAY_TIME)
  TON_data__ TIMER1;                    // FB instance: NOT wrapped in __DECLARE_VAR
  __DECLARE_VAR(BOOL,DONE)
  __DECLARE_VAR(TIME,ELAPSED)

} TEST_TIMER_data__;

void TEST_TIMER_init__(TEST_TIMER_data__ *data__, BOOL retain);
void TEST_TIMER_body__(TEST_TIMER_data__ *data__);
#endif //__POUS_H
```

**Key observation**: FB instances (like `TIMER1`) are embedded directly as `TON_data__` structs, NOT wrapped in `__DECLARE_VAR`. This is because FBs are not simple variables — they have their own init/body functions and internal state.

## Generated C — Init and Body (POUS.c)

```c
void TEST_TIMER_init__(TEST_TIMER_data__ *data__, BOOL retain) {
  __INIT_VAR(data__->START_SIGNAL,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->DELAY_TIME,__time_to_timespec(1, 0, 5, 0, 0, 0),retain)
  TON_init__(&data__->TIMER1,retain);       // FB has its own init function
  __INIT_VAR(data__->DONE,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->ELAPSED,__time_to_timespec(1, 0, 0, 0, 0, 0),retain)
}

void TEST_TIMER_body__(TEST_TIMER_data__ *data__) {
  // Initialise TEMP variables

  // timer1(IN := start_signal, PT := delay_time)
  __SET_VAR(data__->TIMER1.,IN,,__GET_VAR(data__->START_SIGNAL,));   // Set input
  __SET_VAR(data__->TIMER1.,PT,,__GET_VAR(data__->DELAY_TIME,));     // Set input
  TON_body__(&data__->TIMER1);                                       // Execute FB

  // done := timer1.Q
  __SET_VAR(data__->,DONE,,__GET_VAR(data__->TIMER1.Q,));            // Read output

  // elapsed := timer1.ET
  __SET_VAR(data__->,ELAPSED,,__GET_VAR(data__->TIMER1.ET,));        // Read output

  // IF done THEN start_signal := FALSE; END_IF
  if (__GET_VAR(data__->DONE,)) {
    __SET_VAR(data__->,START_SIGNAL,,__BOOL_LITERAL(FALSE));
  };

  goto __end;

__end:
  return;
} // TEST_TIMER_body__()
```

**FB Invocation Pattern**:
1. Set input fields: `__SET_VAR(data__->TIMER1.,IN,,value)`
2. Call body: `TON_body__(&data__->TIMER1)`
3. Read output fields: `__GET_VAR(data__->TIMER1.Q,)`

This is a **direct struct field access + function call** pattern — no dynamic dispatch, no virtual calls.

## TON_data__ Structure (from `lib/C/iec_std_FB.h:389-406`)

```c
typedef struct {
  // FB Interface - IN, OUT, IN_OUT variables
  __DECLARE_VAR(BOOL,EN)           // Enable input (auto-generated)
  __DECLARE_VAR(BOOL,ENO)          // Enable output (auto-generated)
  __DECLARE_VAR(BOOL,IN)           // Timer trigger input
  __DECLARE_VAR(TIME,PT)           // Preset time (duration)
  __DECLARE_VAR(BOOL,Q)            // Output: timer complete
  __DECLARE_VAR(TIME,ET)           // Output: elapsed time

  // FB private variables - TEMP, private and located variables
  __DECLARE_VAR(SINT,STATE)        // State machine: 0=idle, 1=timing, 2=done
  __DECLARE_VAR(BOOL,PREV_IN)      // Previous IN (for rising edge detection)
  __DECLARE_VAR(TIME,CURRENT_TIME) // Snapshot of __CURRENT_TIME
  __DECLARE_VAR(TIME,START_TIME)   // When timing started
} TON_data__;
```

## TON State Machine (from `lib/C/iec_std_FB.h:1472-1514`)

Complete annotated implementation:

```c
static void TON_body__(TON_data__ *data__) {
  // EN/ENO check (auto-generated for all FBs)
  if (!__GET_VAR(data__->EN)) {
    __SET_VAR(data__->,ENO,,__BOOL_LITERAL(FALSE));
    return;                         // Skip execution if disabled
  } else {
    __SET_VAR(data__->,ENO,,__BOOL_LITERAL(TRUE));
  }

  // Read global time
  __SET_VAR(data__->,CURRENT_TIME,,__CURRENT_TIME)

  // STATE 0 → 1: Rising edge of IN (IN goes TRUE when it was FALSE)
  if ((STATE == 0) && !PREV_IN && IN) {
    STATE = 1;                      // Start timing
    Q = FALSE;                      // Output not yet triggered
    START_TIME = CURRENT_TIME;      // Record start time
  }
  // IN goes FALSE at any time: RESET
  else if (!IN) {
    ET = T#0s;                      // Reset elapsed time to 0
    Q = FALSE;                      // Output goes FALSE
    STATE = 0;                      // Back to idle
  }
  // STATE 1: Currently timing, check if PT has elapsed
  else if (STATE == 1) {
    if (START_TIME + PT <= CURRENT_TIME) {  // Time elapsed?
      STATE = 2;                    // Done
      Q = TRUE;                     // Output goes TRUE
      ET = PT;                      // ET clamped to PT
    } else {
      ET = CURRENT_TIME - START_TIME;  // Update elapsed time
    }
  }
  // STATE 2: Done, IN still TRUE — hold Q=TRUE, ET=PT
  // (no explicit code needed, values already set)

  PREV_IN = IN;                     // Save for next cycle's edge detection
}
```

### State Transition Diagram

```
                 IN rising edge
    ┌──────┐   (STATE==0, !PREV_IN, IN)   ┌──────────┐
    │ IDLE │ ─────────────────────────────→│ TIMING   │
    │  S=0 │                               │  S=1     │
    │  Q=0 │←──────────────────────────────│  Q=0     │
    │ ET=0 │       IN goes FALSE           │ ET=curr  │
    └──────┘       (RESET)                 └────┬─────┘
        ↑                                       │
        │           IN goes FALSE               │ START_TIME+PT <= CURRENT_TIME
        │           (RESET)                     ↓
        │                                  ┌──────────┐
        └──────────────────────────────────│  DONE    │
                                           │  S=2     │
                                           │  Q=1     │
                                           │ ET=PT    │
                                           └──────────┘
```

### Critical Behavior: Bouncing Input (Timer RESETS)

If IN goes FALSE at any time during STATE==1 (timing):
- **ET resets to 0** (not preserved)
- **STATE goes back to 0** (idle)
- There is **NO elapsed-time continuation** — the timer starts from scratch on the next rising edge

This is the IEC 61131-3 specified behavior for TON. If an application needs a "pausable" timer, it must implement that logic externally.

## IEC_TIMESPEC Structure (from `lib/C/iec_types.h`)

```c
typedef struct {
    int64_t tv_sec;            // Seconds (y2038 ready)
    int32_t tv_nsec;           // Nanoseconds (32-bit is enough)
} __attribute__((packed)) IEC_TIMESPEC;   // 12 bytes, packed

typedef IEC_TIMESPEC IEC_TIME;
typedef IEC_TIMESPEC IEC_DATE;
typedef IEC_TIMESPEC IEC_DT;
typedef IEC_TIMESPEC IEC_TOD;
```

### Time Conversion

`T#5s` → `__time_to_timespec(1, 0, 5, 0, 0, 0)`:
- Arguments: (sign, days, hours_or_secs, minutes, seconds, milliseconds)
- Actually: `__time_to_timespec(sign, days, hours, minutes, seconds, milliseconds)`
- For `T#5s`: sign=1, days=0, hours=0, minutes=0, seconds=5, milliseconds=0
- Result: `{tv_sec=5, tv_nsec=0}`

### Time Arithmetic Functions

```c
// Addition: a + b
static inline IEC_TIMESPEC __time_add(IEC_TIMESPEC a, IEC_TIMESPEC b) {
    IEC_TIMESPEC res = {a.tv_sec + b.tv_sec, a.tv_nsec + b.tv_nsec};
    __normalize_timespec(&res);
    return res;
}

// Subtraction: a - b
static inline IEC_TIMESPEC __time_sub(IEC_TIMESPEC a, IEC_TIMESPEC b) {
    IEC_TIMESPEC res = {a.tv_sec - b.tv_sec, a.tv_nsec - b.tv_nsec};
    __normalize_timespec(&res);
    return res;
}

// Normalize: ensure 0 <= tv_nsec < 1000000000
static inline void __normalize_timespec(IEC_TIMESPEC *ts) {
    if (ts->tv_nsec >= 1000000000) {
        ts->tv_sec  += ts->tv_nsec / 1000000000;
        ts->tv_nsec %= 1000000000;
    } else if (ts->tv_nsec < 0) {
        ts->tv_sec  += ts->tv_nsec / 1000000000 - 1;
        ts->tv_nsec %= 1000000000;
        if (ts->tv_nsec < 0) ts->tv_nsec += 1000000000;
    }
}

// Comparison: START_TIME + PT <= CURRENT_TIME
// Uses ___LE_TIME() which compares tv_sec first, then tv_nsec
static inline BOOL ___LE_TIME(BOOL EN, ...) {
    // Compares two IEC_TIMESPEC values: a <= b
}
```

### Global Time

The runtime provides `__CURRENT_TIME` as a global variable of type `IEC_TIMESPEC`, updated before each scan cycle by the PLC runtime. TON reads this each execution:

```c
__SET_VAR(data__->,CURRENT_TIME,,__CURRENT_TIME)
```

## TON Init Function

```c
static void TON_init__(TON_data__ *data__, BOOL retain) {
  __INIT_VAR(data__->EN,__BOOL_LITERAL(TRUE),retain)     // EN defaults TRUE
  __INIT_VAR(data__->ENO,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->IN,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->PT,__time_to_timespec(1,0,0,0,0,0),retain)  // PT=T#0s
  __INIT_VAR(data__->Q,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->ET,__time_to_timespec(1,0,0,0,0,0),retain)  // ET=T#0s
  __INIT_VAR(data__->STATE,0,retain)                      // STATE=0 (idle)
  __INIT_VAR(data__->PREV_IN,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->CURRENT_TIME,__time_to_timespec(1,0,0,0,0,0),retain)
  __INIT_VAR(data__->START_TIME,__time_to_timespec(1,0,0,0,0,0),retain)
}
```

All fields zero-initialized. STATE=0 (idle), PREV_IN=FALSE, all times=T#0s.

## LowPLC Implications

### What LowPLC must model for TON:

1. **State**: 3-state machine (IDLE=0, TIMING=1, DONE=2) as a SINT (int8_t)
2. **Edge detection**: PREV_IN boolean, compared with current IN each cycle
3. **Time arithmetic**: Addition, subtraction, comparison of `{int64_t sec, int32_t nsec}`
4. **Normalization**: nsec must stay in [0, 999999999] after arithmetic
5. **Global time source**: `__CURRENT_TIME` must be provided externally each cycle
6. **Reset behavior**: IN going FALSE always resets to STATE=0, ET=0

### Minimal TON type signature for F*:

```fstar
type ton_state = { state: int8; prev_in: bool; start_time: timespec; }
type timespec = { tv_sec: int64; tv_nsec: int32; }

val ton_step: ton_state → bool → timespec → timespec → (ton_state * bool * timespec)
// ton_step old_state in_val preset_time current_time → (new_state, q_out, et_out)
```

### TOF/TP/CTU/CTD follow similar patterns:
- Same 3-state machine structure
- Same edge detection via PREV_IN
- CTU/CTD replace time with integer counter
- All are pure functions of (current_state, inputs) → (new_state, outputs)
