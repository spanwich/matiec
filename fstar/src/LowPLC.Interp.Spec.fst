(**
 * LowPLC.Interp.Spec — Pure denotational semantics for the TLV interpreter.
 *
 * Defines a recursive interpreter that walks a flat TLV byte sequence
 * and dispatches to the verified FB combinators from D2.  All functions
 * are total and pure; they map (code, var_pool) → var_pool.
 *
 * Termination: every function uses a `gas: nat` fuel parameter as the
 * decreasing measure.  Each recursive call passes `gas - 1`.
 *
 * Source: analysis/tlv_format.md
 *)
module LowPLC.Interp.Spec

open FStar.Seq
open FStar.Mul
open LowPLC.Interp.State

module TON  = LowPLC.TON
module TOF  = LowPLC.TOF
module TP   = LowPLC.TP
module CTU  = LowPLC.CTU
module CTD  = LowPLC.CTD
module CTUD = LowPLC.CTUD
module SR   = LowPLC.SR
module RS   = LowPLC.RS
module RT   = LowPLC.R_TRIG
module FT   = LowPLC.F_TRIG

module U8  = FStar.UInt8
module U32 = FStar.UInt32
module I16 = FStar.Int16

(** {1 TLV Opcode Tags (§6)} *)

(* §6.1 Literal Loads *)
let tag_load_true  : nat = 0x01
let tag_load_false : nat = 0x02
let tag_load_int   : nat = 0x03

(* §6.2 Boolean Operations *)
let tag_bool_not  : nat = 0x10
let tag_bool_and  : nat = 0x11
let tag_bool_or   : nat = 0x12
let tag_bool_xor  : nat = 0x13
let tag_copy_bool : nat = 0x14

(* §6.3 Integer Arithmetic *)
let tag_int_add  : nat = 0x20
let tag_int_sub  : nat = 0x21
let tag_int_mul  : nat = 0x22
let tag_copy_int : nat = 0x23

(* §6.4 Integer Comparisons *)
let tag_cmp_eq : nat = 0x30
let tag_cmp_ne : nat = 0x31
let tag_cmp_lt : nat = 0x32
let tag_cmp_gt : nat = 0x33
let tag_cmp_le : nat = 0x34
let tag_cmp_ge : nat = 0x35

(* §6.5 Control Flow *)
let tag_if_block  : nat = 0x40
let tag_for_block : nat = 0x41

(* §6.6 Timer FB Calls *)
let tag_ton_call : nat = 0x50
let tag_tof_call : nat = 0x51
let tag_tp_call  : nat = 0x52

(* §6.7 Counter FB Calls *)
let tag_ctu_call  : nat = 0x60
let tag_ctd_call  : nat = 0x61
let tag_ctud_call : nat = 0x62

(* §6.8 Bistable and Edge Detector FB Calls *)
let tag_sr_call    : nat = 0x70
let tag_rs_call    : nat = 0x71
let tag_rtrig_call : nat = 0x72
let tag_ftrig_call : nat = 0x73

(** {1 Atomic Opcode Execution}

    Handles all non-compound (non-recursive) opcodes.
    @param code  TLV binary
    @param off   start of the TLV triple (tag byte)
    @param tag   U8.v of the tag byte (already read by caller)
    @param len   payload length (already read by caller)
    @param p     current variable pool *)
