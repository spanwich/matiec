# LowPLC-ST Language Specification v1.0

## 1. Preamble

### 1.1 Purpose

LowPLC-ST is a formally specified subset of IEC 61131-3 Structured Text.
Every valid LowPLC-ST program is also a valid IEC 61131-3 ST program and
compiles unchanged through matiec, CODESYS, and other conforming IEC tools.
The converse is not true: LowPLC-ST excludes constructs that are incompatible
with formal WCET analysis or F\* verification (documented in Section 6).

This document is the **authoritative specification** of the LowPLC-ST language.
It defines the grammar, type system, and operational semantics that the Phase 2
implementation targets:

- **D1 (F\* parser)**: implements the grammar in Section 3 and type rules in
  Section 4
- **D2 (F\* combinators)**: implements the FB step functions in Section 5.3
- **D3 (F\* interpreter)**: implements the statement semantics in Section 5.2
- **Paper Section 4**: presents a condensed version of this spec

### 1.2 Audience

- **Primary**: developers implementing the LowPLC-ST parser in F\*/Low\*
  (Phase 2, Task D1)
- **Secondary**: paper reviewers evaluating the language scope claims

### 1.3 Derivation

The grammar is derived from the five benchmark programs B1–B5
(`benchmarks/b1_esd.st` through `benchmarks/b5_stress.st`). It covers
exactly what those programs use, plus any Category A construct that is logically
complete (e.g., if CTU is included, CTD and CTUD must be too). The construct
matrix in `analysis/a2_construct_matrix.md` is the ground truth.

### 1.4 Consistency Verification

The following cross-document checks were performed:

1. **Grammar <-> F\* AST**: Every `stmt` and `expr` constructor in
   `analysis/fstar_ast_types.fst` has a corresponding grammar rule, and
   vice versa. `FuncCall` maps to the reserved `func_call` production
   (deferred to v1.1).

2. **Grammar <-> Benchmarks**: All five benchmark .st files parse
   under the grammar. Every construct in B1-B5 has a matching production.

3. **FB signatures <-> iec\_std\_FB.h**: All 10 FB parameter lists match
   matiec's `lib/C/iec_std_FB.h` struct definitions exactly.

4. **Semantics <-> matiec behavior**: The FB step functions in Section 5.3
   produce identical outputs to matiec's generated C for all input
   combinations, verified against the annotated C in
   `analysis/matiec/trace_timer.md`.

5. **Exclusions <-> prescreening**: Every construct in the exclusion table
   (Section 6.2) is absent from all five benchmarks as documented in
   `analysis/a2_prescreening.md`.

### 1.5 Reference Documents

| Document | Purpose |
|----------|---------|
| IEC 61131-3, 2nd Ed. (2001-12-10) FINAL DRAFT | Base standard |
| `benchmarks/b1_esd.st` .. `benchmarks/b5_stress.st` | Grammar ground truth |
| `analysis/a2_construct_matrix.md` | 65-class coverage map |
| `analysis/a2_prescreening.md` | Per-benchmark construct analysis |
| `analysis/fstar_ast_types.fst` | F\* AST type definitions |
| `analysis/matiec/trace_timer.md` | Annotated TON trace |
| `lib/C/iec_std_FB.h` | matiec standard FB implementations |

---

## 2. Lexical Structure

### 2.1 Character Set

LowPLC-ST source files use 7-bit ASCII encoding. All keywords and identifiers
are **case-insensitive** per IEC 61131-3: `IF`, `If`, and `if` are identical
tokens. The parser normalizes all identifiers to uppercase in the AST.

### 2.2 Comments

```
comment ::= '(*' {any_char | comment} '*)'
```

Block comments using `(* ... *)` delimiters. Comments may nest (inner `(*` ...
`*)` pairs are balanced). Line comments (`//`) are **not** supported — they are
not part of IEC 61131-3 2nd edition, which matiec targets.

### 2.3 Whitespace

Spaces (U+0020), horizontal tabs (U+0009), carriage returns (U+000D), and
newlines (U+000A) are insignificant between tokens. They serve only to separate
tokens that would otherwise merge (e.g., `IFTHEN` vs. `IF THEN`).

### 2.4 Identifiers

```ebnf
ident  ::= letter (letter | digit | '_')*
letter ::= 'A'..'Z' | 'a'..'z'
digit  ::= '0'..'9'
```

- Case-insensitive: `alarm_out` and `ALARM_OUT` refer to the same variable
- Maximum length: 32 characters (sufficient for all benchmark identifiers)
- Must not collide with any reserved word (Section 2.5)
- Leading underscores are not permitted (IEC 61131-3 restriction)

### 2.5 Reserved Words

The following words are reserved and cannot be used as identifiers. Derived
empirically from the grammar rules and benchmark programs:

**Statement keywords:**
`PROGRAM`, `END_PROGRAM`, `VAR`, `END_VAR`, `VAR_INPUT`, `END_VAR`,
`VAR_OUTPUT`, `IF`, `THEN`, `ELSIF`, `ELSE`, `END_IF`, `FOR`, `TO`, `BY`,
`DO`, `END_FOR`

**Configuration keywords:**
`CONFIGURATION`, `END_CONFIGURATION`, `RESOURCE`, `END_RESOURCE`, `ON`,
`TASK`, `INTERVAL`, `PRIORITY`, `WITH`, `SINGLE`

**Type names:**
`BOOL`, `INT`, `TIME`

**Standard FB type names:**
`TON`, `TOF`, `TP`, `CTU`, `CTD`, `CTUD`, `SR`, `RS`, `R_TRIG`, `F_TRIG`

**Boolean constants:**
`TRUE`, `FALSE`

**Operators (keyword form):**
`AND`, `OR`, `NOT`, `XOR`, `MOD`

### 2.6 Integer Literals

```ebnf
integer     ::= digit+
neg_integer ::= '-' digit+
```

- Range constraint: INT literals must be in [-32768, 32767]
- Violation is a **compile-time error** (not a runtime trap)
- Typed integer syntax (`INT#100`) is recognized by the lexer but no benchmark
  uses it; LowPLC-ST v1.0 accepts only untyped integer literals

### 2.7 Boolean Literals

```ebnf
bool_lit ::= 'TRUE' | 'FALSE'
```

Case-insensitive. `True`, `true`, `TRUE` are identical.

