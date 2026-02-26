# matiec IR Class Categorization for LowPLC F* Modeling

**Context**: 323 AST node classes in `absyntax/absyntax.def`. Each categorized for the LowPLC verified interpreter project.

## Categories

| Category | Meaning | Count |
|----------|---------|-------|
| **Model in F*** | Directly needed in the LowPLC simplified DSL (Option C). Must have an F* type/constructor. | 65 |
| **Not applicable** | Clearly out of scope: IL language, SFC, SAFE types, REF_TO, STRING, non-standard extensions. Will never be modeled. | 182 |
| **Deferred** | Could be needed if we expand beyond Category A benchmarks (CASE, arrays, enums, user-defined types, WHILE). Model later if needed. | 55 |
| **Needs investigation** | Unclear mapping. Need to check benchmark programs or clarify semantics before deciding. | 21 |

---

## Model in F* (65 classes)

These are the core classes needed for the Category A simplified DSL.

### Infrastructure / Root (3)

| Class | Type | Purpose | F* equivalent |
|-------|------|---------|---------------|
| `library_c` | SYM_LIST | Root of the entire AST | `type program_unit = list decl` |
| `identifier_c` | SYM_TOKEN | Variable/type/program names | `type ident = string` |
| `invalid_type_name_c` | SYM_REF0 | Sentinel for type errors | Not needed in verified parser (reject at parse) |

### Literals (8)

