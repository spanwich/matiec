(**
 * LowPLC.Costs — Per-opcode worst-case cycle cost constants.
 *
 * All values are placeholder zeros until populated from DWT measurements
 * on STM32F767ZI @ 216 MHz (Phase 3 of D5).  Each constant represents
 * the empirically measured worst-case cycle count for one opcode
 * execution, including the verified FB combinator call where applicable.
 *
 * Source: analysis/tlv_format.md §6
 *)
module LowPLC.Costs

(** Interpreter dispatch overhead — cost of reading [tag:1][len:2],
    dispatching to the correct handler, and advancing the offset.
    Applies to every atomic opcode. *)
let cost_dispatch : nat = 0

(** Control flow overhead — compound opcode framing costs. *)
let cost_if_overhead  : nat = 0   (* read cond + branch *)
let cost_for_overhead : nat = 0   (* loop setup / per-iteration compare+increment *)

(** §6.1 Literal Loads *)
let cost_load_true  : nat = 0
let cost_load_false : nat = 0
let cost_load_int   : nat = 0

(** §6.2 Boolean Operations *)
let cost_bool_not  : nat = 0
let cost_bool_and  : nat = 0
let cost_bool_or   : nat = 0
let cost_bool_xor  : nat = 0
let cost_copy_bool : nat = 0

(** §6.3 Integer Arithmetic *)
let cost_int_add  : nat = 0
let cost_int_sub  : nat = 0
let cost_int_mul  : nat = 0
let cost_copy_int : nat = 0

(** §6.4 Integer Comparisons *)
let cost_cmp_eq : nat = 0
let cost_cmp_ne : nat = 0
let cost_cmp_lt : nat = 0
let cost_cmp_gt : nat = 0
let cost_cmp_le : nat = 0
let cost_cmp_ge : nat = 0

(** §6.6 Timer FB Calls *)
let cost_ton_call : nat = 0
let cost_tof_call : nat = 0
let cost_tp_call  : nat = 0

(** §6.7 Counter FB Calls *)
let cost_ctu_call  : nat = 0
let cost_ctd_call  : nat = 0
let cost_ctud_call : nat = 0

(** §6.8 Bistable and Edge Detector FB Calls *)
let cost_sr_call    : nat = 0
let cost_rs_call    : nat = 0
let cost_rtrig_call : nat = 0
let cost_ftrig_call : nat = 0