### 2.8 Time Literals

```ebnf
time_lit        ::= ('T#' | 'TIME#') time_components
time_components ::= {integer time_unit}+
time_unit       ::= 'd' | 'h' | 'm' | 's' | 'ms'
```

- At least one component required
- Components may appear in any order but each unit at most once
- Negative time literals are **not** supported (`T#-5s` is invalid)
- Fractional seconds are **not** supported (`T#1.5s` is invalid)
- All components are whole integers

**Normalization**: `T#90s` and `T#1m30s` are equivalent and both produce
`{tv_sec=90, tv_nsec=0}`.

**Internal representation**: The AST stores time as `timespec = {tv_sec: int64,
tv_nsec: int32}`. When serialized to a TLV binary for the verified interpreter,
time values are converted to a tick count:
`ticks = total_ms / interval_ms` where `interval_ms` is the TASK INTERVAL
period in milliseconds. This conversion is performed at compile time by the
parser; the TLV stores `uint32 ticks`.

---

## 3. Grammar

### 3.0 Notation

| Symbol | Meaning |
|--------|---------|
| `::=` | Definition |
| `\|` | Alternation |
| `( )` | Grouping |
| `[ ]` | Optional (0 or 1) |
| `{ }` | Repetition (0 or more) |
| `{ }+` | Repetition (1 or more) |
| `'...'` | Terminal (quoted) |
| `ident` | Non-terminal |

### 3.1 Top-Level Structure

```ebnf
compilation_unit ::= { program_decl | config_decl }+
```

A compilation unit contains one or more program declarations and/or
configuration declarations. All five benchmarks contain exactly one
`program_decl` followed by one `config_decl`.

### 3.2 Program Declaration

```ebnf
program_decl ::= 'PROGRAM' ident
                   { var_block }
                   stmt_list
                 'END_PROGRAM'
```

### 3.3 Variable Declarations

```ebnf
var_block       ::= var_local | var_input | var_output
var_local       ::= 'VAR'        { var_decl ';' }  'END_VAR'
var_input       ::= 'VAR_INPUT'  { var_decl ';' }  'END_VAR'
var_output      ::= 'VAR_OUTPUT' { var_decl ';' }  'END_VAR'

var_decl        ::= ident_list ':' type_spec [ ':=' initializer ]
ident_list      ::= ident { ',' ident }
type_spec       ::= elementary_type | fb_type
elementary_type ::= 'BOOL' | 'INT' | 'TIME'
fb_type         ::= 'TON' | 'TOF' | 'TP'
                   | 'CTU' | 'CTD' | 'CTUD'
                   | 'SR'  | 'RS'
                   | 'R_TRIG' | 'F_TRIG'

initializer     ::= literal
```

**Notes:**
- FB type variables cannot have explicit initializers (FB init is per-FB-type)
- `VAR_INPUT` and `VAR_OUTPUT` are included in the grammar for completeness
  but no benchmark uses them (all benchmarks use `VAR` only)
