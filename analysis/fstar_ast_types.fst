(**
 * LowPLC.AST — Verified AST for LowPLC Structured Text subset
 *
 * This module defines the core abstract syntax tree for the LowPLC verified
 * interpreter. It models the 65 "Model in F*" matiec AST classes as ~20 F*
 * types with ~35 constructors — a 90% reduction from matiec's full 323-class
 * AST.
 *
 * Source: analysis/matiec/ir_class_categories.md
 * Covers: IEC 61131-3 Category A subset (BOOL, INT, TIME; standard FBs;
 *         ST expressions and statements; PROGRAM/CONFIGURATION structure)
 *)
module LowPLC.AST

open FStar.Int16
open FStar.Int32
open FStar.Int64

(** {1 Identifiers} *)

(** Variable, type, program, and resource names.
    Maps to matiec: identifier_c (SYM_TOKEN) *)
type ident = string

(** {1 Type System} *)

(** Standard function block type names.
    Maps to matiec FB types resolved during semantic analysis (stage 3).
    These are the 9 standard FB types from IEC 61131-3 Table 37-39:
    - Bistable: SR, RS
    - Edge detection: R_TRIG, F_TRIG
    - Counters: CTU, CTD, CTUD
    - Timers: TON, TOF, TP *)
type fb_type_name =
  | FB_TON    (** On-delay timer — matiec TON standard FB *)
  | FB_TOF    (** Off-delay timer — matiec TOF standard FB *)
  | FB_TP     (** Pulse timer — matiec TP standard FB.
                   Coverage gap: TP is not exercised by any benchmark (B1–B5).
                   Verification proof is required but lacks runtime measurement
                   data.  Consider adding a B6 micro-benchmark if TP coverage
                   becomes necessary. *)
  | FB_CTU    (** Up counter — matiec CTU standard FB *)
  | FB_CTD    (** Down counter — matiec CTD standard FB *)
  | FB_CTUD   (** Up/down counter — matiec CTUD standard FB *)
  | FB_SR     (** Set-dominant bistable — matiec SR standard FB *)
  | FB_RS     (** Reset-dominant bistable — matiec RS standard FB *)
  | FB_R_TRIG (** Rising edge detector — matiec R_TRIG standard FB *)
  | FB_F_TRIG (** Falling edge detector — matiec F_TRIG standard FB *)

(** Elementary and FB type names.
    Maps to matiec: bool_type_name_c, int_type_name_c, time_type_name_c,
    plus FB type references resolved from fb_spec_init_c *)
type type_name =
  | TBool          (** BOOL — matiec bool_type_name_c *)
  | TInt           (** INT (16-bit signed) — matiec int_type_name_c *)
  | TTime          (** TIME — matiec time_type_name_c *)
  | TFB of fb_type_name  (** Function block instance type *)

(** {1 Time Representation} *)