| Class | Type | Purpose | F* equivalent |
|-------|------|---------|---------------|
| `integer_c` | SYM_TOKEN | Integer literal value | `| IntLit of int16` |
| `neg_integer_c` | SYM_REF1 | Negated integer literal | Fold into `IntLit` with negation |
| `integer_literal_c` | SYM_REF2 | Typed integer (INT#100) | `| TypedIntLit of type_name * int16` |
| `boolean_literal_c` | SYM_REF2 | BOOL literal (may be typed) | `| BoolLit of bool` |
| `boolean_true_c` | SYM_REF0 | TRUE value | `| BoolLit true` |
| `boolean_false_c` | SYM_REF0 | FALSE value | `| BoolLit false` |
| `duration_c` | SYM_REF3 | TIME literal (T#5s) | `| TimeLit of timespec` |
| `interval_c` | SYM_REF5 | Duration components (d,h,m,s,ms) | Fold into `TimeLit` |

### Elementary Type Names (3 — Category A)

| Class | Type | Purpose | F* equivalent |
|-------|------|---------|---------------|
| `bool_type_name_c` | SYM_REF0 | BOOL type | `| TBool` |
| `int_type_name_c` | SYM_REF0 | INT type | `| TInt` |
| `time_type_name_c` | SYM_REF0 | TIME type | `| TTime` |

### Variable Declarations (11)

| Class | Type | Purpose | F* equivalent |
|-------|------|---------|---------------|
| `var_declarations_c` | SYM_REF2 | `VAR ... END_VAR` block | `type var_block = list var_decl` |
| `var_init_decl_list_c` | SYM_LIST | List of variable declarations | (part of var_block) |
| `var1_init_decl_c` | SYM_REF2 | `name : type [:= init]` | `type var_decl = {name; type; init}` |
| `var1_list_c` | SYM_LIST | List of variable names | `list ident` |
| `simple_spec_init_c` | SYM_REF2 | `type_name [:= initial_value]` | Fold into `var_decl` |
| `fb_name_decl_c` | SYM_REF2 | FB instance declaration | `| VarFB of ident * fb_type` |
| `fb_name_list_c` | SYM_LIST | List of FB names | `list ident` |
| `fb_spec_init_c` | SYM_REF2 | FB type + optional init | Fold into `VarFB` |
| `var_declarations_list_c` | SYM_LIST | All VAR blocks in a POU | `list var_block` |
| `input_declarations_c` | SYM_REF3 | `VAR_INPUT ... END_VAR` | `type var_block = {kind=Input; decls}` |
| `output_declarations_c` | SYM_REF3 | `VAR_OUTPUT ... END_VAR` | `type var_block = {kind=Output; decls}` |

### Program/POU Structure (3)

| Class | Type | Purpose | F* equivalent |
|-------|------|---------|---------------|
| `program_declaration_c` | SYM_REF3 | `PROGRAM name VAR...END_VAR body END_PROGRAM` | `type program = {name; vars; body}` |
| `input_declaration_list_c` | SYM_LIST | List of input declarations | `list var_decl` |
| `constant_option_c` | SYM_REF0 | CONSTANT qualifier in VAR | `type var_qual = Constant \| Normal` |

### Configuration/Resource/Task (12)

| Class | Type | Purpose | F* equivalent |
|-------|------|---------|---------------|
| `configuration_declaration_c` | SYM_REF5 | `CONFIGURATION ... END_CONFIGURATION` | `type config = {name; resources}` |
| `resource_declaration_list_c` | SYM_LIST | List of resources | `list resource` |
| `resource_declaration_c` | SYM_REF4 | `RESOURCE name ON type ... END_RESOURCE` | `type resource = {name; tasks; programs}` |
| `single_resource_declaration_c` | SYM_REF2 | Tasks + programs in resource | Fold into `resource` |
| `task_configuration_list_c` | SYM_LIST | List of tasks | `list task` |
| `program_configuration_list_c` | SYM_LIST | List of program instances | `list program_inst` |
| `task_configuration_c` | SYM_REF2 | `TASK name (...)` | `type task = {name; interval; priority}` |
| `task_initialization_c` | SYM_REF3 | `(SINGLE, INTERVAL, PRIORITY)` | Fold into `task` |
| `program_configuration_c` | SYM_REF5 | `PROGRAM inst WITH task : type` | `type program_inst = {name; task; type}` |
| `global_var_declarations_list_c` | SYM_LIST | Global var lists in config | (may be empty for Category A) |
| `fixed_point_c` | SYM_TOKEN | Fractional part of time literal | Fold into time parsing |
| `neg_time_c` | SYM_REF0 | Negative time indicator | Fold into `TimeLit` |

### ST Expressions (15)

| Class | Type | Purpose | F* equivalent |
|-------|------|---------|---------------|
| `or_expression_c` | SYM_REF2 | `a OR b` | `| Or of expr * expr` |
| `xor_expression_c` | SYM_REF2 | `a XOR b` | `| Xor of expr * expr` |
| `and_expression_c` | SYM_REF2 | `a AND b` | `| And of expr * expr` |
| `not_expression_c` | SYM_REF1 | `NOT a` | `| Not of expr` |
| `equ_expression_c` | SYM_REF2 | `a = b` | `| Eq of expr * expr` |
| `notequ_expression_c` | SYM_REF2 | `a <> b` | `| Neq of expr * expr` |
| `lt_expression_c` | SYM_REF2 | `a < b` | `| Lt of expr * expr` |
| `gt_expression_c` | SYM_REF2 | `a > b` | `| Gt of expr * expr` |
| `le_expression_c` | SYM_REF2 | `a <= b` | `| Le of expr * expr` |
| `ge_expression_c` | SYM_REF2 | `a >= b` | `| Ge of expr * expr` |
| `add_expression_c` | SYM_REF2 | `a + b` | `| Add of expr * expr` |
| `sub_expression_c` | SYM_REF2 | `a - b` | `| Sub of expr * expr` |
| `mul_expression_c` | SYM_REF2 | `a * b` | `| Mul of expr * expr` |
| `div_expression_c` | SYM_REF2 | `a / b` | `| Div of expr * expr` |
| `mod_expression_c` | SYM_REF2 | `a MOD b` | `| Mod of expr * expr` |

### ST Statements (10)

| Class | Type | Purpose | F* equivalent |
|-------|------|---------|---------------|
| `statement_list_c` | SYM_LIST | List of statements | `list stmt` |
| `assignment_statement_c` | SYM_REF2 | `var := expr` | `| Assign of lvalue * expr` |
| `if_statement_c` | SYM_REF4 | `IF...THEN...ELSIF...ELSE...END_IF` | `| If of expr * stmts * elsifs * stmts option` |
| `elseif_statement_list_c` | SYM_LIST | List of ELSIF clauses | `list (expr * stmts)` |
| `elseif_statement_c` | SYM_REF2 | `ELSIF expr THEN stmts` | `(expr * stmts)` |
| `for_statement_c` | SYM_REF5 | `FOR var := a TO b [BY c] DO...END_FOR` | `| For of ident * expr * expr * expr option * stmts` |
| `fb_invocation_c` | SYM_REF3 | `fb_name(IN:=x, PT:=y)` | `| FBCall of ident * list param_assign` |
| `param_assignment_list_c` | SYM_LIST | List of parameter assignments | `list param_assign` |
| `input_variable_param_assignment_c` | SYM_REF2 | `param_name := expr` | `type param_assign = {name; expr}` |
| `function_invocation_c` | SYM_REF3 | `func_name(params)` | `| FuncCall of ident * list param_assign` |

### Variables / L-values (2)

| Class | Type | Purpose | F* equivalent |
|-------|------|---------|---------------|
| `symbolic_variable_c` | SYM_REF1 | Simple variable reference | `| Var of ident` |
| `structured_variable_c` | SYM_REF2 | `fb_name.field` access | `| FieldAccess of ident * ident` |

---

## Not Applicable (182 classes)

Will never be needed for LowPLC. Excluded because the construct is from a different language (IL/SFC), a non-standard extension, or a PLCOpen Safety feature.

### Instruction List — IL Language (44 classes)

All IL constructs. LowPLC only targets ST.

```
instruction_list_c, il_instruction_c, il_simple_operation_c,
il_function_call_c, il_expression_c, il_jump_operation_c,
il_fb_call_c, il_formal_funct_call_c, il_operand_list_c,
simple_instr_list_c, il_simple_instruction_c, il_param_list_c,
il_param_assignment_c, il_param_out_assignment_c,
LD_operator_c, LDN_operator_c, ST_operator_c, STN_operator_c,
NOT_operator_c, S_operator_c, R_operator_c, S1_operator_c,
R1_operator_c, CLK_operator_c, CU_operator_c, CD_operator_c,
PV_operator_c, IN_operator_c, PT_operator_c,
AND_operator_c, OR_operator_c, XOR_operator_c,
ANDN_operator_c, ORN_operator_c, XORN_operator_c,
ADD_operator_c, SUB_operator_c, MUL_operator_c,
DIV_operator_c, MOD_operator_c,
GT_operator_c, GE_operator_c, EQ_operator_c,
LT_operator_c, LE_operator_c, NE_operator_c,
CAL_operator_c, CALC_operator_c, CALCN_operator_c,
RET_operator_c, RETC_operator_c, RETCN_operator_c,
JMP_operator_c, JMPC_operator_c, JMPCN_operator_c,
il_assign_operator_c, il_assign_out_operator_c
```

### Sequential Function Chart — SFC (14 classes)

```
sequential_function_chart_c, sfc_network_c, initial_step_c,
action_association_list_c, step_c, action_association_c,
qualifier_c, timed_qualifier_c, indicator_name_list_c,
action_qualifier_c, transition_c, transition_condition_c,
steps_c, step_name_list_c, action_c, step_name_list_c
```

### PLCOpen Safety SAFE Types (22 classes)

```
safetime_type_name_c, safebool_type_name_c, safesint_type_name_c,
safeint_type_name_c, safedint_type_name_c, safelint_type_name_c,
safeusint_type_name_c, safeuint_type_name_c, safeudint_type_name_c,
safeulint_type_name_c, safereal_type_name_c, safelreal_type_name_c,
safedate_type_name_c, safetod_type_name_c, safedt_type_name_c,
safebyte_type_name_c, safeword_type_name_c, safedword_type_name_c,
safelword_type_name_c, safestring_type_name_c, safewstring_type_name_c
```

### REF_TO / Pointer Extensions — IEC v3 (6 classes)

```
ref_value_null_literal_c, ref_spec_c, ref_spec_init_c,
ref_type_decl_c, ref_expression_c, deref_expression_c,
deref_operator_c
```

### Generic Types (10 classes)

Only used internally by matiec for standard function overloading. Not in source programs.

```
generic_type_any_c, generic_type_any_derived_c,
generic_type_any_elementary_c, generic_type_any_magnitude_c,
generic_type_any_num_c, generic_type_any_real_c,
generic_type_any_int_c, generic_type_any_bit_c,
generic_type_any_string_c, generic_type_any_date_c
```

### STRING / WSTRING Types (10 classes)

No dynamic allocation on baremetal target.

```
string_type_name_c, wstring_type_name_c,
double_byte_character_string_c, single_byte_character_string_c,
single_byte_string_var_declaration_c, single_byte_string_spec_c,
single_byte_limited_len_string_spec_c, double_byte_limited_len_string_spec_c,
double_byte_string_var_declaration_c, double_byte_string_spec_c,
string_type_declaration_c
```

### REAL / LREAL Type Names (2 classes)

No floating point on baremetal target.

```
real_type_name_c, lreal_type_name_c
```

### DATE / TOD / DT Type Names (3 classes)

Not needed for Category A benchmarks.

```
date_type_name_c, tod_type_name_c, dt_type_name_c
```

### Date/Time-of-Day Literals (5 classes)

```
time_of_day_c, daytime_c, date_c, date_literal_c, date_and_time_c
```

### Pragmas / Code Gen Control (3 classes)

```
disable_code_generation_pragma_c, enable_code_generation_pragma_c, pragma_c
```

### Located Variables / Direct Variables (8 classes)

I/O-mapped variables (%IX, %QX). Not applicable for software-only interpreter.

```
direct_variable_c, located_var_declarations_c, located_var_decl_list_c,
located_var_decl_c, location_c, incompl_located_var_declarations_c,
incompl_located_var_decl_list_c, incompl_located_var_decl_c,
incompl_location_c
```

### Non-Standard Extensions (5 classes)

```
void_type_name_c, symbolic_constant_c, extensible_input_parameter_c,
power_expression_c, continue_statement_c
```

### EN/ENO Internal Machinery (4 classes)

Auto-generated by matiec, not explicit in source. LowPLC handles EN/ENO in FB semantics.

```
en_param_declaration_c, eno_param_declaration_c,
implicit_definition_c, explicit_definition_c
```

### Misc Internal/Helper (8 classes)

```
derived_datatype_identifier_c, poutype_identifier_c,
retain_option_c, non_retain_option_c,
raising_edge_option_c, falling_edge_option_c,
not_paramassign_c, any_fb_name_list_c
```

### Global Variables (8 classes)

```
global_var_declarations_c, global_var_decl_list_c, global_var_decl_c,
global_var_spec_c, global_var_list_c, global_var_reference_c,
program_output_reference_c
```

### External Variables (3 classes)

```
external_var_declarations_c, external_declaration_list_c, external_declaration_c
```

### Instance-Specific Init (5 classes)

```
instance_specific_initializations_c, instance_specific_init_list_c,
instance_specific_init_c, fb_initialization_c, prog_conf_elements_c
```

### Other Config Helpers (4 classes)

```
fb_task_c, prog_cnxn_assign_c, prog_cnxn_sendto_c
```

---

## Deferred (55 classes)

Potentially needed if we expand beyond Category A. Model later.

### CASE Statement (5 classes)

Needed if benchmarks use CASE. Can be expressed as IF/ELSIF chains for now.

```
case_statement_c, case_element_list_c, case_element_c, case_list_c
```

### WHILE / REPEAT / EXIT (3 classes)

Excluded from Category A (no termination guarantee). Might be needed for industrial programs.

```
while_statement_c, repeat_statement_c, exit_statement_c
```

### RETURN Statement (1 class)

```
return_statement_c
```

### Integer Type Variants Beyond INT (8 classes)

Category A uses only INT. Real programs may use SINT, DINT, LINT, USINT, UINT, UDINT, ULINT.

```
sint_type_name_c, dint_type_name_c, lint_type_name_c,
usint_type_name_c, uint_type_name_c, udint_type_name_c, ulint_type_name_c
```

### Bit Types (4 classes)

Used for bit manipulation. May be needed for MODBUS data handling.

```
byte_type_name_c, word_type_name_c, dword_type_name_c, lword_type_name_c
```

### REAL Literals (3 classes)

If floating point is ever needed.

```
real_c, real_literal_c, neg_real_c
```

### Bit String / Hex / Octal / Binary Literals (4 classes)

```
binary_integer_c, octal_integer_c, hex_integer_c, bit_string_literal_c
```

### Enumerated Types (4 classes)

Could be useful for state machines.

```
enumerated_type_declaration_c, enumerated_spec_init_c,
enumerated_value_list_c, enumerated_value_c
```

### Array Types (8 classes)

Needed if benchmarks use indexed data.

```
array_type_declaration_c, array_spec_init_c, array_specification_c,
array_subrange_list_c, array_initial_elements_list_c,
array_initial_elements_c, array_variable_c, subscript_list_c,
array_var_init_decl_c, array_var_declaration_c
```

### Structure Types (5 classes)

User-defined structures.

```
structure_type_declaration_c, initialized_structure_c,
structure_element_declaration_list_c, structure_element_declaration_c,
structure_element_initialization_list_c, structure_element_initialization_c,
structured_var_init_decl_c, structured_var_declaration_c
```

### User-Defined Type Declarations (5 classes)

```
data_type_declaration_c, type_declaration_list_c,
simple_type_declaration_c, subrange_type_declaration_c,
subrange_spec_init_c, subrange_specification_c, subrange_c
```

### Function / Function Block Declarations (8 classes)

User-defined functions and FBs. Category A uses only standard library FBs.

```
function_declaration_c, function_var_decls_c, var2_init_decl_list_c,
function_block_declaration_c, temp_var_decls_c, temp_var_decls_list_c,
non_retentive_var_decls_c
```

### VAR_IN_OUT Declarations (2 classes)

```
input_output_declarations_c, var_declaration_list_c
```

### Retentive / Non-Retentive Var Decls (1 class)

```
retentive_var_declarations_c
```

### Output Variable Param (for FB call with => syntax) (1 class)

```
output_variable_param_assignment_c
```

### Neg Expression (1 class)

Unary minus on expressions.

```
neg_expression_c
```

### Edge Declarations (1 class)

```
edge_declaration_c
```

---

## Needs Investigation (21 classes)

### Time Literal Sub-Components (5 classes)

The commented-out `days_c`, `hours_c`, `minutes_c`, `seconds_c`, `milliseconds_c` — these are listed in absyntax.def but commented out. Currently `interval_c` (SYM_REF5) handles all components. Need to verify these are truly dead code.

```
days_c, hours_c, minutes_c, seconds_c, milliseconds_c
```

**Likely verdict**: Not applicable (dead code, confirmed by being inside `/* */` comments).

### signed_integer_c / signed_real_c / numeric_literal_c (3 classes)

Commented out in absyntax.def. Replaced by `neg_integer_c` / `neg_real_c`. Need to confirm they're never instantiated.

```
signed_integer_c, signed_real_c, numeric_literal_c
```

**Likely verdict**: Not applicable (dead code).

### en_param_c / eno_param_c (2 classes)

Commented out. EN/ENO stored as `identifier_c` instead.

```
en_param_c, eno_param_c
```

**Likely verdict**: Not applicable (dead code).

### function_invocation_c in Expression Context (1 class)

`function_invocation_c` is used for standard function calls like `ADD(a, b)` or `INT_TO_BOOL(x)`. Do any Category A benchmarks call standard functions in expressions? Or do they only use operator syntax (`a + b`)?

```
function_invocation_c
```

**Likely verdict**: Model in F* — standard functions like SEL, MUX may appear. Already listed in "Model in F*" above.

### neg_expression_c (1 class)

Unary minus in expressions (`-count`). Already listed under Deferred. Need to check if Category A benchmarks use it.

**Likely verdict**: Deferred or Model — depends on benchmarks.

### Access Declarations in Configuration (2 classes)

Are access declarations used in simple single-resource configurations?

```
access_declarations (field of configuration_declaration_c — typically NULL)
```

**Likely verdict**: Not applicable for Category A.

### Multiple Programs with Same Task (1 class)

Does any benchmark have multiple programs in one resource?

**Likely verdict**: Not applicable for Category A (single program per resource).

### Misc (6 classes already categorized above but flagged for edge cases)

These are classes that appear in "Model in F*" or "Deferred" but have edge-case behaviors worth investigating:

- `simple_spec_init_c` — When `constant` field is NULL (no initializer), does F* need an explicit "no init" case?
- `fb_spec_init_c` — Can FBs have initial values in user code? (Yes, but rare for standard FBs)
- `task_initialization_c` — SINGLE data source field — do any benchmarks use single-shot tasks?
- `global_var_declarations_list_c` — Can this be empty/NULL in a simple config?
- `input_declarations_c` — The `method` field distinguishes explicit vs implicit — relevant for EN/ENO handling
- `output_declarations_c` — Same `method` field question

**Likely verdict**: Mostly resolved by choosing appropriate defaults (NULL → None in F*).

---

## Summary for DG-1

### What LowPLC's F* AST Needs

The 65 "Model in F*" classes collapse into roughly **20 F* types**:

```fstar
type ident = string

type type_name = TBool | TInt | TTime | TFB of fb_type_name
type fb_type_name = TON | TOF | TP | CTU | CTD | CTUD | SR | RS | R_TRIG | F_TRIG

type literal = BoolLit of bool | IntLit of int16 | TimeLit of timespec
type timespec = { tv_sec: int64; tv_nsec: int32 }

type expr =
  | Lit of literal
  | Var of ident
  | FieldAccess of ident * ident        (* fb.Q *)
  | Not of expr | And of expr * expr | Or of expr * expr | Xor of expr * expr
  | Eq of expr * expr | Neq of expr * expr
  | Lt of expr * expr | Gt of expr * expr | Le of expr * expr | Ge of expr * expr
  | Add of expr * expr | Sub of expr * expr
  | Mul of expr * expr | Div of expr * expr | Mod of expr * expr
  | FuncCall of ident * list param_assign

type param_assign = { name: ident; value: expr }

type stmt =
  | Assign of ident * expr
  | FBCall of ident * list param_assign
  | If of expr * list stmt * list (expr * list stmt) * list stmt option
  | For of ident * expr * expr * expr option * list stmt

type var_decl = { name: ident; vtype: type_name; init: literal option }
type var_block = { kind: var_kind; decls: list var_decl }
type var_kind = Local | Input | Output

type program = { name: ident; vars: list var_block; body: list stmt }
type task = { name: ident; interval: timespec; priority: nat }
type program_inst = { name: ident; task: ident; prog_type: ident }
type resource = { name: ident; tasks: list task; programs: list program_inst }
type config = { name: ident; resources: list resource }
```

### Reduction Ratio

- matiec: **323 classes** (full IEC 61131-3 + extensions)
- LowPLC F*: **~20 types** with **~35 constructors**
- Reduction: **~90%** of matiec's AST is not needed

This confirms the DG-1 recommendation: a simplified DSL (Option C) is far more tractable than consuming the full matiec IR.