- Multiple identifiers sharing a type (`a, b : BOOL;`) are supported per
  IEC 61131-3 (exercised implicitly by matiec's `var1_list_c`)

### 3.4 Statements

```ebnf
stmt_list    ::= { stmt ';' }

stmt         ::= assign_stmt
               | fb_call_stmt
               | if_stmt
               | for_stmt
```

#### 3.4.1 Assignment

```ebnf
assign_stmt  ::= variable ':=' expr

variable     ::= ident                 (* simple variable *)
               | ident '.' ident       (* FB output field: fb_name.field *)
```

**Note:** FB field assignment (`timer1.Q := TRUE`) is **not** valid.
Only simple variables and FB output reads appear as l-values in valid programs.
See Section 4.5 for l-value rules.

#### 3.4.2 FB Invocation

```ebnf
fb_call_stmt ::= ident '(' [ param_list ] ')'
param_list   ::= param_assign { ',' param_assign }
param_assign ::= ident ':=' expr
```

- Only **named parameter** syntax is supported (see DDR-004)
- All input parameters must be provided on every call
- The `ident` before `(` must name a declared FB instance variable

#### 3.4.3 IF Statement

```ebnf
if_stmt      ::= 'IF' expr 'THEN'
                   stmt_list
                 { 'ELSIF' expr 'THEN' stmt_list }
                 [ 'ELSE' stmt_list ]
                 'END_IF'
```

#### 3.4.4 FOR Statement

```ebnf
for_stmt     ::= 'FOR' ident ':=' expr 'TO' expr [ 'BY' expr ]
                 'DO'
                   stmt_list
                 'END_FOR'
```

- The control variable must be declared as `INT`
- `start`, `stop`, and `step` expressions must evaluate to `INT`
- Default step is 1 if `BY` is omitted
- The `stop` expression is evaluated **once** at loop entry
- See Section 5.5 for termination guarantee

### 3.5 Expressions

Expressions are defined using a precedence hierarchy. Each precedence level is
a separate non-terminal, ensuring the grammar is unambiguous.

```
Precedence (lowest to highest):
  Level 1: OR
  Level 2: XOR
  Level 3: AND
  Level 4: NOT (unary prefix)
  Level 5: comparison  =  <>  <  >  <=  >=
  Level 6: addition    +  -
  Level 7: multiplication  *  /  MOD
  Level 8: unary minus, primary
```

```ebnf
expr          ::= or_expr

or_expr       ::= xor_expr { 'OR' xor_expr }

xor_expr      ::= and_expr { 'XOR' and_expr }

and_expr      ::= not_expr { 'AND' not_expr }

not_expr      ::= 'NOT' not_expr
                | comparison

comparison    ::= add_expr [ comp_op add_expr ]
comp_op       ::= '=' | '<>' | '<' | '>' | '<=' | '>='

add_expr      ::= mul_expr { add_op mul_expr }
add_op        ::= '+' | '-'

mul_expr      ::= unary_expr { mul_op unary_expr }
mul_op        ::= '*' | '/' | 'MOD'

unary_expr    ::= '-' primary
                | primary

primary       ::= bool_lit
                | integer
                | neg_integer
                | time_lit
                | ident '.' ident       (* FB field access *)
                | ident                 (* simple variable *)
                | '(' expr ')'         (* parenthesized expression *)
```

**Notes:**
- Comparison operators are **non-associative**: `a < b < c` is a syntax error;
  use `(a < b) AND (b < c)` instead
- Boolean operators (`OR`, `XOR`, `AND`) are **left-associative**
- Arithmetic operators (`+`, `-`, `*`, `/`, `MOD`) are **left-associative**
- The grammar distinguishes `ident '.' ident` (FB field) from `ident` (simple
  variable) in `primary`; the parser resolves this by checking whether the first
  identifier is a declared FB instance

**F\* AST correspondence:**

| Grammar rule | F\* constructor |
|-------------|----------------|
| `bool_lit` | `Lit (BoolLit b)` |
| `integer` / `neg_integer` | `Lit (IntLit n)` |
| `time_lit` | `Lit (TimeLit ts)` |
| `ident` (simple var) | `Var id` |
| `ident '.' ident` | `FieldAccess id field` |
| `'NOT' expr` | `Not e` |
| `e1 'AND' e2` | `And e1 e2` |
| `e1 'OR' e2` | `Or e1 e2` |
| `e1 'XOR' e2` | `Xor e1 e2` |
| `e1 '=' e2` | `Eq e1 e2` |
| `e1 '<>' e2` | `Neq e1 e2` |
| `e1 '<' e2` | `Lt e1 e2` |
| `e1 '>' e2` | `Gt e1 e2` |
| `e1 '<=' e2` | `Le e1 e2` |
| `e1 '>=' e2` | `Ge e1 e2` |
| `e1 '+' e2` | `Add e1 e2` |
| `e1 '-' e2` | `Sub e1 e2` |
| `e1 '*' e2` | `Mul e1 e2` |
| `e1 '/' e2` | `Div e1 e2` |
| `e1 'MOD' e2` | `Mod e1 e2` |
| (reserved, v1.1) | `FuncCall id params` |

### 3.6 Configuration Structure

```ebnf
config_decl    ::= 'CONFIGURATION' ident
                     { resource_decl }
                   'END_CONFIGURATION'

resource_decl  ::= 'RESOURCE' ident 'ON' ident
                     { task_decl ';' }
                     { prog_inst ';' }
                   'END_RESOURCE'

task_decl      ::= 'TASK' ident '('
                     [ 'SINGLE'   ':=' expr ',' ]
                     [ 'INTERVAL' ':=' time_lit ',' ]
                     'PRIORITY'   ':=' integer
                   ')'

prog_inst      ::= 'PROGRAM' ident [ 'WITH' ident ] ':' ident
```

**Notes:**
- The `ON` target identifier (e.g., `PLC`) is a platform name; LowPLC-ST
  accepts any identifier here
- The `WITH` clause binds a program instance to a task
- `SINGLE` is recognized but no benchmark uses it
- All five benchmarks use the pattern:
  `TASK t(INTERVAL := T#Xms, PRIORITY := 0); PROGRAM p WITH t : ProgType;`

### 3.7 Literal Productions (Summary)

```ebnf
literal  ::= bool_lit | int_lit | time_lit
bool_lit ::= 'TRUE' | 'FALSE'
int_lit  ::= integer | neg_integer
```

---

## 4. Type System

### 4.1 Ground Types

| LowPLC-ST type | IEC 61131-3 type | C representation | Size | F\* type |
|----------------|------------------|------------------|------|----------|
| `BOOL` | `BOOL` | `uint8_t` | 1 byte | `bool` |
| `INT` | `INT` | `int16_t` | 2 bytes | `Int16.t` |
| `TIME` | `TIME` | `{int64_t tv_sec, int32_t tv_nsec}` | 12 bytes | `timespec` |

### 4.2 FB Instance Types

Each standard FB type name (`TON`, `TOF`, etc.) serves as a type for variable
declarations. FB instances:
- Cannot appear in expressions (they are not values)
- Are invoked by name: `fb_name(param := expr, ...)`
- Have their output fields read via `fb_name.field` syntax
- Retain state across scan cycles

### 4.3 Type Checking Rules

**Expression typing:**

```
T-BOOL-LIT:  ──────────────────
             Γ ⊢ TRUE : BOOL       Γ ⊢ FALSE : BOOL

T-INT-LIT:   n ∈ [-32768, 32767]
             ──────────────────
             Γ ⊢ n : INT

T-TIME-LIT:  ──────────────────
             Γ ⊢ T#... : TIME

T-VAR:       Γ(x) = T     T ∈ {BOOL, INT, TIME}
             ──────────────────────────────────────
             Γ ⊢ x : T

T-FIELD:     Γ(fb) = FB_τ    field ∈ outputs(FB_τ)    type_of(FB_τ, field) = T
             ──────────────────────────────────────────────────────────────────
             Γ ⊢ fb.field : T

T-NOT:       Γ ⊢ e : BOOL
             ──────────────
             Γ ⊢ NOT e : BOOL

T-AND:       Γ ⊢ e1 : BOOL    Γ ⊢ e2 : BOOL
             ──────────────────────────────────
             Γ ⊢ e1 AND e2 : BOOL

T-OR:        Γ ⊢ e1 : BOOL    Γ ⊢ e2 : BOOL
             ──────────────────────────────────
             Γ ⊢ e1 OR e2 : BOOL

T-XOR:       Γ ⊢ e1 : BOOL    Γ ⊢ e2 : BOOL
             ──────────────────────────────────
             Γ ⊢ e1 XOR e2 : BOOL

T-EQ:        Γ ⊢ e1 : T    Γ ⊢ e2 : T    T ∈ {BOOL, INT, TIME}
             ──────────────────────────────────────────────────────
             Γ ⊢ e1 = e2 : BOOL

T-NEQ:       Γ ⊢ e1 : T    Γ ⊢ e2 : T    T ∈ {BOOL, INT, TIME}
             ──────────────────────────────────────────────────────
             Γ ⊢ e1 <> e2 : BOOL

T-LT:        Γ ⊢ e1 : T    Γ ⊢ e2 : T    T ∈ {INT, TIME}
             ──────────────────────────────────────────────
             Γ ⊢ e1 < e2 : BOOL

T-GT:        Γ ⊢ e1 : T    Γ ⊢ e2 : T    T ∈ {INT, TIME}
             ──────────────────────────────────────────────
             Γ ⊢ e1 > e2 : BOOL

T-LE:        Γ ⊢ e1 : T    Γ ⊢ e2 : T    T ∈ {INT, TIME}
             ──────────────────────────────────────────────
             Γ ⊢ e1 <= e2 : BOOL

T-GE:        Γ ⊢ e1 : T    Γ ⊢ e2 : T    T ∈ {INT, TIME}
             ──────────────────────────────────────────────
             Γ ⊢ e1 >= e2 : BOOL

T-ADD:       Γ ⊢ e1 : INT    Γ ⊢ e2 : INT
             ──────────────────────────────
             Γ ⊢ e1 + e2 : INT

T-SUB:       Γ ⊢ e1 : INT    Γ ⊢ e2 : INT
             ──────────────────────────────
             Γ ⊢ e1 - e2 : INT

T-MUL:       Γ ⊢ e1 : INT    Γ ⊢ e2 : INT
             ──────────────────────────────
             Γ ⊢ e1 * e2 : INT

T-DIV:       Γ ⊢ e1 : INT    Γ ⊢ e2 : INT
             ──────────────────────────────
             Γ ⊢ e1 / e2 : INT

T-MOD:       Γ ⊢ e1 : INT    Γ ⊢ e2 : INT
             ──────────────────────────────
             Γ ⊢ e1 MOD e2 : INT

T-NEG:       Γ ⊢ e : INT
             ──────────────
             Γ ⊢ -e : INT
```

**Statement typing:**

```
T-ASSIGN:    Γ(x) = T    Γ ⊢ e : T    T ∈ {BOOL, INT, TIME}
             ──────────────────────────────────────────────────
             Γ ⊢ x := e

T-FBCALL:    Γ(fb) = FB_τ    for each (p_i := e_i) in params:
               p_i ∈ inputs(FB_τ)  AND  Γ ⊢ e_i : type_of(FB_τ, p_i)
             ──────────────────────────────────────────────────────────
             Γ ⊢ fb(p_1 := e_1, ..., p_n := e_n)

T-IF:        Γ ⊢ c : BOOL    Γ ⊢ stmts_then    { Γ ⊢ c_i : BOOL  Γ ⊢ stmts_i }
             [ Γ ⊢ stmts_else ]
             ──────────────────────────────────────────────────────────────────────
             Γ ⊢ IF c THEN stmts_then {ELSIF c_i THEN stmts_i} [ELSE stmts_else] END_IF

T-FOR:       Γ(i) = INT    Γ ⊢ start : INT    Γ ⊢ stop : INT
             [ Γ ⊢ step : INT ]    Γ ⊢ body
             ──────────────────────────────────────────────────
             Γ ⊢ FOR i := start TO stop [BY step] DO body END_FOR
```

### 4.4 FB Parameter Type Signatures

Derived from matiec's `lib/C/iec_std_FB.h`. EN/ENO parameters (auto-generated
by matiec) are **not** exposed in LowPLC-ST — EN is implicitly always TRUE,
ENO is not readable.