(** Time value representation for IEC 61131-3 TIME type.

    Design note: We use a seconds + nanoseconds pair rather than a single
    tick counter for two reasons:
    1. It directly corresponds to the IEC TIME literal syntax (T#3s, T#500ms)
       and matiec's internal duration_c / interval_c representation.
    2. The LowPLC interpreter converts to a tick count at runtime using the
       task's INTERVAL as the tick period. This separation keeps the AST
       target-independent while allowing the interpreter to use efficient
       integer tick arithmetic.

    Maps to matiec: duration_c (SYM_REF3), interval_c (SYM_REF5),
    fixed_point_c (fractional seconds), neg_time_c (sign indicator) *)
type timespec = {
  tv_sec:  Int64.t;   (** Whole seconds component *)
  tv_nsec: Int32.t;   (** Nanoseconds component (0 to 999_999_999) *)
}

(** {1 Literals} *)

(** Literal values in ST expressions.
    Maps to matiec: integer_c, neg_integer_c, integer_literal_c → IntLit;
    boolean_literal_c, boolean_true_c, boolean_false_c → BoolLit;
    duration_c, interval_c → TimeLit *)
type literal =
  | BoolLit of bool        (** Boolean literal — matiec boolean_true_c / boolean_false_c *)
  | IntLit  of Int16.t     (** Integer literal — matiec integer_c, neg_integer_c, integer_literal_c *)
  | TimeLit of timespec    (** Time literal — matiec duration_c + interval_c *)

(** {1 Expressions} *)

(** Named parameter assignment in FB/function calls.
    Maps to matiec: input_variable_param_assignment_c (SYM_REF2),
    within param_assignment_list_c (SYM_LIST) *)
type param_assign = {
  name:  ident;   (** Parameter name (e.g., "IN", "PT", "S1") *)
  value: expr;    (** Expression assigned to parameter *)
}

(** ST expressions — the core expression language.
    Maps to matiec ST expression classes in stage1_2/iec_bison.yy.

    Boolean operators:
      or_expression_c, xor_expression_c, and_expression_c, not_expression_c
    Comparison operators:
      equ_expression_c, notequ_expression_c, lt_expression_c,
      gt_expression_c, le_expression_c, ge_expression_c
    Arithmetic operators:
      add_expression_c, sub_expression_c, mul_expression_c,
      div_expression_c, mod_expression_c
    Variable references:
      symbolic_variable_c, structured_variable_c
    Function calls:
      function_invocation_c *)
and expr =
  | Lit         of literal              (** Literal value *)
  | Var         of ident                (** Simple variable — matiec symbolic_variable_c *)
  | FieldAccess of ident *ident        (** FB field access (fb.Q) — matiec structured_variable_c *)
  (* Boolean operators *)
  | Not of expr                         (** NOT a — matiec not_expression_c *)
  | And of expr *expr                  (** a AND b — matiec and_expression_c *)
  | Or  of expr *expr                  (** a OR b — matiec or_expression_c *)
  | Xor of expr *expr                  (** a XOR b — matiec xor_expression_c *)
  (* Comparison operators *)
  | Eq  of expr *expr                  (** a = b — matiec equ_expression_c *)
  | Neq of expr *expr                  (** a <> b — matiec notequ_expression_c *)
  | Lt  of expr *expr                  (** a < b — matiec lt_expression_c *)
  | Gt  of expr *expr                  (** a > b — matiec gt_expression_c *)
  | Le  of expr *expr                  (** a <= b — matiec le_expression_c *)
  | Ge  of expr *expr                  (** a >= b — matiec ge_expression_c *)
  (* Arithmetic operators *)
  | Add of expr *expr                  (** a + b — matiec add_expression_c *)
  | Sub of expr *expr                  (** a - b — matiec sub_expression_c *)
  | Mul of expr *expr                  (** a * b — matiec mul_expression_c *)
  | Div of expr *expr                  (** a / b — matiec div_expression_c *)
  | Mod of expr *expr                  (** a MOD b — matiec mod_expression_c *)
  (* Function calls *)
  | FuncCall    of ident *list param_assign  (** Function call — matiec function_invocation_c *)

(** {1 Statements} *)

(** ST statements — the core statement language.
    Maps to matiec: assignment_statement_c, fb_invocation_c,
    if_statement_c (with elseif_statement_list_c, elseif_statement_c),
    for_statement_c, statement_list_c *)
type stmt =
  | Assign of ident *expr
    (** Variable assignment: var := expr
        Maps to matiec assignment_statement_c *)
  | FBCall of ident *list param_assign
    (** FB invocation: fb_name(param := expr, ...)
        Maps to matiec fb_invocation_c + param_assignment_list_c *)
  | If     of expr *list stmt *list (expr *list stmt) *option (list stmt)
    (** IF cond THEN stmts {ELSIF cond THEN stmts} [ELSE stmts] END_IF
        Maps to matiec if_statement_c, elseif_statement_list_c, elseif_statement_c *)
  | For    of ident *expr *expr *option expr *list stmt
    (** FOR var := start TO stop [BY step] DO stmts END_FOR
        Maps to matiec for_statement_c *)

(** {1 Variable Declarations} *)

(** Variable declaration within a VAR block.
    Maps to matiec: var1_init_decl_c (SYM_REF2) with var1_list_c (names),
    simple_spec_init_c (type + init) for elementary types,
    fb_name_decl_c + fb_spec_init_c for FB instances *)
type var_decl = {
  vname: ident;               (** Variable name — matiec var1_list_c entry *)
  vtype: type_name;           (** Type — matiec simple_spec_init_c or fb_spec_init_c *)
  init:  option literal;      (** Optional initial value — matiec simple_spec_init_c ref2 *)
}

(** Variable block qualifier.
    Maps to matiec: var_declarations_c (Local), input_declarations_c (Input),
    output_declarations_c (Output). constant_option_c modifies these but is
    not modeled separately — Category A programs do not use CONSTANT. *)
type var_kind =
  | Local    (** VAR ... END_VAR — matiec var_declarations_c *)
  | Input    (** VAR_INPUT ... END_VAR — matiec input_declarations_c *)
  | Output   (** VAR_OUTPUT ... END_VAR — matiec output_declarations_c *)

(** A variable declaration block (VAR/VAR_INPUT/VAR_OUTPUT).
    Maps to matiec: var_declarations_c, input_declarations_c, output_declarations_c,
    with var_init_decl_list_c as the declaration list.
    The collection of all blocks maps to var_declarations_list_c. *)
type var_block = {
  kind:  var_kind;           (** Block qualifier *)
  decls: list var_decl;      (** Declarations in this block *)
}

(** {1 Program Structure} *)

(** Program declaration — the primary POU type for Category A.
    Maps to matiec: program_declaration_c (SYM_REF3) with
    var_declarations_list_c (variables) and statement_list_c (body).
    input_declaration_list_c maps to the list of var_blocks. *)
type program = {
  pname: ident;              (** Program name *)
  vars:  list var_block;     (** Variable declaration blocks *)
  body:  list stmt;          (** Statement body *)
}

(** {1 Configuration Structure} *)

(** Task configuration within a resource.
    Maps to matiec: task_configuration_c (SYM_REF2) with
    task_initialization_c (SYM_REF3) providing interval and priority. *)
type task = {
  tname:    ident;           (** Task name *)
  interval: timespec;        (** Task period — from task_initialization_c INTERVAL field *)
  priority: nat;             (** Task priority — from task_initialization_c PRIORITY field *)
}

(** Program instance bound to a task within a resource.
    Maps to matiec: program_configuration_c (SYM_REF5) *)
type program_inst = {
  iname:     ident;          (** Instance name *)
  task_name: ident;          (** Associated task name *)
  prog_type: ident;          (** Program type name *)
}

(** Resource within a configuration.
    Maps to matiec: resource_declaration_c (SYM_REF4) with
    single_resource_declaration_c (SYM_REF2) containing
    task_configuration_list_c and program_configuration_list_c *)
type resource = {
  rname:    ident;           (** Resource name *)
  tasks:    list task;       (** Task declarations *)
  programs: list program_inst; (** Program instances *)
}

(** Top-level configuration — root of the deployment model.
    Maps to matiec: configuration_declaration_c (SYM_REF5) with
    resource_declaration_list_c. global_var_declarations_list_c is empty
    for Category A programs. *)
type config = {
  cname:     ident;          (** Configuration name *)
  resources: list resource;  (** Resource declarations *)
}

(** {1 Top-Level AST} *)

(** Top-level declaration in an IEC 61131-3 source file.
    Maps to matiec: library_c (SYM_LIST) — the root AST node whose
    children are program declarations and configuration declarations. *)
type decl =
  | DeclProgram of program   (** PROGRAM declaration *)
  | DeclConfig  of config    (** CONFIGURATION declaration *)

(** A complete IEC 61131-3 compilation unit.
    Maps to matiec: library_c — the root of the entire AST *)
type compilation_unit = list decl
