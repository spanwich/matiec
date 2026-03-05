(**
 * LowPLC.WCET — Compositional worst-case execution time calculator.
 *
 * Mirrors the interpreter structure (LowPLC.Interp.Spec) but computes
 * a cycle-count upper bound instead of executing opcodes.  All functions
 * are total and pure (Tot effect); termination is via `gas : nat`.
 *
 * Compositionality:
 *   - Atomic opcodes:  atomic_cost(tag) + cost_dispatch
 *   - IF_BLOCK:        cost_if_overhead + max(then_wcet, else_wcet)
 *   - FOR_BLOCK:       cost_for_overhead + iters * (body_wcet + cost_for_overhead)
 *   - Statement list:  sum of individual statement WCETs
 *
 * Soundness: The bound is valid iff every cost constant in LowPLC.Costs
 * is >= the actual worst-case cycle count measured on the target.  This
 * is an empirical property verified by DWT measurement (Phase 3).
 *
 * TLV format: Uses the flattened IF/FOR payload format (Phase 0 update):
 *   IF_BLOCK:  [cond:1][then_count:1][else_count:1][then_len:2][else_len:2]
 *              [then_body][else_body]
 *   FOR_BLOCK: [var_idx:1][from_val:2][to_val:2][body_count:1][body_len:2]
 *              [body]
 *
 * Source: analysis/tlv_format.md §6
 *)
module LowPLC.WCET

open FStar.Seq
open FStar.Mul
open LowPLC.Interp.State
open LowPLC.Interp.Spec
open LowPLC.Costs

module U8  = FStar.UInt8
module I16 = FStar.Int16

(** {1 Helpers} *)

let max (a b : nat) : nat = if a >= b then a else b

(** Look up worst-case cycle cost for an atomic (non-compound) opcode. *)
let atomic_cost (tag: nat) : nat =
  if tag = tag_load_true       then cost_load_true
  else if tag = tag_load_false then cost_load_false
  else if tag = tag_load_int   then cost_load_int
  else if tag = tag_bool_not   then cost_bool_not
  else if tag = tag_bool_and   then cost_bool_and
  else if tag = tag_bool_or    then cost_bool_or
  else if tag = tag_bool_xor   then cost_bool_xor
  else if tag = tag_copy_bool  then cost_copy_bool
  else if tag = tag_int_add    then cost_int_add
  else if tag = tag_int_sub    then cost_int_sub
  else if tag = tag_int_mul    then cost_int_mul
  else if tag = tag_copy_int   then cost_copy_int
  else if tag = tag_cmp_eq     then cost_cmp_eq
  else if tag = tag_cmp_ne     then cost_cmp_ne
  else if tag = tag_cmp_lt     then cost_cmp_lt
  else if tag = tag_cmp_gt     then cost_cmp_gt
  else if tag = tag_cmp_le     then cost_cmp_le
  else if tag = tag_cmp_ge     then cost_cmp_ge
  else if tag = tag_ton_call   then cost_ton_call
  else if tag = tag_tof_call   then cost_tof_call
  else if tag = tag_tp_call    then cost_tp_call
  else if tag = tag_ctu_call   then cost_ctu_call
  else if tag = tag_ctd_call   then cost_ctd_call
  else if tag = tag_ctud_call  then cost_ctud_call
  else if tag = tag_sr_call    then cost_sr_call
  else if tag = tag_rs_call    then cost_rs_call
  else if tag = tag_rtrig_call then cost_rtrig_call
  else if tag = tag_ftrig_call then cost_ftrig_call
  else 0


(** {1 Compositional WCET Calculator}

    Three mutually recursive functions using `gas : nat` as the
    decreasing termination measure (same pattern as the interpreter):

    - `wcet_stmts`     — scan statement list, sum costs
    - `wcet_if_block`  — IF_BLOCK compound opcode
    - `wcet_for_block` — FOR_BLOCK compound opcode *)

(** Compute WCET of a sequence of TLV opcodes.
    @param code  TLV binary
    @param off   byte offset of the first opcode
    @param rem   remaining bytes in this section
    @param gas   fuel for termination *)
let rec wcet_stmts (code: seq U8.t) (off: nat) (rem: nat) (gas: nat)
    : Tot nat (decreases gas) =
  if gas = 0 || rem < 3 then 0
  else if off + 3 > length code then 0
  else
    let tag = U8.v (index code off) in
    let len = read_u16_le code (off + 1) in
    if 3 + len > rem then 0
    else if off + 3 + len > length code then 0
    else
      let stmt_cost =
        if tag = tag_if_block then
          wcet_if_block code (off + 3) len (gas - 1)
        else if tag = tag_for_block then
          wcet_for_block code (off + 3) len (gas - 1)
        else
          atomic_cost tag + cost_dispatch
      in
      stmt_cost + wcet_stmts code (off + 3 + len) (rem - (3 + len)) (gas - 1)

(** Compute WCET of an IF_BLOCK.
    Payload format: [cond:1][then_count:1][else_count:1]
                    [then_len:2][else_len:2][then_body][else_body]
    Cost = cost_if_overhead + max(then_wcet, else_wcet) *)
and wcet_if_block (code: seq U8.t) (poff: nat) (plen: nat) (gas: nat)
    : Tot nat (decreases gas) =
  if gas = 0 then 0
  else if poff + 7 > length code then 0
  else
    let then_len = read_u16_le code (poff + 3) in
    let else_len = read_u16_le code (poff + 5) in
    let then_off = poff + 7 in
    let else_off = then_off + then_len in
    if then_off + then_len > length code then 0
    else if else_off + else_len > length code then 0
    else
      let then_cost = wcet_stmts code then_off then_len (gas - 1) in
      let else_cost = wcet_stmts code else_off else_len (gas - 1) in
      cost_if_overhead + max then_cost else_cost

(** Compute WCET of a FOR_BLOCK.
    Payload format: [var_idx:1][from_val:2][to_val:2]
                    [body_count:1][body_len:2][body]
    Cost = cost_for_overhead + iters * (body_wcet + cost_for_overhead)
    where iters = max(0, to_val - from_val + 1) *)
and wcet_for_block (code: seq U8.t) (poff: nat) (plen: nat) (gas: nat)
    : Tot nat (decreases gas) =
  if gas = 0 then 0
  else if poff + 8 > length code then 0
  else
    let start_v = read_i16_le code (poff + 1) in
    let end_v   = read_i16_le code (poff + 3) in
    let body_len = read_u16_le code (poff + 6) in
    let body_off = poff + 8 in
    if body_off + body_len > length code then 0
    else
      let sv = I16.v start_v in
      let ev = I16.v end_v in
      let iters = if ev >= sv then ev - sv + 1 else 0 in
      let body_cost = wcet_stmts code body_off body_len (gas - 1) in
      cost_for_overhead + iters * (body_cost + cost_for_overhead)


(** {1 Entry Point}

    Compute worst-case execution time for one scan cycle (BODY section).
    Parses the 20-byte TLV header to locate the body, then calls
    wcet_stmts on the body section.

    @param code  Full TLV program binary
    @param gas   Fuel for termination (e.g., 65536) *)
let compute_wcet (code: seq U8.t) (gas: nat) : nat =
  if length code < 20 then 0
  else
    let body_off = 20 + read_u16_le code 16 in
    let body_len = read_u16_le code 18 in
    if body_off + body_len > length code then 0
    else
      wcet_stmts code body_off body_len gas