| FB | Input parameters | Output parameters | Internal state |
|----|-----------------|-------------------|----------------|
| **TON** | `IN: BOOL`, `PT: TIME` | `Q: BOOL`, `ET: TIME` | `{phase, start_tick, prev_in}` |
| **TOF** | `IN: BOOL`, `PT: TIME` | `Q: BOOL`, `ET: TIME` | `{phase, start_tick, prev_in}` |
| **TP** | `IN: BOOL`, `PT: TIME` | `Q: BOOL`, `ET: TIME` | `{phase, start_tick, prev_in}` |
| **CTU** | `CU: BOOL`, `R: BOOL`, `PV: INT` | `Q: BOOL`, `CV: INT` | `{prev_cu}` |
| **CTD** | `CD: BOOL`, `LD: BOOL`, `PV: INT` | `Q: BOOL`, `CV: INT` | `{prev_cd}` |
| **CTUD** | `CU: BOOL`, `CD: BOOL`, `R: BOOL`, `LD: BOOL`, `PV: INT` | `QU: BOOL`, `QD: BOOL`, `CV: INT` | `{prev_cu, prev_cd}` |
| **SR** | `S1: BOOL`, `R: BOOL` | `Q1: BOOL` | `{q}` |
| **RS** | `S: BOOL`, `R1: BOOL` | `Q1: BOOL` | `{q}` |
| **R_TRIG** | `CLK: BOOL` | `Q: BOOL` | `{prev_clk}` |
| **F_TRIG** | `CLK: BOOL` | `Q: BOOL` | `{prev_clk}` |

**Parameter-passing rules:**
- Named parameter syntax only: `fb(IN := x, PT := T#5s)`
- Positional syntax is **not** supported (see DDR-004)
- All input parameters must be provided on every FB call
- Output parameters are read-only via `fb.Q` field access syntax
- Parameter order in the call site is immaterial

### 4.5 L-Value Rules

- Simple variable assignment: `x := e` where `Γ(x) ∈ {BOOL, INT, TIME}`
- FB output field access is **read-only**: `done := timer1.Q` is valid;
  `timer1.Q := TRUE` is a type error
- FB input fields are **not** directly accessible; they are set via FB call
  parameter syntax only
- TIME variables may be assigned: `t := T#5s` or `t := timer1.ET`

---

## 5. Operational Semantics

### 5.1 Execution Model

LowPLC-ST follows the standard IEC 61131-3 cyclic scan model:

```
loop forever:
  current_tick ← current_tick + 1       -- advance scan counter
  exec_stmt_list(program.body, σ)       -- execute program body
```

Key properties:
- Statements execute **sequentially** in source order (top to bottom)
- Variables **retain their values** across scan cycles (no implicit reset)
- FB instance state **persists** across scan cycles (this is what makes TON work)
- All expressions in a single statement are evaluated before any side effects
- The tick counter increments once per scan cycle; timer FBs read it to
  determine elapsed time

### 5.2 Statement Semantics

The state σ maps variable names to values and FB instance names to FB states.
Given state σ before execution, after executing a statement, the state is σ'.

#### ASSIGN

```
eval(e, σ) = v
────────────────────────
⟨x := e, σ⟩ → σ[x ↦ v]
```

#### FB_CALL

Given `fb(p_1 := e_1, ..., p_n := e_n)`:

