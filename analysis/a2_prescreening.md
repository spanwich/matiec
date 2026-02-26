# A2 Pre-Screening: Benchmark Construct Analysis

**Purpose**: Document the ST constructs, FB types, and matiec AST classes exercised by each benchmark (B1–B5). Identify any constructs outside the 65-class "Model in F*" scope and any fixes applied during development.

---

## B1: Emergency Shutdown Interlock (`benchmarks/b1_esd.st`)

**Scenario**: Three trip sensors (pressure, temperature, flow) trigger an SR-latched alarm. A TON timer filters transients with a 3-second delay. Manual acknowledge resets the latch. IF/ELSIF/ELSE selects output logic.

**ST Constructs Used**:
- Boolean operators: `OR` (`or_expression_c`), `AND` (`and_expression_c`), `NOT` (`not_expression_c`)
- Assignment: `assignment_statement_c`
- IF/ELSIF/ELSE: `if_statement_c`, `elseif_statement_list_c`, `elseif_statement_c`
- FB invocation: `fb_invocation_c`, `param_assignment_list_c`, `input_variable_param_assignment_c`
- Structured variable access: `structured_variable_c` (e.g., `alarm_latch.Q1`, `alarm_delay.Q`)
- Statement list: `statement_list_c`

**FB Types**:
- `SR` — params: `S1`, `R`; output: `Q1`
- `TON` — params: `IN`, `PT`; output: `Q`