let exec_atomic (code: seq U8.t) (off: nat) (tag: nat) (len: nat)
    (p: var_pool) : var_pool =
  let poff = off + 3 in
  if poff + len > length code then p

  (* §6.1 Literal Loads *)
  else if tag = tag_load_true && len >= 1 then
    set_bool p (U8.v (index code poff)) true

  else if tag = tag_load_false && len >= 1 then
    set_bool p (U8.v (index code poff)) false

  else if tag = tag_load_int && len >= 3 then
    set_int p (U8.v (index code poff)) (read_i16_le code (poff + 1))

  (* §6.2 Boolean Operations *)
  else if tag = tag_bool_not && len >= 2 then
    let dst = U8.v (index code poff) in
    let src = U8.v (index code (poff + 1)) in
    set_bool p dst (not (get_bool p src))

  else if tag = tag_bool_and && len >= 3 then
    let dst = U8.v (index code poff) in
    let a   = U8.v (index code (poff + 1)) in
    let b   = U8.v (index code (poff + 2)) in
    set_bool p dst (get_bool p a && get_bool p b)

  else if tag = tag_bool_or && len >= 3 then
    let dst = U8.v (index code poff) in
    let a   = U8.v (index code (poff + 1)) in
    let b   = U8.v (index code (poff + 2)) in
    set_bool p dst (get_bool p a || get_bool p b)

  else if tag = tag_bool_xor && len >= 3 then
    let dst = U8.v (index code poff) in
    let a   = U8.v (index code (poff + 1)) in
    let b   = U8.v (index code (poff + 2)) in
    set_bool p dst (get_bool p a <> get_bool p b)

  else if tag = tag_copy_bool && len >= 2 then
    let dst = U8.v (index code poff) in
    let src = U8.v (index code (poff + 1)) in
    set_bool p dst (get_bool p src)

  (* §6.3 Integer Arithmetic — wrapping per DDR-003 *)
  else if tag = tag_int_add && len >= 3 then
    let dst = U8.v (index code poff) in
    let a   = U8.v (index code (poff + 1)) in
    let b   = U8.v (index code (poff + 2)) in
    set_int p dst (add_wrap (get_int p a) (get_int p b))

  else if tag = tag_int_sub && len >= 3 then
    let dst = U8.v (index code poff) in
    let a   = U8.v (index code (poff + 1)) in
    let b   = U8.v (index code (poff + 2)) in
    set_int p dst (sub_wrap (get_int p a) (get_int p b))

  else if tag = tag_int_mul && len >= 3 then
    let dst = U8.v (index code poff) in
    let a   = U8.v (index code (poff + 1)) in
    let b   = U8.v (index code (poff + 2)) in
    set_int p dst (mul_wrap (get_int p a) (get_int p b))

  else if tag = tag_copy_int && len >= 2 then
    let dst = U8.v (index code poff) in
    let src = U8.v (index code (poff + 1)) in
    set_int p dst (get_int p src)

  (* §6.4 Integer Comparisons *)
  else if tag = tag_cmp_eq && len >= 3 then
    let dst = U8.v (index code poff) in
    let a   = U8.v (index code (poff + 1)) in
    let b   = U8.v (index code (poff + 2)) in
    set_bool p dst (I16.v (get_int p a) = I16.v (get_int p b))

  else if tag = tag_cmp_ne && len >= 3 then
    let dst = U8.v (index code poff) in
    let a   = U8.v (index code (poff + 1)) in
    let b   = U8.v (index code (poff + 2)) in
    set_bool p dst (I16.v (get_int p a) <> I16.v (get_int p b))

  else if tag = tag_cmp_lt && len >= 3 then
    let dst = U8.v (index code poff) in
    let a   = U8.v (index code (poff + 1)) in
    let b   = U8.v (index code (poff + 2)) in
    set_bool p dst (I16.v (get_int p a) < I16.v (get_int p b))

  else if tag = tag_cmp_gt && len >= 3 then
    let dst = U8.v (index code poff) in
    let a   = U8.v (index code (poff + 1)) in
    let b   = U8.v (index code (poff + 2)) in
    set_bool p dst (I16.v (get_int p a) > I16.v (get_int p b))

  else if tag = tag_cmp_le && len >= 3 then
    let dst = U8.v (index code poff) in
    let a   = U8.v (index code (poff + 1)) in
    let b   = U8.v (index code (poff + 2)) in
    set_bool p dst (I16.v (get_int p a) <= I16.v (get_int p b))

  else if tag = tag_cmp_ge && len >= 3 then
    let dst = U8.v (index code poff) in
    let a   = U8.v (index code (poff + 1)) in
    let b   = U8.v (index code (poff + 2)) in
    set_bool p dst (I16.v (get_int p a) >= I16.v (get_int p b))

  (* §6.6 Timer FB Calls — precondition checked at runtime *)
  else if tag = tag_ton_call && len >= 7 then
    let idx  = U8.v (index code poff) in
    let in_b = U8.v (index code (poff + 1)) in
    let pt   = read_u32_le code (poff + 2) in
    let q_b  = U8.v (index code (poff + 6)) in
    let s = get_ton p idx in
    if U32.v p.tick >= U32.v s.start_tick then
      let (new_s, out) = TON.ton_step s (get_bool p in_b) pt p.tick in
      set_bool (set_ton p idx new_s) q_b out.q
    else p

  else if tag = tag_tof_call && len >= 7 then
    let idx  = U8.v (index code poff) in
    let in_b = U8.v (index code (poff + 1)) in
    let pt   = read_u32_le code (poff + 2) in
    let q_b  = U8.v (index code (poff + 6)) in
    let s = get_tof p idx in
    if U32.v p.tick >= U32.v s.start_tick then
      let (new_s, out) = TOF.tof_step s (get_bool p in_b) pt p.tick in
      set_bool (set_tof p idx new_s) q_b out.q
    else p

  else if tag = tag_tp_call && len >= 7 then
    let idx  = U8.v (index code poff) in
    let in_b = U8.v (index code (poff + 1)) in
    let pt   = read_u32_le code (poff + 2) in
    let q_b  = U8.v (index code (poff + 6)) in
    let s = get_tp p idx in
    if U32.v p.tick >= U32.v s.start_tick then
      let (new_s, out) = TP.tp_step s (get_bool p in_b) pt p.tick in
      set_bool (set_tp p idx new_s) q_b out.q
    else p

  (* §6.7 Counter FB Calls *)
  else if tag = tag_ctu_call && len >= 6 then
    let idx  = U8.v (index code poff) in
    let cu_b = U8.v (index code (poff + 1)) in
    let r_b  = U8.v (index code (poff + 2)) in
    let pv_i = U8.v (index code (poff + 3)) in
    let q_b  = U8.v (index code (poff + 4)) in
    let cv_i = U8.v (index code (poff + 5)) in
    let s = get_ctu p idx in
    let (new_s, out) = CTU.ctu_step s (get_bool p cu_b) (get_bool p r_b)
                                       (get_int p pv_i) in
    set_int (set_bool (set_ctu p idx new_s) q_b out.q) cv_i out.cv_out

  else if tag = tag_ctd_call && len >= 6 then
    let idx  = U8.v (index code poff) in
    let cd_b = U8.v (index code (poff + 1)) in
    let ld_b = U8.v (index code (poff + 2)) in
    let pv_i = U8.v (index code (poff + 3)) in
    let q_b  = U8.v (index code (poff + 4)) in
    let cv_i = U8.v (index code (poff + 5)) in
    let s = get_ctd p idx in
    let (new_s, out) = CTD.ctd_step s (get_bool p cd_b) (get_bool p ld_b)
                                       (get_int p pv_i) in
    set_int (set_bool (set_ctd p idx new_s) q_b out.q) cv_i out.cv_out

  else if tag = tag_ctud_call && len >= 9 then
    let idx  = U8.v (index code poff) in
    let cu_b = U8.v (index code (poff + 1)) in
    let cd_b = U8.v (index code (poff + 2)) in
    let r_b  = U8.v (index code (poff + 3)) in
    let ld_b = U8.v (index code (poff + 4)) in
    let pv_i = U8.v (index code (poff + 5)) in
    let qu_b = U8.v (index code (poff + 6)) in
    let qd_b = U8.v (index code (poff + 7)) in
    let cv_i = U8.v (index code (poff + 8)) in
    let s = get_ctud p idx in
    let (new_s, out) = CTUD.ctud_step s (get_bool p cu_b) (get_bool p cd_b)
                                         (get_bool p r_b)  (get_bool p ld_b)
                                         (get_int p pv_i) in
    let p1 = set_ctud p idx new_s in
    let p2 = set_bool p1 qu_b out.qu in
    let p3 = set_bool p2 qd_b out.qd in
    set_int p3 cv_i out.cv_out

  (* §6.8 Bistable and Edge Detector FB Calls *)
  else if tag = tag_sr_call && len >= 4 then
    let idx  = U8.v (index code poff) in
    let s1_b = U8.v (index code (poff + 1)) in
    let r_b  = U8.v (index code (poff + 2)) in
    let q1_b = U8.v (index code (poff + 3)) in
    let s = get_sr p idx in
    let (new_s, out) = SR.sr_step s (get_bool p s1_b) (get_bool p r_b) in
    set_bool (set_sr p idx new_s) q1_b out.q1

  else if tag = tag_rs_call && len >= 4 then
    let idx  = U8.v (index code poff) in
    let s_b  = U8.v (index code (poff + 1)) in
    let r1_b = U8.v (index code (poff + 2)) in
    let q1_b = U8.v (index code (poff + 3)) in
    let s = get_rs p idx in
    let (new_s, out) = RS.rs_step s (get_bool p s_b) (get_bool p r1_b) in
    set_bool (set_rs p idx new_s) q1_b out.q1

  else if tag = tag_rtrig_call && len >= 3 then
    let idx   = U8.v (index code poff) in
    let clk_b = U8.v (index code (poff + 1)) in
    let q_b   = U8.v (index code (poff + 2)) in
    let s = get_rtrig p idx in
    let (new_s, out) = RT.r_trig_step s (get_bool p clk_b) in
    set_bool (set_rtrig p idx new_s) q_b out.q

  else if tag = tag_ftrig_call && len >= 3 then
    let idx   = U8.v (index code poff) in
    let clk_b = U8.v (index code (poff + 1)) in
    let q_b   = U8.v (index code (poff + 2)) in
    let s = get_ftrig p idx in
    let (new_s, out) = FT.f_trig_step s (get_bool p clk_b) in
    set_bool (set_ftrig p idx new_s) q_b out.q

  else p  (* Unknown opcode — skip *)