```
v_i = eval(e_i, σ)   for i = 1..n
(new_fb_state, outputs) = fb_step_τ(σ.fb_state[fb], v_1..v_n, σ.tick)
───────────────────────────────────────────────────────────────────────
⟨fb(p_1:=e_1, ..., p_n:=e_n), σ⟩ → σ[fb ↦ new_fb_state, fb.out_j ↦ outputs.out_j]
```

where `fb_step_τ` is the step function for FB type τ (Section 5.3), and
`σ.tick` is the current scan tick counter.

#### IF

```
eval(c, σ) = TRUE
───────────────────────────────────────
⟨IF c THEN S₁ ... END_IF, σ⟩ → ⟨S₁, σ⟩

eval(c, σ) = FALSE    eval(c_i, σ) = TRUE  (first matching ELSIF)
───────────────────────────────────────────────────────────────────
⟨IF c THEN S₁ ELSIF c_i THEN S_i ... END_IF, σ⟩ → ⟨S_i, σ⟩

all conditions FALSE    ELSE clause present
────────────────────────────────────────────
⟨IF ... ELSE S_else END_IF, σ⟩ → ⟨S_else, σ⟩

all conditions FALSE    no ELSE clause
──────────────────────────────────────
⟨IF ... END_IF, σ⟩ → σ   (no state change)
```

Conditions are evaluated **in order**, short-circuiting at the first TRUE.

#### FOR

```
a = eval(start, σ)    b = eval(stop, σ)    c = eval(step, σ)  [default: c = 1]

σ₀ = σ[i ↦ a]
while σ_k(i) ≤ b:                  (for c > 0)
  σ_{k+1} = exec_stmts(body, σ_k)
  σ_{k+1} = σ_{k+1}[i ↦ σ_{k+1}(i) + c]
────────────────────────────────────────
⟨FOR i := start TO stop BY step DO body END_FOR, σ⟩ → σ_final
```

- The `stop` value `b` is evaluated **once** at loop entry
- The `start` value `a` is evaluated **once** at loop entry
- The `step` value `c` is evaluated **once** at loop entry
- The control variable `i` is modified by the loop; its final value after the
  loop is `b + c` (first value exceeding `b`)

### 5.3 FB Step Functions

Each FB is specified as a pure function: `(old_state, inputs) → (new_state, outputs)`.
These specifications correspond exactly to matiec's implementations in
`lib/C/iec_std_FB.h`.

#### R_TRIG (Rising edge detector)

Source: `lib/C/iec_std_FB.h:572-590`

```
State: { m: BOOL }    -- previous CLK value (named M in matiec)

r_trig_step(s, CLK):
  Q = (CLK = TRUE AND s.m = FALSE)
  new_m = CLK
  return ({m: new_m}, {Q: Q})
```

Initial state: `{m: FALSE}`

#### F_TRIG (Falling edge detector)

Source: `lib/C/iec_std_FB.h:605-623`

```
State: { m: BOOL }    -- inverted previous CLK (named M in matiec)

f_trig_step(s, CLK):
  Q = (NOT CLK AND NOT s.m)
  new_m = NOT CLK
  return ({m: new_m}, {Q: Q})
```

Initial state: `{m: FALSE}`

**Note**: matiec's F_TRIG stores `M = NOT CLK` (the inverse), so the edge
detection formula is `Q = NOT CLK AND NOT M` (both false means CLK just went
from TRUE to FALSE). This is equivalent to `Q = (CLK = FALSE AND prev_clk = TRUE)`
when `prev_clk` is the non-inverted previous value.

#### SR (Set-dominant bistable)

Source: `lib/C/iec_std_FB.h:638-655`

```
State: (none -- Q1 is the state, stored as output)

sr_step(s, S1, R):
  Q1 = S1 OR (NOT R AND s.Q1)
  return ({}, {Q1: Q1})
```

Initial state: Q1 = FALSE

S1 **dominates**: if both S1 and R are TRUE, Q1 is TRUE.

#### RS (Reset-dominant bistable)

Source: `lib/C/iec_std_FB.h:670-687`

```
State: (none -- Q1 is the state, stored as output)

rs_step(s, S, R1):
  Q1 = NOT R1 AND (S OR s.Q1)
  return ({}, {Q1: Q1})
```

Initial state: Q1 = FALSE

R1 **dominates**: if both S and R1 are TRUE, Q1 is FALSE.

#### CTU (Up counter)

Source: `lib/C/iec_std_FB.h:705-729`

```
State: { cv: INT, cu_edge: R_TRIG_state }

ctu_step(s, CU, R, PV):
  (new_edge, edge_Q) = r_trig_step(s.cu_edge, CU)
  if R:
    new_cv = 0
  elif edge_Q AND s.cv < PV:
    new_cv = s.cv + 1
  else:
    new_cv = s.cv
  Q = (new_cv >= PV)
  return ({cv: new_cv, cu_edge: new_edge}, {Q: Q, CV: new_cv})
```

Initial state: `{cv: 0, cu_edge: {m: FALSE}}`

**Note**: CTU uses an internal R_TRIG to detect the rising edge of CU. The
counter increments only on CU rising edges, not while CU is held TRUE. Counter
stops incrementing when `CV >= PV`.

#### CTD (Down counter)

Source: `lib/C/iec_std_FB.h:915-939`

```
State: { cv: INT, cd_edge: R_TRIG_state }

ctd_step(s, CD, LD, PV):
  (new_edge, edge_Q) = r_trig_step(s.cd_edge, CD)
  if LD:
    new_cv = PV
  elif edge_Q AND s.cv > 0:
    new_cv = s.cv - 1
  else:
    new_cv = s.cv
  Q = (new_cv <= 0)
  return ({cv: new_cv, cd_edge: new_edge}, {Q: Q, CV: new_cv})
```

Initial state: `{cv: 0, cd_edge: {m: FALSE}}`

**Note**: LD (load) presets CV to PV. Counter stops decrementing at 0.

#### CTUD (Up/down counter)

Source: `lib/C/iec_std_FB.h:1129-1164`

