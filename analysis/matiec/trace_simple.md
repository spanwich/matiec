# DELIVERABLE 3: Annotated Trace — test_simple.st

## Input Source (Structured Text)

```iec
PROGRAM test_simple
  VAR
    start_btn : BOOL;
    stop_btn  : BOOL;
    running   : BOOL;
    count     : INT;
    max_count : INT := 100;
    alarm     : BOOL;
  END_VAR

  (* Boolean logic: start/stop latch *)
  running := (start_btn OR running) AND NOT stop_btn;

  (* Counting with IF *)
  IF running THEN
    count := count + 1;
    IF count >= max_count THEN
      alarm := TRUE;
      count := 0;
    END_IF;
  ELSE
    alarm := FALSE;
  END_IF;

END_PROGRAM

CONFIGURATION test_config
  RESOURCE test_resource ON PLC
    TASK main_task(INTERVAL := T#20ms, PRIORITY := 0);
    PROGRAM main_inst WITH main_task : test_simple;
  END_RESOURCE
END_CONFIGURATION
```

## AST Node Tree (Simplified)

After stage1_2 parsing, the AST looks like this (class names from `absyntax/absyntax.def`):

```
library_c
├── program_declaration_c                           // PROGRAM test_simple
│   ├── program_type_name: identifier_c("test_simple")
│   ├── var_declarations: var_declarations_list_c
│   │   └── var_declaration_c
│   │       └── var_init_decl_list_c
│   │           ├── var1_init_decl_c                 // start_btn : BOOL
│   │           │   ├── var1_list: var1_list_c
│   │           │   │   └── identifier_c("start_btn")
│   │           │   └── simple_spec_init_c
│   │           │       └── bool_type_name_c
│   │           ├── var1_init_decl_c                 // stop_btn : BOOL
│   │           ├── var1_init_decl_c                 // running : BOOL
│   │           ├── var1_init_decl_c                 // count : INT
│   │           │   ├── var1_list: var1_list_c
│   │           │   │   └── identifier_c("count")
│   │           │   └── simple_spec_init_c
│   │           │       └── int_type_name_c
│   │           ├── var1_init_decl_c                 // max_count : INT := 100
│   │           │   ├── var1_list_c
│   │           │   │   └── identifier_c("max_count")
│   │           │   └── simple_spec_init_c
│   │           │       ├── int_type_name_c
│   │           │       └── integer_c("100")
│   │           └── var1_init_decl_c                 // alarm : BOOL
│   │
│   └── function_block_body: statement_list_c
│       ├── assignment_statement_c                   // running := ...
│       │   ├── l_exp: symbolic_variable_c
│       │   │   └── identifier_c("running")
│       │   └── r_exp: and_expression_c              // (...) AND NOT stop_btn
│       │       ├── l_exp: or_expression_c           // start_btn OR running
│       │       │   ├── l_exp: symbolic_variable_c("start_btn")
│       │       │   └── r_exp: symbolic_variable_c("running")
│       │       └── r_exp: not_expression_c          // NOT stop_btn
│       │           └── exp: symbolic_variable_c("stop_btn")
│       │
│       └── if_statement_c                           // IF running THEN ...
│           ├── expression: symbolic_variable_c("running")
│           ├── statement_list: statement_list_c     // THEN branch
│           │   ├── assignment_statement_c           // count := count + 1
│           │   │   ├── l_exp: symbolic_variable_c("count")
│           │   │   └── r_exp: add_expression_c
│           │   │       ├── l_exp: symbolic_variable_c("count")
│           │   │       └── r_exp: integer_c("1")
│           │   └── if_statement_c                   // nested IF count >= max_count
│           │       ├── expression: ge_expression_c
│           │       │   ├── l_exp: symbolic_variable_c("count")
│           │       │   └── r_exp: symbolic_variable_c("max_count")
│           │       ├── statement_list: statement_list_c  // THEN
│           │       │   ├── assignment_statement_c   // alarm := TRUE
│           │       │   │   ├── l_exp: symbolic_variable_c("alarm")
│           │       │   │   └── r_exp: boolean_literal_c → boolean_true_c
│           │       │   └── assignment_statement_c   // count := 0
│           │       │       ├── l_exp: symbolic_variable_c("count")
│           │       │       └── r_exp: integer_c("0")
│           │       ├── elseif_statement_list: NULL
│           │       └── else_statement_list: NULL
│           ├── elseif_statement_list: NULL
│           └── else_statement_list: statement_list_c  // ELSE
│               └── assignment_statement_c           // alarm := FALSE
│                   ├── l_exp: symbolic_variable_c("alarm")
│                   └── r_exp: boolean_literal_c → boolean_false_c
│
└── configuration_declaration_c                      // CONFIGURATION test_config
    ├── configuration_name: identifier_c("test_config")
    └── resource_declarations: resource_declaration_list_c
        └── resource_declaration_c
            ├── resource_name: identifier_c("test_resource")
            ├── resource_type_name: identifier_c("PLC")
            └── resource_declaration: single_resource_declaration_c
                ├── task_configuration_list_c
                │   └── task_configuration_c
                │       ├── task_name: identifier_c("main_task")
                │       └── task_initialization_c (INTERVAL := T#20ms, PRIORITY := 0)
                └── program_configuration_list_c
                    └── program_configuration_c
                        └── (main_inst WITH main_task : test_simple)
```