(** {1 Recursive Interpreter Core}

    Five mutually recursive functions, all using `gas: nat` as the
    decreasing termination measure:

    - `exec_stmts`        — sequential opcode scanner
    - `exec_if_block`     — IF_BLOCK compound opcode
    - `exec_elsif_chain`  — ELSIF clause scanner
    - `exec_for_block`    — FOR_BLOCK compound opcode
    - `exec_for_iter`     — FOR loop iteration *)

(** Scan and execute a sequence of TLV opcodes.
    @param code  TLV binary
    @param off   byte offset of the first opcode
    @param rem   remaining bytes in this section
    @param p     current variable pool
    @param gas   fuel for termination *)
let rec exec_stmts (code: seq U8.t) (off: nat) (rem: nat) (p: var_pool)
    (gas: nat) : Tot var_pool (decreases gas) =
  if gas = 0 || rem < 3 then p
  else if off + 3 > length code then p
  else
    let tag = U8.v (index code off) in
    let len = read_u16_le code (off + 1) in
    if 3 + len > rem then p
    else if off + 3 + len > length code then p
    else
      let p' =
        if tag = tag_if_block then
          exec_if_block code (off + 3) len p (gas - 1)
        else if tag = tag_for_block then
          exec_for_block code (off + 3) len p (gas - 1)
        else
          exec_atomic code off tag len p
      in
      exec_stmts code (off + 3 + len) (rem - (3 + len)) p' (gas - 1)