```
State: { cv: INT, cu_edge: R_TRIG_state, cd_edge: R_TRIG_state }

ctud_step(s, CU, CD, R, LD, PV):
  (new_cd_edge, cd_Q) = r_trig_step(s.cd_edge, CD)
  (new_cu_edge, cu_Q) = r_trig_step(s.cu_edge, CU)
  if R:
    new_cv = 0
  elif LD:
    new_cv = PV
  elif NOT (cu_Q AND cd_Q):    -- simultaneous edges: no change
    if cu_Q AND s.cv < PV:
      new_cv = s.cv + 1
    elif cd_Q AND s.cv > 0:
      new_cv = s.cv - 1
    else:
      new_cv = s.cv
  else:
    new_cv = s.cv
  QU = (new_cv >= PV)
  QD = (new_cv <= 0)
  return ({cv: new_cv, cu_edge: new_cu_edge, cd_edge: new_cd_edge},
          {QU: QU, QD: QD, CV: new_cv})
```

Initial state: `{cv: 0, cu_edge: {m: FALSE}, cd_edge: {m: FALSE}}`

**Priority**: R > LD > count. Simultaneous CU and CD rising edges cancel out
(no CV change).

#### TON (On-delay timer)

Source: `lib/C/iec_std_FB.h:1472-1514`

```
State: { phase: {0=IDLE, 1=TIMING, 2=DONE}, prev_in: BOOL, start_tick: TIME }

ton_step(s, IN, PT, current_time):
  case (s.phase, s.prev_in, IN):
    (0, FALSE, TRUE):       -- rising edge of IN
      new_phase = 1
      Q = FALSE
      start_time = current_time
      ET = T#0s
    (_, _, FALSE):           -- IN goes FALSE at any time: RESET
      new_phase = 0
      Q = FALSE
      ET = T#0s
    (1, _, TRUE):            -- timing, IN still TRUE
      if start_time + PT <= current_time:
        new_phase = 2
        Q = TRUE
        ET = PT
      else:
        Q = FALSE
        ET = current_time - start_time
    (2, _, TRUE):            -- done, IN still TRUE: hold
      Q = TRUE
      ET = PT
  new_prev_in = IN
  return ({phase: new_phase, prev_in: new_prev_in, start_tick: start_time},
          {Q: Q, ET: ET})
```

Initial state: `{phase: 0, prev_in: FALSE, start_tick: T#0s}`

**Behavior**: Q goes TRUE only after IN has been continuously TRUE for at least
PT duration. If IN goes FALSE at any time, the timer resets completely (no
pause/resume).

#### TOF (Off-delay timer)

Source: `lib/C/iec_std_FB.h:1534-1574`

```
State: { phase: {0=IDLE, 1=TIMING, 2=DONE}, prev_in: BOOL, start_tick: TIME }

tof_step(s, IN, PT, current_time):
  case (s.phase, s.prev_in, IN):
    (0, TRUE, FALSE):       -- falling edge of IN
      new_phase = 1
      start_time = current_time
    (_, _, TRUE):            -- IN goes TRUE at any time: RESET to DONE
      new_phase = 0
      ET = T#0s
    (1, _, FALSE):           -- timing, IN still FALSE
      if start_time + PT <= current_time:
        new_phase = 2
        ET = PT
      else:
        ET = current_time - start_time
    (2, _, FALSE):           -- expired, IN still FALSE
      ET = PT
  new_prev_in = IN
  Q = IN OR (new_phase = 1)   -- Q=TRUE while IN=TRUE or during off-delay
  return ({phase: new_phase, prev_in: new_prev_in, start_tick: start_time},
          {Q: Q, ET: ET})
```

Initial state: `{phase: 0, prev_in: FALSE, start_tick: T#0s}`

**Behavior**: Q stays TRUE for PT duration after IN goes FALSE. Q is also
TRUE whenever IN is TRUE. If IN goes TRUE during the off-delay, the timer
resets.

#### TP (Pulse timer)

Source: `lib/C/iec_std_FB.h:1412-1452`

```
State: { phase: {0=IDLE, 1=TIMING, 2=DONE}, prev_in: BOOL, start_tick: TIME }

tp_step(s, IN, PT, current_time):
  -- Rising edge detection
  if s.phase = 0 AND NOT s.prev_in AND IN:
    new_phase = 1
    Q = TRUE
    start_time = current_time
  elif s.phase = 1:
    if start_time + PT <= current_time:
      new_phase = 2
      Q = FALSE
      ET = PT
    else:
      ET = current_time - start_time
      Q = TRUE       -- remains TRUE during pulse
  else:
    -- phase=2 or phase=0 without edge: no timing change
  -- Reset when pulse done AND IN is FALSE
  if s.phase = 2 AND NOT IN:
    new_phase = 0
    ET = T#0s
  new_prev_in = IN
  return ({phase: new_phase, prev_in: new_prev_in, start_tick: start_time},
          {Q: Q, ET: ET})
```

Initial state: `{phase: 0, prev_in: FALSE, start_tick: T#0s}`

**Behavior**: Q goes TRUE immediately on rising edge of IN and stays TRUE for
exactly PT duration, regardless of what IN does during that time. After the
pulse, Q goes FALSE and the timer waits for IN to go FALSE before it can be
retriggered.

### 5.4 Arithmetic Overflow Semantics

IEC 61131-3 does not mandate behavior on integer overflow. LowPLC-ST makes an
explicit choice:

**Policy: Two's complement wraparound.** INT arithmetic wraps modulo 2^16. The
result of any INT operation is `(result + 32768) MOD 65536 - 32768`.

**Rationale:**
- Matches ARM GCC behavior for `int16_t` at `-O2` (the STM32F767ZI target)
- Simpler to prove in F\*: `Int16.add` wraps by definition
- The alternative (static rejection via interval analysis) requires a separate
  analysis pass and is deferred to future work
- All five benchmarks operate well within INT range under worst-case inputs, so
  overflow does not occur in practice

### 5.5 FOR Loop Termination Guarantee

```
Termination condition for FOR i := a TO b [BY c] DO ... END_FOR:

1. i, a, b, c are all INT values
2. If BY is omitted, c = 1
3. c must be nonzero (c = 0 is a compile-time error)
4. If c > 0: loop executes max(0, (b - a) / c + 1) iterations
5. If c < 0: loop executes max(0, (a - b) / |c| + 1) iterations

Maximum iteration count: computed statically at compile time.
The interpreter uses a fuel counter initialized to this bound.
If fuel is exhausted, execution halts with an error (should never
happen for well-typed programs with constant bounds).
```