## Stage 3 Type Annotations

After stage3, key nodes have their `datatype` field populated:

| Expression | candidate_datatypes | Resolved datatype |
|-----------|---------------------|-------------------|
| `start_btn` (symbolic_variable_c) | [bool_type_name_c] | bool_type_name_c |
| `running` (symbolic_variable_c) | [bool_type_name_c] | bool_type_name_c |
| `count` (symbolic_variable_c) | [int_type_name_c] | int_type_name_c |
| `max_count` (symbolic_variable_c) | [int_type_name_c] | int_type_name_c |
| `start_btn OR running` (or_expression_c) | [bool_type_name_c] | bool_type_name_c |
| `NOT stop_btn` (not_expression_c) | [bool_type_name_c] | bool_type_name_c |
| `(...) AND NOT stop_btn` (and_expression_c) | [bool_type_name_c] | bool_type_name_c |
| `count + 1` (add_expression_c) | [int_type_name_c] | int_type_name_c |
| `count >= max_count` (ge_expression_c) | [bool_type_name_c] | bool_type_name_c |
| `integer_c("1")` | [int_type_name_c, uint_type_name_c, ...] | int_type_name_c |
| `integer_c("0")` (in count:=0) | [int_type_name_c, uint_type_name_c, ...] | int_type_name_c |
| `integer_c("100")` (initializer) | [int_type_name_c, uint_type_name_c, ...] | int_type_name_c |
| `boolean_true_c` | [bool_type_name_c] | bool_type_name_c |

**Narrowing**: Integer literals initially have many candidate types (INT, UINT, DINT, etc.). The narrowing pass resolves them based on context — `count + 1` where count is INT narrows `1` to INT.

## Generated C Output (Complete)

### POUS.h — Program Data Structure

```c
#ifndef __POUS_H
#define __POUS_H

#include "accessor.h"
#include "iec_std_lib.h"

// PROGRAM TEST_SIMPLE
// Data part
typedef struct {
  // PROGRAM Interface - IN, OUT, IN_OUT variables

  // PROGRAM private variables - TEMP, private and located variables
  __DECLARE_VAR(BOOL,START_BTN)
  __DECLARE_VAR(BOOL,STOP_BTN)
  __DECLARE_VAR(BOOL,RUNNING)
  __DECLARE_VAR(INT,COUNT)
  __DECLARE_VAR(INT,MAX_COUNT)
  __DECLARE_VAR(BOOL,ALARM)

} TEST_SIMPLE_data__;

void TEST_SIMPLE_init__(TEST_SIMPLE_data__ *data__, BOOL retain);
// Code part
void TEST_SIMPLE_body__(TEST_SIMPLE_data__ *data__);
#endif //__POUS_H
```

**Observations**:
- Program → `typedef struct { ... } TEST_SIMPLE_data__`
- All variable names uppercased
- Each variable wrapped in `__DECLARE_VAR(type, name)` macro
- No separation between different VAR sections (all become private)
- Two functions: `_init__` and `_body__`

### POUS.c — Program Init and Body

