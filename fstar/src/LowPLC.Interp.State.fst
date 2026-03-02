(**
 * LowPLC.Interp.State — Variable pool types and accessors for the
 * verified interpreter.
 *
 * Defines the pure (Seq-based) var_pool used by the denotational
 * semantics in LowPLC.Interp.Spec.  Each field is an immutable F*
 * sequence; the spec function returns a new pool after each opcode.
 *
 * Source: analysis/tlv_format.md §4
 *)
module LowPLC.Interp.State

open FStar.Seq
open FStar.Mul

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

(** {1 Variable Pool} *)

(** Pure variable pool — the state of a LowPLC program.
    Each field is an immutable F* sequence.  FB state types are
    imported from the verified combinator modules (D2). *)
noeq type var_pool = {
  bools:  seq bool;
  ints:   seq I16.t;
  tons:   seq TON.ton_state;
  tofs:   seq TOF.tof_state;
  tps:    seq TP.tp_state;
  ctus:   seq CTU.ctu_state;
  ctds:   seq CTD.ctd_state;
  ctuds:  seq CTUD.ctud_state;
  srs:    seq SR.sr_state;
  rss:    seq RS.rs_state;
  rtrigs: seq RT.r_trig_state;
  ftrigs: seq FT.f_trig_state;
  tick:   U32.t;
}

(** {1 Safe Scalar Accessors} *)

let get_bool (p: var_pool) (i: nat) : bool =
  if i < length p.bools then index p.bools i else false

let set_bool (p: var_pool) (i: nat) (v: bool) : var_pool =
  if i < length p.bools then { p with bools = upd p.bools i v } else p

let get_int (p: var_pool) (i: nat) : I16.t =
  if i < length p.ints then index p.ints i else 0s

let set_int (p: var_pool) (i: nat) (v: I16.t) : var_pool =
  if i < length p.ints then { p with ints = upd p.ints i v } else p

(** {1 Safe FB State Accessors} *)

let get_ton (p: var_pool) (i: nat) : TON.ton_state =
  if i < length p.tons then index p.tons i else TON.ton_init

let set_ton (p: var_pool) (i: nat) (s: TON.ton_state) : var_pool =
  if i < length p.tons then { p with tons = upd p.tons i s } else p

let get_tof (p: var_pool) (i: nat) : TOF.tof_state =
  if i < length p.tofs then index p.tofs i else TOF.tof_init

let set_tof (p: var_pool) (i: nat) (s: TOF.tof_state) : var_pool =
  if i < length p.tofs then { p with tofs = upd p.tofs i s } else p

let get_tp (p: var_pool) (i: nat) : TP.tp_state =
  if i < length p.tps then index p.tps i else TP.tp_init

let set_tp (p: var_pool) (i: nat) (s: TP.tp_state) : var_pool =
  if i < length p.tps then { p with tps = upd p.tps i s } else p

let get_ctu (p: var_pool) (i: nat) : CTU.ctu_state =
  if i < length p.ctus then index p.ctus i else CTU.ctu_init

let set_ctu (p: var_pool) (i: nat) (s: CTU.ctu_state) : var_pool =
  if i < length p.ctus then { p with ctus = upd p.ctus i s } else p

let get_ctd (p: var_pool) (i: nat) : CTD.ctd_state =
  if i < length p.ctds then index p.ctds i else CTD.ctd_init

let set_ctd (p: var_pool) (i: nat) (s: CTD.ctd_state) : var_pool =
  if i < length p.ctds then { p with ctds = upd p.ctds i s } else p

let get_ctud (p: var_pool) (i: nat) : CTUD.ctud_state =
  if i < length p.ctuds then index p.ctuds i else CTUD.ctud_init

let set_ctud (p: var_pool) (i: nat) (s: CTUD.ctud_state) : var_pool =
  if i < length p.ctuds then { p with ctuds = upd p.ctuds i s } else p

let get_sr (p: var_pool) (i: nat) : SR.sr_state =
  if i < length p.srs then index p.srs i else SR.sr_init

let set_sr (p: var_pool) (i: nat) (s: SR.sr_state) : var_pool =
  if i < length p.srs then { p with srs = upd p.srs i s } else p

let get_rs (p: var_pool) (i: nat) : RS.rs_state =
  if i < length p.rss then index p.rss i else RS.rs_init

let set_rs (p: var_pool) (i: nat) (s: RS.rs_state) : var_pool =
  if i < length p.rss then { p with rss = upd p.rss i s } else p

let get_rtrig (p: var_pool) (i: nat) : RT.r_trig_state =
  if i < length p.rtrigs then index p.rtrigs i else RT.r_trig_init

let set_rtrig (p: var_pool) (i: nat) (s: RT.r_trig_state) : var_pool =
  if i < length p.rtrigs then { p with rtrigs = upd p.rtrigs i s } else p

let get_ftrig (p: var_pool) (i: nat) : FT.f_trig_state =
  if i < length p.ftrigs then index p.ftrigs i else FT.f_trig_init

let set_ftrig (p: var_pool) (i: nat) (s: FT.f_trig_state) : var_pool =
  if i < length p.ftrigs then { p with ftrigs = upd p.ftrigs i s } else p

(** {1 Byte Parsing Helpers} *)

let read_u16_le (s: seq U8.t) (off: nat) : nat =
  if off + 2 <= length s then
    U8.v (index s off) + 256 * U8.v (index s (off + 1))
  else 0

let read_u32_le (s: seq U8.t) (off: nat) : U32.t =
  if off + 4 <= length s then
    let b0 = U8.v (index s off) in
    let b1 = U8.v (index s (off + 1)) in
    let b2 = U8.v (index s (off + 2)) in
    let b3 = U8.v (index s (off + 3)) in
    U32.uint_to_t (b0 + 256 * b1 + 65536 * b2 + 16777216 * b3)
  else 0ul

let read_i16_le (s: seq U8.t) (off: nat) : I16.t =
  if off + 2 <= length s then
    let lo = U8.v (index s off) in
    let hi = U8.v (index s (off + 1)) in
    let raw = lo + 256 * hi in
    I16.int_to_t (if raw >= 32768 then raw - 65536 else raw)
  else 0s

(** {1 Wrapping INT Arithmetic (DDR-003)} *)

let wrap_i16 (x: int) : I16.t =
  I16.int_to_t (((x + 32768) % 65536) - 32768)

let add_wrap (a b: I16.t) : I16.t = wrap_i16 (I16.v a + I16.v b)
let sub_wrap (a b: I16.t) : I16.t = wrap_i16 (I16.v a - I16.v b)
let mul_wrap (a b: I16.t) : I16.t = wrap_i16 (I16.v a * I16.v b)
