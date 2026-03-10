(**
 * LowDispatch -- Proof of concept: verified Low* opcode dispatch.
 * Tests whether SMT can discharge buffer safety for the dispatch pattern
 * WITHOUT --admit_smt_queries true.
 *)
module LowDispatch

open FStar.HyperStack.ST
open LowStar.Buffer
open LowStar.BufferOps
open FStar.HyperStack

module B   = LowStar.Buffer
module HS  = FStar.HyperStack
module ST  = FStar.HyperStack.ST
module U8  = FStar.UInt8
module U16 = FStar.UInt16
module U32 = FStar.UInt32
module I16 = FStar.Int16

(** Minimal pool for testing *)
noeq type mini_pool = {
  mp_bools: B.buffer bool;
  mp_ints:  B.buffer I16.t;
  mp_n_bools: U32.t;
  mp_n_ints:  U32.t;
}

let mini_pool_live (h: HS.mem) (p: mini_pool) : GTot Type0 =
  B.live h p.mp_bools /\ B.live h p.mp_ints

let mini_pool_lengths_ok (p: mini_pool) : GTot Type0 =
  B.length p.mp_bools >= U32.v p.mp_n_bools /\
  B.length p.mp_ints >= U32.v p.mp_n_ints

let mini_pool_disjoint (p: mini_pool) : GTot Type0 =
  B.loc_disjoint (B.loc_buffer p.mp_bools) (B.loc_buffer p.mp_ints)

let mini_pool_loc (p: mini_pool) : GTot B.loc =
  B.loc_union (B.loc_buffer p.mp_bools) (B.loc_buffer p.mp_ints)

(** Valid pool: live + lengths + disjoint *)
let mini_pool_valid (h: HS.mem) (p: mini_pool) : GTot Type0 =
  mini_pool_live h p /\ mini_pool_lengths_ok p /\ mini_pool_disjoint p

(** Test: dispatch 3 simple opcodes with genuine SMT proofs *)
val dispatch_test: code:B.buffer U8.t -> code_len:U32.t ->
  off:U32.t -> tag:U8.t -> len:U16.t -> p:mini_pool ->
  Stack unit
    (requires fun h ->
      B.live h code /\
      mini_pool_valid h p /\
      B.loc_disjoint (B.loc_buffer code) (mini_pool_loc p) /\
      B.length code = U32.v code_len /\
      U32.v off + 3 <= U32.v code_len /\
      U32.v off + 3 + U16.v len <= U32.v code_len)
    (ensures fun h0 () h1 ->
      B.modifies (mini_pool_loc p) h0 h1 /\
      mini_pool_live h1 p)

let dispatch_test code code_len off tag len p =
  let poff = U32.add off 3ul in

  (* LOAD_TRUE: payload [dst_idx:1] *)
  if tag = 0x01uy && U16.gte len 1us then
    let idx = code.(poff) in
    let idx32 = FStar.Int.Cast.uint8_to_uint32 idx in
    if U32.lt idx32 p.mp_n_bools then
      p.mp_bools.(idx32) <- true
    else ()

  (* LOAD_FALSE: payload [dst_idx:1] *)
  else if tag = 0x02uy && U16.gte len 1us then
    let idx = code.(poff) in
    let idx32 = FStar.Int.Cast.uint8_to_uint32 idx in
    if U32.lt idx32 p.mp_n_bools then
      p.mp_bools.(idx32) <- false
    else ()

  (* LOAD_INT: payload [dst_idx:1][value:2] *)
  else if tag = 0x03uy && U16.gte len 3us then
    let idx = code.(poff) in
    let idx32 = FStar.Int.Cast.uint8_to_uint32 idx in
    if U32.lt idx32 p.mp_n_ints then begin
      let r0 = code.(U32.add poff 1ul) in
      let r1 = code.(U32.add poff 2ul) in
      let lo = FStar.Int.Cast.uint8_to_uint16 r0 in
      let hi = FStar.Int.Cast.uint8_to_uint16 r1 in
      let raw = U16.add lo (U16.mul hi 256us) in
      let v = FStar.Int.Cast.uint16_to_int16 raw in
      p.mp_ints.(idx32) <- v
    end else ()

  else ()