```c
void TEST_SIMPLE_init__(TEST_SIMPLE_data__ *data__, BOOL retain) {
  __INIT_VAR(data__->START_BTN,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->STOP_BTN,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->RUNNING,__BOOL_LITERAL(FALSE),retain)
  __INIT_VAR(data__->COUNT,0,retain)
  __INIT_VAR(data__->MAX_COUNT,100,retain)              // Initial value from := 100
  __INIT_VAR(data__->ALARM,__BOOL_LITERAL(FALSE),retain)
}

// Code part
void TEST_SIMPLE_body__(TEST_SIMPLE_data__ *data__) {
  // Initialise TEMP variables

  // running := (start_btn OR running) AND NOT stop_btn
  __SET_VAR(data__->,RUNNING,,
    ((__GET_VAR(data__->START_BTN,) || __GET_VAR(data__->RUNNING,))
     && !(__GET_VAR(data__->STOP_BTN,))));

  // IF running THEN
  if (__GET_VAR(data__->RUNNING,)) {
    // count := count + 1
    __SET_VAR(data__->,COUNT,,(__GET_VAR(data__->COUNT,) + 1));

    // IF count >= max_count THEN
    if ((__GET_VAR(data__->COUNT,) >= __GET_VAR(data__->MAX_COUNT,))) {
      // alarm := TRUE
      __SET_VAR(data__->,ALARM,,__BOOL_LITERAL(TRUE));
      // count := 0
      __SET_VAR(data__->,COUNT,,0);
    };
  } else {
    // alarm := FALSE
    __SET_VAR(data__->,ALARM,,__BOOL_LITERAL(FALSE));
  };

  goto __end;

__end:
  return;
} // TEST_SIMPLE_body__()
```

**Code Generation Patterns**:

| IEC Construct | C Output |
|--------------|----------|
| `var := expr` | `__SET_VAR(data__->,VAR,,expr)` |
| Read variable | `__GET_VAR(data__->VAR,)` |
| `a OR b` | `a \|\| b` |
| `a AND b` | `a && b` |
| `NOT a` | `!(a)` |
| `a >= b` | `(a >= b)` |
| `a + b` | `(a + b)` |
| `IF cond THEN ... END_IF` | `if (cond) { ... };` |
| `IF ... ELSE ... END_IF` | `if (...) { ... } else { ... };` |
| `TRUE` / `FALSE` | `__BOOL_LITERAL(TRUE)` / `__BOOL_LITERAL(FALSE)` |
| Integer literal `100` | `100` (plain C literal) |
| Init value `INT := 100` | `__INIT_VAR(data__->VAR,100,retain)` |

### test_config.c — Configuration

```c
#include "iec_std_lib.h"
#include "accessor.h"
#include "POUS.h"

// CONFIGURATION TEST_CONFIG

void TEST_RESOURCE_init__(void);

void config_init__(void) {
  BOOL retain;
  retain = 0;
  TEST_RESOURCE_init__();
}

void TEST_RESOURCE_run__(unsigned long tick);

void config_run__(unsigned long tick) {
  TEST_RESOURCE_run__(tick);
}
unsigned long long common_ticktime__ = 20000000ULL * 1ULL; /*ns*/
unsigned long greatest_tick_count__ = (unsigned long)0UL; /*tick*/
```

**Note**: `common_ticktime__ = 20000000ULL` = 20ms in nanoseconds (matches `T#20ms` from TASK declaration).

### test_resource.c — Resource (Task Scheduling)

```c
#include "iec_std_lib.h"

// RESOURCE TEST_RESOURCE

extern unsigned long long common_ticktime__;

#include "accessor.h"
#include "POUS.h"
#include "test_config.h"
#include "POUS.c"                    // NOTE: #include of .c file!

BOOL MAIN_TASK;
TEST_SIMPLE_data__ TEST_RESOURCE__MAIN_INST;
#define MAIN_INST TEST_RESOURCE__MAIN_INST

void TEST_RESOURCE_init__(void) {
  BOOL retain;
  retain = 0;
  MAIN_TASK = __BOOL_LITERAL(FALSE);
  TEST_SIMPLE_init__(&MAIN_INST,retain);
}

void TEST_RESOURCE_run__(unsigned long tick) {
  MAIN_TASK = !(tick % 1);           // Task fires every tick (INTERVAL/common_ticktime)
  if (MAIN_TASK) {
    TEST_SIMPLE_body__(&MAIN_INST);  // Execute program body
  }
}
```