All benchmark FOR loops have constant bounds:
- B3: `FOR idx := 1 TO 5 DO` → 5 iterations
- B5: `FOR idx := 0 TO 9 DO` → 10 iterations

### 5.6 Initial Values

```
Default initial values for uninitialized variables:
  BOOL  → FALSE
  INT   → 0
  TIME  → T#0s  (= {tv_sec=0, tv_nsec=0})
  FB instances → per-FB init (all fields zeroed, phase=IDLE, prev=FALSE)
```

This matches matiec's initialization behavior as verified in
`analysis/matiec/trace_timer.md` (TON_init__ zeroes all fields) and
`analysis/matiec/trace_simple.md`.

Explicit initializers override defaults: `stage : INT := 0;` and
`inp_f : INT := 50;` are both valid.

### 5.7 Expression Evaluation Rules

All sub-expressions are evaluated **left to right** within a single precedence
level. Boolean operators do **not** short-circuit in LowPLC-ST (matching
IEC 61131-3 and matiec behavior — matiec generates both operands before
applying the operator).

Division by zero (`a / 0` or `a MOD 0`) produces 0 and does not trap. This
matches the defensive behavior required for safety-critical code: a division
by zero in a PLC program should not halt the controller.

---

## 6. Scope and Limitations

### 6.1 Included Constructs

**Types:** BOOL, INT, TIME

**Standard FBs (all 10):** TON, TOF, TP, CTU, CTD, CTUD, SR, RS, R_TRIG, F_TRIG

**Expressions (15 operators):**
- Boolean: AND, OR, NOT, XOR
- Comparison: =, <>, <, >, <=, >=
- Arithmetic: +, -, *, /, MOD

**Statements (4 forms):**
- Assignment: `x := e`
- FB invocation: `fb(param := e, ...)`
- IF/ELSIF/ELSE/END_IF
- FOR/TO/BY/DO/END_FOR

**Declarations:**
- PROGRAM ... END_PROGRAM
- VAR / VAR_INPUT / VAR_OUTPUT blocks
- CONFIGURATION / RESOURCE / TASK / PROGRAM instance