(** Execute an IF_BLOCK compound opcode.
    Payload: [cond_b:1][then_len:2][then_body][num_elsif:1]
             {[elsif_cond:1][elsif_len:2][elsif_body]}*
             [else_len:2][else_body] *)
and exec_if_block (code: seq U8.t) (poff: nat) (plen: nat) (p: var_pool)
    (gas: nat) : Tot var_pool (decreases gas) =
  if gas = 0 then p
  else if poff + 3 > length code then p
  else
    let cond_b  = U8.v (index code poff) in
    let then_len = read_u16_le code (poff + 1) in
    let then_off = poff + 3 in
    if then_off + then_len > length code then p
    else if get_bool p cond_b then
      (* IF condition true — execute THEN body *)
      exec_stmts code then_off then_len p (gas - 1)
    else
      (* IF condition false — check ELSIFs *)
      let ec_off = then_off + then_len in
      if ec_off >= length code then p
      else
        let num_elsif = U8.v (index code ec_off) in
        let (p', rest) =
          exec_elsif_chain code (ec_off + 1) num_elsif false p (gas - 1)
        in
        let (matched, after) = rest in
        if matched then p'
        else
          (* No ELSIF matched — try ELSE *)
          if after + 2 > length code then p'
          else
            let else_len = read_u16_le code after in
            if else_len = 0 then p'
            else if after + 2 + else_len > length code then p'
            else exec_stmts code (after + 2) else_len p' (gas - 1)

(** Scan ELSIF clauses, executing the first one whose condition is true.
    Returns (updated_pool, matched, offset_after_all_scanned_elsifs). *)
and exec_elsif_chain (code: seq U8.t) (off: nat) (n: nat) (matched: bool)
    (p: var_pool) (gas: nat) : Tot (var_pool & (bool & nat)) (decreases gas) =
  if gas = 0 || n = 0 then (p, (matched, off))
  else if off + 3 > length code then (p, (matched, off))
  else
    let cond_b   = U8.v (index code off) in
    let body_len = read_u16_le code (off + 1) in
    let body_off = off + 3 in
    if body_off + body_len > length code then (p, (matched, off))
    else
      let next_off = body_off + body_len in
      if (not matched) && get_bool p cond_b then
        let p' = exec_stmts code body_off body_len p (gas - 1) in
        exec_elsif_chain code next_off (n - 1) true p' (gas - 1)
      else
        exec_elsif_chain code next_off (n - 1) matched p (gas - 1)

(** Execute a FOR_BLOCK compound opcode.
    Payload: [idx_i:1][start:2][end:2][body_len:2][body] *)
and exec_for_block (code: seq U8.t) (poff: nat) (plen: nat) (p: var_pool)
    (gas: nat) : Tot var_pool (decreases gas) =
  if gas = 0 then p
  else if poff + 7 > length code then p
  else
    let idx_i    = U8.v (index code poff) in
    let start_v  = read_i16_le code (poff + 1) in
    let end_v    = read_i16_le code (poff + 3) in
    let body_len = read_u16_le code (poff + 5) in
    let body_off = poff + 7 in
    if body_off + body_len > length code then p
    else
      let sv = I16.v start_v in
      let ev = I16.v end_v in
      let iters = if ev >= sv then ev - sv + 1 else 0 in
      let p' = set_int p idx_i start_v in
      exec_for_iter code body_off body_len idx_i end_v p' iters (gas - 1)

(** Execute FOR loop iterations.
    @param iters_left  remaining iterations (bounds the loop) *)
and exec_for_iter (code: seq U8.t) (body_off: nat) (body_len: nat)
    (idx_i: nat) (end_v: I16.t) (p: var_pool) (iters_left: nat) (gas: nat)
    : Tot var_pool (decreases gas) =
  if gas = 0 || iters_left = 0 then p
  else
    let p' = exec_stmts code body_off body_len p (gas - 1) in
    let cur = get_int p' idx_i in
    let next = add_wrap cur 1s in
    let p'' = set_int p' idx_i next in
    exec_for_iter code body_off body_len idx_i end_v p'' (iters_left - 1)
                  (gas - 1)


(** {1 Header Parsing} *)

type header = {
  h_n_bools:  nat;
  h_n_ints:   nat;
  h_n_ton:    nat;
  h_n_tof:    nat;
  h_n_tp:     nat;
  h_n_ctu:    nat;
  h_n_ctd:    nat;
  h_n_ctud:   nat;
  h_n_sr:     nat;
  h_n_rs:     nat;
  h_n_rtrig:  nat;
  h_n_ftrig:  nat;
  h_init_len: nat;
  h_body_len: nat;
}

(** Parse a 20-byte TLV header.  Returns None on invalid magic/version. *)
let parse_header (code: seq U8.t) : option header =
  if length code < 20 then None
  else
    let m0 = U8.v (index code 0) in
    let m1 = U8.v (index code 1) in
    if m0 <> 0x4C || m1 <> 0x50 then None   (* magic "LP" *)
    else
      let ver = U8.v (index code 2) in
      if ver <> 1 then None
      else Some {
        h_n_bools  = U8.v (index code 4);
        h_n_ints   = U8.v (index code 5);
        h_n_ton    = U8.v (index code 6);
        h_n_tof    = U8.v (index code 7);
        h_n_tp     = U8.v (index code 8);
        h_n_ctu    = U8.v (index code 9);
        h_n_ctd    = U8.v (index code 10);
        h_n_ctud   = U8.v (index code 11);
        h_n_sr     = U8.v (index code 12);
        h_n_rs     = U8.v (index code 13);
        h_n_rtrig  = U8.v (index code 14);
        h_n_ftrig  = U8.v (index code 15);
        h_init_len = read_u16_le code 16;
        h_body_len = read_u16_le code 18;
      }


(** {1 Pool Initialization} *)

(** Create a zero-initialized variable pool from header dimensions. *)
let mk_pool (h: header) (tick: U32.t) : var_pool = {
  bools  = create h.h_n_bools false;
  ints   = create h.h_n_ints 0s;
  tons   = create h.h_n_ton  TON.ton_init;
  tofs   = create h.h_n_tof  TOF.tof_init;
  tps    = create h.h_n_tp   TP.tp_init;
  ctus   = create h.h_n_ctu  CTU.ctu_init;
  ctds   = create h.h_n_ctd  CTD.ctd_init;
  ctuds  = create h.h_n_ctud CTUD.ctud_init;
  srs    = create h.h_n_sr   SR.sr_init;
  rss    = create h.h_n_rs   RS.rs_init;
  rtrigs = create h.h_n_rtrig RT.r_trig_init;
  ftrigs = create h.h_n_ftrig FT.f_trig_init;
  tick   = tick;
}


(** {1 Entry Points} *)

(** Execute the INIT section (called once at program load).
    Creates a fresh pool, runs init opcodes, returns initialized pool. *)
let exec_init (code: seq U8.t) (h: header) (gas: nat) : var_pool =
  let p0 = mk_pool h 0ul in
  exec_stmts code 20 h.h_init_len p0 gas

(** Execute the BODY section (called each scan cycle).
    The caller should update p.tick to the current scan cycle counter
    before calling this function. *)
let exec_scan (code: seq U8.t) (h: header) (p: var_pool) (gas: nat)
    : var_pool =
  exec_stmts code (20 + h.h_init_len) h.h_body_len p gas