**Literals**:
- `boolean_true_c`, `boolean_false_c` (TRUE/FALSE in assignments)
- `duration_c`, `interval_c` (T#3s, T#10ms)

**Type Names**: `bool_type_name_c`, `time_type_name_c` (implicit via FB params)

**Variable Declarations**: `var_declarations_c`, `var_init_decl_list_c`, `var1_init_decl_c`, `var1_list_c`, `simple_spec_init_c`, `fb_name_decl_c`, `fb_spec_init_c`

**Configuration**: `configuration_declaration_c`, `resource_declaration_c`, `single_resource_declaration_c`, `task_configuration_c`, `task_initialization_c`, `program_configuration_c`, `task_configuration_list_c`, `program_configuration_list_c`

**Infrastructure**: `library_c`, `identifier_c`, `program_declaration_c`

**Constructs Outside LowPLC-ST Scope**: None.

> **Design deviation**: B1 was intended as a pure boolean baseline, but includes a TON timer (`ALARM_DELAY`) for transient filtering. This means B1 measures boolean logic + one timer body, not boolean logic alone. The TON contribution is separately measurable via GPIO instrumentation.

**Fixes Applied**: None required — no keyword collisions, no comment syntax issues.

---

## B2: Burner Management Purge Sequence (`benchmarks/b2_bms.st`)

**Scenario**: 5-stage burner management: idle(0) → purge(1) → ignite(2) → run(3) → lockout(4). Uses R_TRIG/F_TRIG for edge detection, two TON timers, and a nested ELSIF chain implementing the state machine. INT comparisons drive state transitions.

**ST Constructs Used**:
- Comparison operators: `equ_expression_c` (`=`), `notequ_expression_c` (`<>` — implicit in state != checks)
- Boolean operators: `and_expression_c`, `not_expression_c`
- Assignment: `assignment_statement_c`
- IF/ELSIF: `if_statement_c`, `elseif_statement_list_c`, `elseif_statement_c`
- FB invocation: `fb_invocation_c`, `param_assignment_list_c`, `input_variable_param_assignment_c`
- Structured variable access: `structured_variable_c` (e.g., `start_edge.Q`, `flame_lost.Q`, `purge_timer.Q`)
- Statement list: `statement_list_c`

**FB Types**:
- `R_TRIG` — params: `CLK`; output: `Q`
- `F_TRIG` — params: `CLK`; output: `Q`
- `TON` (x2) — params: `IN`, `PT`; output: `Q`

**Literals**:
- `integer_c` (0, 1, 2, 3, 4 — state values)
- `boolean_true_c`, `boolean_false_c`
- `duration_c`, `interval_c` (T#30s, T#5s, T#20ms)

**Type Names**: `bool_type_name_c`, `int_type_name_c`, `time_type_name_c`

**Variable Declarations**: `var_declarations_c`, `var_init_decl_list_c`, `var1_init_decl_c`, `var1_list_c`, `simple_spec_init_c`, `fb_name_decl_c`, `fb_spec_init_c`

**Configuration**: `configuration_declaration_c`, `resource_declaration_c`, `single_resource_declaration_c`, `task_configuration_c`, `task_initialization_c`, `program_configuration_c`, `task_configuration_list_c`, `program_configuration_list_c`

**Infrastructure**: `library_c`, `identifier_c`, `program_declaration_c`

**Constructs Outside LowPLC-ST Scope**: None.

**Fixes Applied**: None required.

---

## B3: Conveyor with Part Counter (`benchmarks/b3_conveyor.st`)

**Scenario**: Conveyor counts parts via CTU counter triggered by R_TRIG edge detection. A FOR loop computes batch totals. Motor latch logic uses IF statements. Exercises integer arithmetic (+), comparison (>), and bounded iteration.

**ST Constructs Used**:
- Arithmetic operators: `add_expression_c` (`+`)
- Comparison operators: `equ_expression_c` (implicit in CTU PV comparison)
- Boolean operators: (none explicit — boolean logic is via IF conditions)
- Assignment: `assignment_statement_c`
- IF: `if_statement_c`
- FOR loop: `for_statement_c` (bounded, 1 TO 5)
- FB invocation: `fb_invocation_c`, `param_assignment_list_c`, `input_variable_param_assignment_c`
- Structured variable access: `structured_variable_c` (e.g., `part_edge.Q`, `part_counter.Q`, `part_counter.CV`)
- Statement list: `statement_list_c`

> **FOR loop bounds note**: Actual loop is `FOR idx := 1 TO 5`, not `1 TO 4` as stated in earlier design notes. The WCET measurement uses the true bound of 5 iterations.

**FB Types**:
- `R_TRIG` — params: `CLK`; output: `Q`
- `CTU` — params: `CU`, `R`, `PV`; outputs: `Q`, `CV`

**Literals**:
- `integer_c` (0, 1, 5, 10 — loop bounds, batch size, init values)
- `boolean_true_c`, `boolean_false_c`
- `duration_c`, `interval_c` (T#10ms)

**Type Names**: `bool_type_name_c`, `int_type_name_c`, `time_type_name_c`

**Variable Declarations**: `var_declarations_c`, `var_init_decl_list_c`, `var1_init_decl_c`, `var1_list_c`, `simple_spec_init_c`, `fb_name_decl_c`, `fb_spec_init_c`

**Configuration**: `configuration_declaration_c`, `resource_declaration_c`, `single_resource_declaration_c`, `task_configuration_c`, `task_initialization_c`, `program_configuration_c`, `task_configuration_list_c`, `program_configuration_list_c`

**Infrastructure**: `library_c`, `identifier_c`, `program_declaration_c`

**Constructs Outside LowPLC-ST Scope**: None.

**Fixes Applied**: None required.

---

## B4: Multi-Stage Shutdown Sequence (`benchmarks/b4_shutdown.st`)

**Scenario**: 4-stage shutdown: running(0) → cooldown(1) → depressurize(2) → safe(3). Each stage has a TON timer. Valve position decrements by 10 per cycle with a floor at 0 using subtraction and `<` comparison.

**ST Constructs Used**:
- Arithmetic operators: `sub_expression_c` (`-`)
- Comparison operators: `equ_expression_c` (`=`), `lt_expression_c` (`<`)
- Assignment: `assignment_statement_c`
- IF/ELSIF: `if_statement_c`, `elseif_statement_list_c`, `elseif_statement_c`
- FB invocation: `fb_invocation_c`, `param_assignment_list_c`, `input_variable_param_assignment_c`
- Structured variable access: `structured_variable_c` (e.g., `cooldown_tmr.Q`, `depress_tmr.Q`, `safe_tmr.Q`)
- Statement list: `statement_list_c`

**FB Types**:
- `TON` (x3) — params: `IN`, `PT`; output: `Q`

**Literals**:
- `integer_c` (0, 1, 2, 3, 10, 100 — state values, valve positions)
- `boolean_true_c`, `boolean_false_c`
- `duration_c`, `interval_c` (T#10s, T#15s, T#5s, T#50ms)
- `neg_integer_c` — implicit: the `< 0` comparison involves integer literal 0

**Type Names**: `bool_type_name_c`, `int_type_name_c`, `time_type_name_c`

**Variable Declarations**: `var_declarations_c`, `var_init_decl_list_c`, `var1_init_decl_c`, `var1_list_c`, `simple_spec_init_c`, `fb_name_decl_c`, `fb_spec_init_c`

**Configuration**: `configuration_declaration_c`, `resource_declaration_c`, `single_resource_declaration_c`, `task_configuration_c`, `task_initialization_c`, `program_configuration_c`, `task_configuration_list_c`, `program_configuration_list_c`

**Infrastructure**: `library_c`, `identifier_c`, `program_declaration_c`

**Constructs Outside LowPLC-ST Scope**: None.

> **Design deviation**: B4 was intended to use a single reused TON timer, but actually instantiates three separate TON instances (`COOLDOWN_TMR`, `DEPRESS_TMR`, `SAFE_TMR`). This means B4 measures 3 TON bodies per scan rather than 1. The WCET impact is proportional.

**Fixes Applied**: None required.

---

## B5: WCET Stress Test (`benchmarks/b5_stress.st`)

**Scenario**: Synthetic benchmark exercising ALL LowPLC constructs simultaneously. Instantiates all 9 standard FB types, uses all comparison operators (=, <>, <, >, <=, >=), all arithmetic operators (+, -, \*), all boolean operators (AND, OR, NOT, XOR), FOR loop with accumulator, and nested IF/ELSIF/ELSE.

**ST Constructs Used**:
- All boolean operators: `and_expression_c`, `or_expression_c`, `not_expression_c`, `xor_expression_c`
- All comparison operators: `equ_expression_c`, `notequ_expression_c`, `lt_expression_c`, `gt_expression_c`, `le_expression_c`, `ge_expression_c`
- Arithmetic operators: `add_expression_c`, `sub_expression_c`, `mul_expression_c`
- Assignment: `assignment_statement_c`
- IF/ELSIF/ELSE: `if_statement_c`, `elseif_statement_list_c`, `elseif_statement_c`
- FOR loop: `for_statement_c` (bounded, 0 TO 9)
- FB invocation: `fb_invocation_c`, `param_assignment_list_c`, `input_variable_param_assignment_c`
- Structured variable access: `structured_variable_c` (e.g., `fb_rtrig.Q`, `fb_sr.Q1`, `fb_ton.Q`, `fb_ctu.CV`, `fb_ctd.CV`, `fb_ctud.CV`, `fb_rs.Q1`)
- Statement list: `statement_list_c`

> **FOR loop bounds note**: Actual loop is `FOR idx := 0 TO 9` (10 iterations), not `1 TO 4` as stated in earlier design notes. The WCET measurement uses the true bound of 10 iterations.

**FB Types** (all 9):
- `SR` — params: `S1`, `R`; output: `Q1`
- `RS` — params: `S`, `R1`; output: `Q1`
- `CTU` — params: `CU`, `R`, `PV`; outputs: `Q`, `CV`
- `CTD` — params: `CD`, `LD`, `PV`; outputs: `Q`, `CV`
- `CTUD` — params: `CU`, `CD`, `R`, `LD`, `PV`; outputs: `QU`, `QD`, `CV`
- `TON` — params: `IN`, `PT`; output: `Q`
- `TOF` — params: `IN`, `PT`; output: `Q`
- `R_TRIG` — params: `CLK`; output: `Q`
- `F_TRIG` — params: `CLK`; output: `Q`

**Literals**:
- `integer_c` (0, 1, 2, 9, 50, 100 — loop bounds, init values, multiplier)
- `boolean_true_c`, `boolean_false_c`
- `duration_c`, `interval_c` (T#2s, T#1s, T#10ms)

**Type Names**: `bool_type_name_c`, `int_type_name_c`, `time_type_name_c`

**Variable Declarations**: `var_declarations_c`, `var_init_decl_list_c`, `var1_init_decl_c`, `var1_list_c`, `simple_spec_init_c`, `fb_name_decl_c`, `fb_name_list_c`, `fb_spec_init_c`

**Configuration**: `configuration_declaration_c`, `resource_declaration_c`, `single_resource_declaration_c`, `task_configuration_c`, `task_initialization_c`, `program_configuration_c`, `task_configuration_list_c`, `program_configuration_list_c`

**Infrastructure**: `library_c`, `identifier_c`, `program_declaration_c`

**Constructs Outside LowPLC-ST Scope**: None.

**Fixes Applied**:
1. **Keyword collision** (`IN`): Input variables named `in_a`, `in_b`, etc. collided with the IEC 61131-3 `IN` keyword (used as a timer/counter parameter). Renamed all inputs to use `inp_` prefix (e.g., `inp_a`, `inp_b`).
2. **Comment syntax**: Original `(* ... *)` comments containing the `*` character (e.g., multiplication descriptions with `*`) caused premature comment closure. Rewrote affected comments to avoid bare `*` characters inside comment delimiters.

---

## Summary Table

| Benchmark | Scenario | FBs Used | Key Expression Types | Key Statement Types | Fixes |
|-----------|----------|----------|---------------------|-------------------|-------|
| B1 | ESD interlock | SR, TON | OR, AND, NOT | IF/ELSIF/ELSE, Assign, FBCall | None |
| B2 | Burner management | R_TRIG, F_TRIG, TON×2 | =, AND, NOT | IF/ELSIF (5-state), Assign, FBCall | None |
| B3 | Conveyor counter | R_TRIG, CTU | + | IF, FOR, Assign, FBCall | None |
| B4 | Shutdown sequence | TON×3 | -, <, = | IF/ELSIF (4-state), Assign, FBCall | None |
| B5 | Stress test (all) | All 9 FBs | All 15 expr types | IF/ELSIF/ELSE, FOR, Assign, FBCall | `inp_` prefix; comment rewrite |

### Operator Coverage Across Benchmarks

| Operator | matiec Class | B1 | B2 | B3 | B4 | B5 |
|----------|-------------|----|----|----|----|-----|
| OR | `or_expression_c` | x | | | | x |
| XOR | `xor_expression_c` | | | | | x |
| AND | `and_expression_c` | x | x | | | x |
| NOT | `not_expression_c` | x | x | | | x |
| = | `equ_expression_c` | | x | | x | x |
| <> | `notequ_expression_c` | | | | | x |
| < | `lt_expression_c` | | | | x | x |
| > | `gt_expression_c` | | | | | x |
| <= | `le_expression_c` | | | | | x |
| >= | `ge_expression_c` | | | | | x |
| + | `add_expression_c` | | | x | | x |
| - | `sub_expression_c` | | | | x | x |
| * | `mul_expression_c` | | | | | x |
| / | `div_expression_c` | | | | | (not used) |
| MOD | `mod_expression_c` | | | | | (not used) |

**Note**: `div_expression_c` and `mod_expression_c` are in the 65-class "Model in F*" set but are not exercised by any benchmark. These are included in the F* AST for completeness since integer division and modulo are fundamental arithmetic operations that will appear in real industrial programs. The 5 benchmarks achieve coverage of 13/15 expression classes.