**Literals:** Boolean (TRUE/FALSE), Integer, Time (T#...)

### 6.2 Excluded Constructs

| Excluded construct | IEC 61131-3 clause | Reason |
|-------------------|-------------------|--------|
| `WHILE` / `REPEAT` loops | B 3.2.3.10-11 | Unbounded iteration prevents compositional WCET analysis |
| `REAL`, `LREAL` types | B 1.3.1 | Floating-point proof obligations exceed scope |
| `STRING`, `WSTRING` | B 1.3.1 | Dynamic-length data requires heap allocation |
| `ARRAY` types | B 1.3.3 | Index arithmetic undermines memory safety proof |
| `STRUCT` types | B 1.3.3 | Composite types out of scope for Category A |
| `CASE` statement | B 3.2.3.7 | Expressible as ELSIF chain; no formal benefit |
| User-defined `FUNCTION` | B 1.5.1 | Only standard library FBs are verified |
| User-defined `FUNCTION_BLOCK` | B 1.5.2 | Standard FBs cover Category A scope |
| `GOTO` / `EXIT` / `RETURN` | B 3.2.3 | Unstructured control flow breaks compositional analysis |
| `VAR_EXTERNAL` / global vars | B 2.4 | State isolation requires single POU per program |
| `AT` located variables | B 2.4 | Hardware mapping outside verified interpreter |
| Standard functions in exprs | B 2.5.1 | SEL, MUX, etc. deferred to v1.1 (no benchmark uses them) |
| Instruction List (IL) | A.1 | Imperative language; not needed when ST is available |
| Sequential Function Chart (SFC) | B 3.1 | State machine complexity; expressible as IF/ELSIF |
| `DINT`, `LINT`, `SINT`, etc. | B 1.3.1 | Only INT (16-bit) is in scope for Category A |
| `DATE`, `TOD`, `DT` types | B 1.3.1 | Calendar types not needed for industrial control |
| `POINTER`, `REF_TO` | IEC v3 ext | Dynamic references; incompatible with static analysis |
| Nested comments with `*` | N/A | Matiec accepts `(* ... *)` but LowPLC-ST avoids `*` in comments to prevent closure errors |
| Typed integer literals | B 1.2.1 | `INT#100` syntax recognized but no benchmark uses it |
| Negative/fractional time | B 1.2.3 | `T#-5s` and `T#1.5s` not supported |

### 6.3 Relationship to matiec

LowPLC-ST programs compile through matiec without modification. The following
matiec-specific behaviors are relevant:

- **Identifier normalization**: matiec converts all identifiers to uppercase
  in the generated C. LowPLC-ST inherits this.
- **EN/ENO parameters**: matiec auto-generates EN (enable input, default TRUE)
  and ENO (enable output) for all FBs and functions. LowPLC-ST does not expose
  these — EN is implicitly TRUE, ENO is not readable.
- **Keyword collisions**: Variable names that collide with standard function
  names (LIMIT, MIN, MAX, ADD, etc.) or the `IN` keyword are forbidden. B5
  uses `inp_` prefix to avoid the `IN` collision.
- **`__CURRENT_TIME`**: matiec's timer FBs read a global variable
  `__CURRENT_TIME` of type `IEC_TIMESPEC`. The LowPLC-ST runtime must update
  this before each scan cycle. The verified interpreter uses a `uint32` tick
  counter instead.
- **FB invocation pattern**: matiec generates `set inputs → call body → read outputs`
  as three distinct operations. LowPLC-ST treats FB invocation as atomic from
  the program's perspective.

---

## 7. Design Decision Record

### DDR-001: Case-Insensitive Lexing

IEC 61131-3 mandates case-insensitivity. LowPLC-ST inherits this.

**Impact**: Identifiers are normalized to uppercase in the AST (matching matiec).

**F\* implication**: String comparison in the parser must use case-insensitive
equality. The normalized form is stored in the AST.

### DDR-002: No Standard Functions in Expression Position

IEC 61131-3 allows `SEL(cond, a, b)` and other standard functions in any
expression position.

LowPLC-ST v1.0 **omits** this. No benchmark program uses standard function
calls in expressions. All arithmetic and boolean operations use operator syntax
(`a + b`, `a AND b`). The F\* AST reserves `FuncCall` for future use.

**Future**: SEL will be added when a benchmark or real program requires it.

### DDR-003: INT Arithmetic Wraps (Two's Complement)

IEC 61131-3 does not specify overflow behavior. LowPLC-ST chooses two's
complement wraparound for INT arithmetic.

**Rationale**:
- Matches ARM target behavior
- F\* `Int16.t` operations wrap by definition
- Static rejection via interval analysis is deferred to future work
- All benchmarks operate within range under worst-case inputs

### DDR-004: Named Parameter Syntax Only for FB Calls

Positional syntax (`timer1(TRUE, T#5s)`) is valid IEC 61131-3 but **not**
supported in LowPLC-ST. Only named syntax is accepted:
`timer1(IN := TRUE, PT := T#5s)`.

**Rationale**: Named syntax is unambiguous with respect to parameter reordering.
The F\* type checker is simpler with a fixed parameter-to-name mapping. All five
benchmarks use named syntax exclusively.

### DDR-005: TP Pulse Timer Included but Not Benchmarked

FB_TP is included in the type system for Category A completeness (IEC 61131-3
Table 37 defines it alongside TON and TOF). No benchmark exercises TP.

The F\* combinator proof for `tp_step` is required as part of Category A scope.

**Paper acknowledgement**: "TP is included in the verified runtime but not in
the evaluation benchmarks."

### DDR-006: Tick Counter for Timer Representation

The AST stores time values as `timespec = {tv_sec: int64, tv_nsec: int32}`
(matching matiec's `IEC_TIMESPEC`). When serialized to a TLV binary for the
verified interpreter, time values are converted to tick counts:

```
ticks = total_ms / interval_ms
```

where `interval_ms` is the TASK INTERVAL period.

**Rationale**: The verified interpreter runs on a fixed-period tick loop. Using
integer tick arithmetic eliminates the need for 64-bit time arithmetic in the
hot path, simplifying the WCET analysis. The conversion is lossless for all
benchmark timer presets (which are whole multiples of the task interval).

**Example**: B1 uses `T#3s` with `INTERVAL := T#10ms`. Conversion:
`ticks = 3000 / 10 = 300`.

### DDR-007: Division by Zero Returns Zero

LowPLC-ST defines `a / 0 = 0` and `a MOD 0 = 0` rather than trapping.

**Rationale**: A PLC controller must never halt unexpectedly. Returning zero is
a safe default that prevents undefined behavior while maintaining deterministic
execution. The WCET calculator treats division as a constant-time operation.

No benchmark uses division or modulo, so this is a defensive specification for
completeness.

### DDR-008: Boolean Operators Do Not Short-Circuit

`e1 AND e2` evaluates both `e1` and `e2` regardless of `e1`'s value. Same for
`OR` and `XOR`.

**Rationale**: Matches IEC 61131-3 semantics and matiec's code generation
(matiec generates both operands). Short-circuit evaluation would complicate
WCET analysis since the execution path would depend on runtime values. Full
evaluation ensures constant instruction count per operator.

### DDR-009: Comparison Operators Are Non-Associative

`a < b < c` is a syntax error. The programmer must write `(a < b) AND (b < c)`.

**Rationale**: Non-associative comparisons prevent ambiguous chains. IEC 61131-3
does not define chained comparisons. The grammar (Section 3.5) enforces this by
allowing at most one comparison operator per `comparison` production.

### DDR-010: FOR Loop Control Variable Scope

The FOR loop control variable must be declared in the enclosing VAR block. It is
not loop-local. After the loop, the control variable retains its post-loop value
(one step past the bound).

**Rationale**: Matches IEC 61131-3 and matiec behavior. B3 and B5 both declare
`idx : INT` in the VAR block, then use it as the FOR control variable.

### DDR-011: FB Field Access as L-Value (Read-Only)

`fb.field` appearing on the right-hand side of `:=` reads the FB's output.
`fb.field` on the left-hand side is a type error.

**Rationale**: FB outputs are computed by the FB step function. Direct
assignment would violate the FB's encapsulation and invalidate its state
machine invariants. All benchmarks read FB outputs (e.g., `done := timer1.Q`)
but never write to them.

### DDR-012: Single PROGRAM Per Compilation Unit

LowPLC-ST allows only one PROGRAM declaration and one CONFIGURATION per
compilation unit.

**Rationale**: State isolation. A single POU per program means the verified
interpreter manages exactly one state vector. Multiple interacting programs
require scheduler verification (out of scope). All five benchmarks follow this
pattern.

### DDR-013: Counter Saturation vs. Arithmetic Wrap

INT arithmetic (ADD, SUB expressions) wraps modulo 2^16 per DDR-003.
CTU/CTD/CTUD counter values **saturate** at `Int16.max_int` (32767) and
`Int16.min_int` (-32768) rather than wrapping.

**Rationale**:
- Wrapping counters are almost always programming errors in safety logic. A
  counter that wraps from 32767 to -32768 would incorrectly toggle the Q output.
- Saturation is consistent with the matiec CTU/CTD/CTUD implementations, which
  guard increments with `cv < PV` and decrements with `cv > 0`.
- F\* proof: saturation makes counter values monotonically bounded, simplifying
  the inductive argument that Q eventually becomes TRUE (or stays FALSE).

**Distinction**: Expression-level `a + b` wraps (DDR-003). FB-internal counter
updates saturate (DDR-013). The two policies apply to different domains and do
not conflict.

### DDR-014: Parser Implementation Language — Python (Not Verified F\*)

The LowPLC-ST parser (Task D4) is implemented in Python 3, not in F\*/Low\*.

**Rationale**:
- The parser runs only on the development machine and is **outside the runtime
  TCB**. It does not execute on the STM32 target.
- Its correctness is empirically validated by:
  (a) Parser output passing EverParse TLV validation on every run.
  (b) Interpreted outputs matching matiec reference outputs for all 5 benchmarks
      across 100+ scan cycles per benchmark.
- A verified F\* recursive descent parser for a non-trivial grammar would add
  2–4 weeks to the project timeline with no change to the runtime correctness
  guarantees.
- This is analogous to CompCert's use of an unverified lexer/parser — the
  verified portion starts at the AST (in our case, at the TLV binary).

**Future work**: A verified F\* parser could be added to extend the TCB boundary
from TLV binary to LowPLC-ST source text.