**Key observation**: The resource instantiates the program as a **global struct** and calls `_body__()` on each qualifying tick. The tick modulo calculation implements task scheduling: `!(tick % 1)` means "fire every tick" (20ms interval / 20ms common ticktime = 1).

### VARIABLES.csv — Variable Inventory

```csv
// Programs
0;TEST_CONFIG.TEST_RESOURCE.MAIN_INST;TEST_SIMPLE;

// Variables
0;FB;TEST_CONFIG.TEST_RESOURCE.MAIN_INST;TEST_CONFIG.TEST_RESOURCE.MAIN_INST;TEST_SIMPLE;;0;
1;VAR;TEST_CONFIG.TEST_RESOURCE.MAIN_INST.START_BTN;...;BOOL;BOOL;0;
2;VAR;TEST_CONFIG.TEST_RESOURCE.MAIN_INST.STOP_BTN;...;BOOL;BOOL;0;
3;VAR;TEST_CONFIG.TEST_RESOURCE.MAIN_INST.RUNNING;...;BOOL;BOOL;0;
4;VAR;TEST_CONFIG.TEST_RESOURCE.MAIN_INST.COUNT;...;INT;INT;0;
5;VAR;TEST_CONFIG.TEST_RESOURCE.MAIN_INST.MAX_COUNT;...;INT;INT;0;
6;VAR;TEST_CONFIG.TEST_RESOURCE.MAIN_INST.ALARM;...;BOOL;BOOL;0;

// Ticktime
20000000
```

## Runtime Type Mapping

From `lib/C/iec_types.h`:

| IEC Type | C Type | Size |
|----------|--------|------|
| BOOL | `uint8_t` (IEC_BOOL) | 1 byte |
| SINT | `int8_t` (IEC_SINT) | 1 byte |
| INT | `int16_t` (IEC_INT) | 2 bytes |
| DINT | `int32_t` (IEC_DINT) | 4 bytes |
| LINT | `int64_t` (IEC_LINT) | 8 bytes |
| USINT | `uint8_t` (IEC_USINT) | 1 byte |
| UINT | `uint16_t` (IEC_UINT) | 2 bytes |
| UDINT | `uint32_t` (IEC_UDINT) | 4 bytes |
| ULINT | `uint64_t` (IEC_ULINT) | 8 bytes |
| REAL | `float` (IEC_REAL) | 4 bytes |
| LREAL | `double` (IEC_LREAL) | 8 bytes |
| TIME | `IEC_TIMESPEC` = `{int64_t tv_sec, int32_t tv_nsec}` | 12 bytes (packed) |
| STRING | `IEC_STRING` = `{uint8_t len, uint8_t body[126]}` | 127 bytes (packed) |
| BYTE | `uint8_t` (IEC_BYTE) | 1 byte |
| WORD | `uint16_t` (IEC_WORD) | 2 bytes |
| DWORD | `uint32_t` (IEC_DWORD) | 4 bytes |
| LWORD | `uint64_t` (IEC_LWORD) | 8 bytes |

**LowPLC implication**: For Category A (BOOL, INT, TIME only), the type universe is:
- `BOOL` → 1 byte (`uint8_t`), values 0/1
- `INT` → 2 bytes (`int16_t`), range -32768..32767
- `TIME` → 12 bytes (`{int64_t sec, int32_t nsec}`), packed struct

## Observation: Keyword Collisions

`LIMIT` is a standard function name in IEC 61131-3 and is treated as a keyword by the matiec lexer. This means it cannot be used as a variable name. Similarly, standard function names like `MIN`, `MAX`, `ABS`, `SEL`, `ADD`, `MUL`, etc. are all reserved.

**Original test_simple.st used `limit` as a variable name** — the compiler rejected it with "invalid variable(s) declaration". Renamed to `max_count`.

This is a consequence of the lexer/parser cooperation: the lexer returns `standard_function_name_token` for known function names, not `identifier`, so Bison cannot parse them as variable declarations.
