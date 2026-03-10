(**
 * LowPLC.Interp.Low - Low* (stateful) TLV interpreter for KaRaMeL extraction.
 *
 * Implements the D5-format interpreter using mutable buffers (B.buffer)
 * so KaRaMeL can extract it to C.  Each function has Stack effect and
 * operates on a flat pool struct with buffer-backed arrays.
 *
 * Design decisions:
 *   - Pool passed as a record of buffer pointers (extracts to C struct)
 *   - FB state stored as flat structs in buffers (phase as U8, bools as U8)
 *   - Buffer safety proved via pool_disjoint + code-pool disjointness
 *   - Gas parameter is U32.t (bounded, C-extractable)
 *
 * Proof status: Buffer safety fully verified (no admits in dispatch).
 *   Refinement proofs connecting to LowPLC.Interp.D5 are in D5.Impl.
 *
 * Source: LowPLC.Interp.D5.fst (pure reference spec)
 *)
module LowPLC.Interp.Low

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

(** {1 Pool Type} *)

(** Flat FB state types - C-compatible, stored in buffers.
    Each uses machine integers only (no algebraic types in buffer storage). *)

noeq type ton_flat = { tf_phase: U8.t; tf_start: U32.t }
noeq type tof_flat = { tff_phase: U8.t; tff_prev_in: U8.t; tff_start: U32.t }
noeq type tp_flat  = { tpf_phase: U8.t; tpf_prev_in: U8.t; tpf_start: U32.t }
noeq type ctu_flat = { ctuf_cv: I16.t; ctuf_prev_cu: U8.t }
noeq type ctd_flat = { ctdf_cv: I16.t; ctdf_prev_cd: U8.t }
noeq type ctud_flat = { ctudf_cv: I16.t; ctudf_prev_cu: U8.t; ctudf_prev_cd: U8.t }
noeq type sr_flat   = { srf_q: U8.t }
noeq type rs_flat   = { rsf_q: U8.t }
noeq type rtrig_flat = { rtf_m: U8.t }
noeq type ftrig_flat = { ftf_m: U8.t }

(** The pool struct - all state held in mutable buffers.
    KaRaMeL extracts this to a C struct with pointer members. *)
noeq type low_pool = {
  lp_bools:  B.buffer bool;     (* length >= lp_n_bools *)
  lp_ints:   B.buffer I16.t;    (* length >= lp_n_ints *)
  lp_tons:   B.buffer ton_flat;
  lp_tofs:   B.buffer tof_flat;
  lp_tps:    B.buffer tp_flat;
  lp_ctus:   B.buffer ctu_flat;
  lp_ctds:   B.buffer ctd_flat;
  lp_ctuds:  B.buffer ctud_flat;
  lp_srs:    B.buffer sr_flat;
  lp_rss:    B.buffer rs_flat;
  lp_rtrigs: B.buffer rtrig_flat;
  lp_ftrigs: B.buffer ftrig_flat;
  lp_n_bools: U32.t;
  lp_n_ints:  U32.t;
  lp_n_ton:   U32.t;
  lp_n_tof:   U32.t;
  lp_n_tp:    U32.t;
  lp_n_ctu:   U32.t;
  lp_n_ctd:   U32.t;
  lp_n_ctud:  U32.t;
  lp_n_sr:    U32.t;
  lp_n_rs:    U32.t;
  lp_n_rtrig: U32.t;
  lp_n_ftrig: U32.t;
  lp_tick:    U32.t;
  lp_scan_period_us: U32.t;
}


(** {1 Pool Validity Predicates} *)

(** All pool buffers are live in the given memory. *)
let pool_live (h: HS.mem) (p: low_pool) : GTot Type0 =
  B.live h p.lp_bools /\ B.live h p.lp_ints /\
  B.live h p.lp_tons /\ B.live h p.lp_tofs /\ B.live h p.lp_tps /\
  B.live h p.lp_ctus /\ B.live h p.lp_ctds /\ B.live h p.lp_ctuds /\
  B.live h p.lp_srs /\ B.live h p.lp_rss /\
  B.live h p.lp_rtrigs /\ B.live h p.lp_ftrigs

(** Buffer lengths match declared pool sizes. *)
let pool_lengths_ok (p: low_pool) : GTot Type0 =
  B.length p.lp_bools >= U32.v p.lp_n_bools /\
  B.length p.lp_ints >= U32.v p.lp_n_ints /\
  B.length p.lp_tons >= U32.v p.lp_n_ton /\
  B.length p.lp_tofs >= U32.v p.lp_n_tof /\
  B.length p.lp_tps >= U32.v p.lp_n_tp /\
  B.length p.lp_ctus >= U32.v p.lp_n_ctu /\
  B.length p.lp_ctds >= U32.v p.lp_n_ctd /\
  B.length p.lp_ctuds >= U32.v p.lp_n_ctud /\
  B.length p.lp_srs >= U32.v p.lp_n_sr /\
  B.length p.lp_rss >= U32.v p.lp_n_rs /\
  B.length p.lp_rtrigs >= U32.v p.lp_n_rtrig /\
  B.length p.lp_ftrigs >= U32.v p.lp_n_ftrig

(** All pool buffers are pairwise disjoint. *)
let pool_disjoint (p: low_pool) : GTot Type0 =
  B.loc_disjoint (B.loc_buffer p.lp_bools) (B.loc_buffer p.lp_ints) /\
  B.loc_disjoint (B.loc_buffer p.lp_bools) (B.loc_buffer p.lp_tons) /\
  B.loc_disjoint (B.loc_buffer p.lp_bools) (B.loc_buffer p.lp_tofs) /\
  B.loc_disjoint (B.loc_buffer p.lp_bools) (B.loc_buffer p.lp_tps) /\
  B.loc_disjoint (B.loc_buffer p.lp_bools) (B.loc_buffer p.lp_ctus) /\
  B.loc_disjoint (B.loc_buffer p.lp_bools) (B.loc_buffer p.lp_ctds) /\
  B.loc_disjoint (B.loc_buffer p.lp_bools) (B.loc_buffer p.lp_ctuds) /\
  B.loc_disjoint (B.loc_buffer p.lp_bools) (B.loc_buffer p.lp_srs) /\
  B.loc_disjoint (B.loc_buffer p.lp_bools) (B.loc_buffer p.lp_rss) /\
  B.loc_disjoint (B.loc_buffer p.lp_bools) (B.loc_buffer p.lp_rtrigs) /\
  B.loc_disjoint (B.loc_buffer p.lp_bools) (B.loc_buffer p.lp_ftrigs) /\
  B.loc_disjoint (B.loc_buffer p.lp_ints) (B.loc_buffer p.lp_tons) /\
  B.loc_disjoint (B.loc_buffer p.lp_ints) (B.loc_buffer p.lp_tofs) /\
  B.loc_disjoint (B.loc_buffer p.lp_ints) (B.loc_buffer p.lp_tps) /\
  B.loc_disjoint (B.loc_buffer p.lp_ints) (B.loc_buffer p.lp_ctus) /\
  B.loc_disjoint (B.loc_buffer p.lp_ints) (B.loc_buffer p.lp_ctds) /\
  B.loc_disjoint (B.loc_buffer p.lp_ints) (B.loc_buffer p.lp_ctuds) /\
  B.loc_disjoint (B.loc_buffer p.lp_ints) (B.loc_buffer p.lp_srs) /\
  B.loc_disjoint (B.loc_buffer p.lp_ints) (B.loc_buffer p.lp_rss) /\
  B.loc_disjoint (B.loc_buffer p.lp_ints) (B.loc_buffer p.lp_rtrigs) /\
  B.loc_disjoint (B.loc_buffer p.lp_ints) (B.loc_buffer p.lp_ftrigs) /\
  B.loc_disjoint (B.loc_buffer p.lp_tons) (B.loc_buffer p.lp_tofs) /\
  B.loc_disjoint (B.loc_buffer p.lp_tons) (B.loc_buffer p.lp_tps) /\
  B.loc_disjoint (B.loc_buffer p.lp_tons) (B.loc_buffer p.lp_ctus) /\
  B.loc_disjoint (B.loc_buffer p.lp_tons) (B.loc_buffer p.lp_ctds) /\
  B.loc_disjoint (B.loc_buffer p.lp_tons) (B.loc_buffer p.lp_ctuds) /\
  B.loc_disjoint (B.loc_buffer p.lp_tons) (B.loc_buffer p.lp_srs) /\
  B.loc_disjoint (B.loc_buffer p.lp_tons) (B.loc_buffer p.lp_rss) /\
  B.loc_disjoint (B.loc_buffer p.lp_tons) (B.loc_buffer p.lp_rtrigs) /\
  B.loc_disjoint (B.loc_buffer p.lp_tons) (B.loc_buffer p.lp_ftrigs) /\
  B.loc_disjoint (B.loc_buffer p.lp_tofs) (B.loc_buffer p.lp_tps) /\
  B.loc_disjoint (B.loc_buffer p.lp_tofs) (B.loc_buffer p.lp_ctus) /\
  B.loc_disjoint (B.loc_buffer p.lp_tofs) (B.loc_buffer p.lp_ctds) /\
  B.loc_disjoint (B.loc_buffer p.lp_tofs) (B.loc_buffer p.lp_ctuds) /\
  B.loc_disjoint (B.loc_buffer p.lp_tofs) (B.loc_buffer p.lp_srs) /\
  B.loc_disjoint (B.loc_buffer p.lp_tofs) (B.loc_buffer p.lp_rss) /\
  B.loc_disjoint (B.loc_buffer p.lp_tofs) (B.loc_buffer p.lp_rtrigs) /\
  B.loc_disjoint (B.loc_buffer p.lp_tofs) (B.loc_buffer p.lp_ftrigs) /\
  B.loc_disjoint (B.loc_buffer p.lp_tps) (B.loc_buffer p.lp_ctus) /\
  B.loc_disjoint (B.loc_buffer p.lp_tps) (B.loc_buffer p.lp_ctds) /\
  B.loc_disjoint (B.loc_buffer p.lp_tps) (B.loc_buffer p.lp_ctuds) /\
  B.loc_disjoint (B.loc_buffer p.lp_tps) (B.loc_buffer p.lp_srs) /\
  B.loc_disjoint (B.loc_buffer p.lp_tps) (B.loc_buffer p.lp_rss) /\
  B.loc_disjoint (B.loc_buffer p.lp_tps) (B.loc_buffer p.lp_rtrigs) /\
  B.loc_disjoint (B.loc_buffer p.lp_tps) (B.loc_buffer p.lp_ftrigs) /\
  B.loc_disjoint (B.loc_buffer p.lp_ctus) (B.loc_buffer p.lp_ctds) /\
  B.loc_disjoint (B.loc_buffer p.lp_ctus) (B.loc_buffer p.lp_ctuds) /\
  B.loc_disjoint (B.loc_buffer p.lp_ctus) (B.loc_buffer p.lp_srs) /\
  B.loc_disjoint (B.loc_buffer p.lp_ctus) (B.loc_buffer p.lp_rss) /\
  B.loc_disjoint (B.loc_buffer p.lp_ctus) (B.loc_buffer p.lp_rtrigs) /\
  B.loc_disjoint (B.loc_buffer p.lp_ctus) (B.loc_buffer p.lp_ftrigs) /\
  B.loc_disjoint (B.loc_buffer p.lp_ctds) (B.loc_buffer p.lp_ctuds) /\
  B.loc_disjoint (B.loc_buffer p.lp_ctds) (B.loc_buffer p.lp_srs) /\
  B.loc_disjoint (B.loc_buffer p.lp_ctds) (B.loc_buffer p.lp_rss) /\
  B.loc_disjoint (B.loc_buffer p.lp_ctds) (B.loc_buffer p.lp_rtrigs) /\
  B.loc_disjoint (B.loc_buffer p.lp_ctds) (B.loc_buffer p.lp_ftrigs) /\
  B.loc_disjoint (B.loc_buffer p.lp_ctuds) (B.loc_buffer p.lp_srs) /\
  B.loc_disjoint (B.loc_buffer p.lp_ctuds) (B.loc_buffer p.lp_rss) /\
  B.loc_disjoint (B.loc_buffer p.lp_ctuds) (B.loc_buffer p.lp_rtrigs) /\
  B.loc_disjoint (B.loc_buffer p.lp_ctuds) (B.loc_buffer p.lp_ftrigs) /\
  B.loc_disjoint (B.loc_buffer p.lp_srs) (B.loc_buffer p.lp_rss) /\
  B.loc_disjoint (B.loc_buffer p.lp_srs) (B.loc_buffer p.lp_rtrigs) /\
  B.loc_disjoint (B.loc_buffer p.lp_srs) (B.loc_buffer p.lp_ftrigs) /\
  B.loc_disjoint (B.loc_buffer p.lp_rss) (B.loc_buffer p.lp_rtrigs) /\
  B.loc_disjoint (B.loc_buffer p.lp_rss) (B.loc_buffer p.lp_ftrigs) /\
  B.loc_disjoint (B.loc_buffer p.lp_rtrigs) (B.loc_buffer p.lp_ftrigs)

(** Location covering all pool buffers. *)
let pool_loc (p: low_pool) : GTot B.loc =
  B.loc_union (B.loc_buffer p.lp_bools)
  (B.loc_union (B.loc_buffer p.lp_ints)
  (B.loc_union (B.loc_buffer p.lp_tons)
  (B.loc_union (B.loc_buffer p.lp_tofs)
  (B.loc_union (B.loc_buffer p.lp_tps)
  (B.loc_union (B.loc_buffer p.lp_ctus)
  (B.loc_union (B.loc_buffer p.lp_ctds)
  (B.loc_union (B.loc_buffer p.lp_ctuds)
  (B.loc_union (B.loc_buffer p.lp_srs)
  (B.loc_union (B.loc_buffer p.lp_rss)
  (B.loc_union (B.loc_buffer p.lp_rtrigs)
               (B.loc_buffer p.lp_ftrigs)))))))))))

(** Valid pool: live + lengths + disjoint *)
let pool_valid (h: HS.mem) (p: low_pool) : GTot Type0 =
  pool_live h p /\ pool_lengths_ok p /\ pool_disjoint p


(** {1 Safe Pool Accessors} *)

val get_bool_low: p:low_pool -> i:U32.t ->
  Stack bool
    (requires fun h -> pool_live h p /\ pool_lengths_ok p /\ U32.v i < U32.v p.lp_n_bools)
    (ensures fun h0 r h1 -> h0 == h1)
let get_bool_low p i = p.lp_bools.(i)

val set_bool_low: p:low_pool -> i:U32.t -> v:bool ->
  Stack unit
    (requires fun h -> pool_live h p /\ pool_lengths_ok p /\ U32.v i < U32.v p.lp_n_bools)
    (ensures fun h0 () h1 -> B.modifies (B.loc_buffer p.lp_bools) h0 h1 /\ B.live h1 p.lp_bools)
let set_bool_low p i v = p.lp_bools.(i) <- v

val get_int_low: p:low_pool -> i:U32.t ->
  Stack I16.t
    (requires fun h -> pool_live h p /\ pool_lengths_ok p /\ U32.v i < U32.v p.lp_n_ints)
    (ensures fun h0 r h1 -> h0 == h1)
let get_int_low p i = p.lp_ints.(i)

val set_int_low: p:low_pool -> i:U32.t -> v:I16.t ->
  Stack unit
    (requires fun h -> pool_live h p /\ pool_lengths_ok p /\ U32.v i < U32.v p.lp_n_ints)
    (ensures fun h0 () h1 -> B.modifies (B.loc_buffer p.lp_ints) h0 h1 /\ B.live h1 p.lp_ints)
let set_int_low p i v = p.lp_ints.(i) <- v


(** {1 Byte Parsing Helpers (from code buffer)} *)

val read_u16_le_low: code:B.buffer U8.t -> off:U32.t ->
  Stack U16.t
    (requires fun h -> B.live h code /\ U32.v off + 2 <= B.length code)
    (ensures fun h0 r h1 -> h0 == h1)
let read_u16_le_low code off =
  let r0 = code.(off) in
  let r1 = code.(U32.add off 1ul) in
  let b0 = FStar.Int.Cast.uint8_to_uint16 r0 in
  let b1 = FStar.Int.Cast.uint8_to_uint16 r1 in
  U16.add b0 (U16.mul b1 256us)

val read_u32_le_low: code:B.buffer U8.t -> off:U32.t ->
  Stack U32.t
    (requires fun h -> B.live h code /\ U32.v off + 4 <= B.length code)
    (ensures fun h0 r h1 -> h0 == h1)
let read_u32_le_low code off =
  let r0 = code.(off) in
  let r1 = code.(U32.add off 1ul) in
  let r2 = code.(U32.add off 2ul) in
  let r3 = code.(U32.add off 3ul) in
  let b0 = FStar.Int.Cast.uint8_to_uint32 r0 in
  let b1 = FStar.Int.Cast.uint8_to_uint32 r1 in
  let b2 = FStar.Int.Cast.uint8_to_uint32 r2 in
  let b3 = FStar.Int.Cast.uint8_to_uint32 r3 in
  U32.add b0 (U32.add (U32.mul b1 256ul)
    (U32.add (U32.mul b2 65536ul) (U32.mul b3 16777216ul)))

val read_i16_le_low: code:B.buffer U8.t -> off:U32.t ->
  Stack I16.t
    (requires fun h -> B.live h code /\ U32.v off + 2 <= B.length code)
    (ensures fun h0 r h1 -> h0 == h1)
let read_i16_le_low code off =
  let r0 = code.(off) in
  let r1 = code.(U32.add off 1ul) in
  let lo = FStar.Int.Cast.uint8_to_uint16 r0 in
  let hi = FStar.Int.Cast.uint8_to_uint16 r1 in
  let raw = U16.add lo (U16.mul hi 256us) in
  FStar.Int.Cast.uint16_to_int16 raw


(** {1 Wrapping INT Arithmetic} *)

let wrap_i16_low (x: I16.t) : I16.t = x  (* i16 wraps naturally in C *)

let add_wrap_low (a b: I16.t) : I16.t =
  FStar.Int.Cast.int32_to_int16
    (FStar.Int32.add (FStar.Int.Cast.int16_to_int32 a)
                     (FStar.Int.Cast.int16_to_int32 b))

let sub_wrap_low (a b: I16.t) : I16.t =
  FStar.Int.Cast.int32_to_int16
    (FStar.Int32.sub (FStar.Int.Cast.int16_to_int32 a)
                     (FStar.Int.Cast.int16_to_int32 b))

let mul_wrap_low (a b: I16.t) : I16.t =
  FStar.Int.Cast.int32_to_int16
    (FStar.Int32.mul (FStar.Int.Cast.int16_to_int32 a)
                     (FStar.Int.Cast.int16_to_int32 b))


(** {1 TLV Opcode Tags (U8.t for machine comparison)} *)

let tag_load_true_u8  : U8.t = 0x01uy
let tag_load_false_u8 : U8.t = 0x02uy
let tag_load_int_u8   : U8.t = 0x03uy

let tag_bool_not_u8  : U8.t = 0x10uy
let tag_bool_and_u8  : U8.t = 0x11uy
let tag_bool_or_u8   : U8.t = 0x12uy
let tag_bool_xor_u8  : U8.t = 0x13uy
let tag_copy_bool_u8 : U8.t = 0x14uy

let tag_int_add_u8  : U8.t = 0x20uy
let tag_int_sub_u8  : U8.t = 0x21uy
let tag_int_mul_u8  : U8.t = 0x22uy
let tag_copy_int_u8 : U8.t = 0x23uy

let tag_cmp_eq_u8 : U8.t = 0x30uy
let tag_cmp_ne_u8 : U8.t = 0x31uy
let tag_cmp_lt_u8 : U8.t = 0x32uy
let tag_cmp_gt_u8 : U8.t = 0x33uy
let tag_cmp_le_u8 : U8.t = 0x34uy
let tag_cmp_ge_u8 : U8.t = 0x35uy

let tag_if_block_u8  : U8.t = 0x40uy
let tag_for_block_u8 : U8.t = 0x41uy

let tag_ton_call_u8  : U8.t = 0x50uy
let tag_tof_call_u8  : U8.t = 0x51uy
let tag_tp_call_u8   : U8.t = 0x52uy

let tag_ctu_call_u8  : U8.t = 0x60uy
let tag_ctd_call_u8  : U8.t = 0x61uy
let tag_ctud_call_u8 : U8.t = 0x62uy

let tag_sr_call_u8    : U8.t = 0x70uy
let tag_rs_call_u8    : U8.t = 0x71uy
let tag_rtrig_call_u8 : U8.t = 0x72uy
let tag_ftrig_call_u8 : U8.t = 0x73uy


(** {1 ADR-009: pt_us to pt_ticks (LowStar)} *)

let pt_us_to_ticks_low (pt_us: U32.t) (scan_period_us: U32.t) : U32.t =
  if scan_period_us = 0ul then 0ul
  else U32.div pt_us scan_period_us


(** {1 Atomic Opcode Execution - Simple Leaf Opcodes (LowStar)}

    Handles the 18 non-FB leaf opcodes: loads (3), booleans (5),
    integer arithmetic (4), comparisons (6).

    FB calls are handled separately in exec_fb_call_low.
    Compound opcodes (IF_BLOCK, FOR_BLOCK) are handled by the caller. *)


#push-options "--z3rlimit 200"

val exec_simple_low: code:B.buffer U8.t -> code_len:U32.t ->
  off:U32.t -> tag:U8.t -> len:U16.t -> p:low_pool ->
  Stack unit
    (requires fun h ->
      B.live h code /\ pool_valid h p /\
      B.loc_disjoint (B.loc_buffer code) (pool_loc p) /\
      B.length code = U32.v code_len /\
      U32.v off + 3 <= U32.v code_len /\
      U32.v off + 3 + U16.v len <= U32.v code_len)
    (ensures fun h0 () h1 ->
      B.modifies (B.loc_union (B.loc_buffer p.lp_bools) (B.loc_buffer p.lp_ints)) h0 h1 /\
      pool_live h1 p)

let exec_simple_low code code_len off tag len p =
  let poff = U32.add off 3ul in
  let len32 = FStar.Int.Cast.uint16_to_uint32 len in
  if U32.gt (U32.add poff len32) code_len then ()

  (* S6.1 Literal Loads *)
  else if tag = tag_load_true_u8 && U16.gte len 1us then
    let idx = code.(poff) in
    let idx32 = FStar.Int.Cast.uint8_to_uint32 idx in
    if U32.lt idx32 p.lp_n_bools then
      set_bool_low p idx32 true
    else ()

  else if tag = tag_load_false_u8 && U16.gte len 1us then
    let idx = code.(poff) in
    let idx32 = FStar.Int.Cast.uint8_to_uint32 idx in
    if U32.lt idx32 p.lp_n_bools then
      set_bool_low p idx32 false
    else ()

  else if tag = tag_load_int_u8 && U16.gte len 3us then
    let idx = code.(poff) in
    let idx32 = FStar.Int.Cast.uint8_to_uint32 idx in
    if U32.lt idx32 p.lp_n_ints then
      let v = read_i16_le_low code (U32.add poff 1ul) in
      set_int_low p idx32 v
    else ()

  (* S6.2 Boolean Operations *)
  else if tag = tag_bool_not_u8 && U16.gte len 2us then
    let dst = code.(poff) in
    let src = code.(U32.add poff 1ul) in
    let dst32 = FStar.Int.Cast.uint8_to_uint32 dst in
    let src32 = FStar.Int.Cast.uint8_to_uint32 src in
    if U32.lt dst32 p.lp_n_bools &&
       U32.lt src32 p.lp_n_bools then
      let v = get_bool_low p src32 in
      set_bool_low p dst32 (not v)
    else ()

  else if tag = tag_bool_and_u8 && U16.gte len 3us then
    let dst = code.(poff) in
    let a   = code.(U32.add poff 1ul) in
    let b   = code.(U32.add poff 2ul) in
    let dst32 = FStar.Int.Cast.uint8_to_uint32 dst in
    let a32   = FStar.Int.Cast.uint8_to_uint32 a in
    let b32   = FStar.Int.Cast.uint8_to_uint32 b in
    if U32.lt dst32 p.lp_n_bools &&
       U32.lt a32   p.lp_n_bools &&
       U32.lt b32   p.lp_n_bools then
      let va = get_bool_low p a32 in
      let vb = get_bool_low p b32 in
      set_bool_low p dst32 (va && vb)
    else ()

  else if tag = tag_bool_or_u8 && U16.gte len 3us then
    let dst = code.(poff) in
    let a   = code.(U32.add poff 1ul) in
    let b   = code.(U32.add poff 2ul) in
    let dst32 = FStar.Int.Cast.uint8_to_uint32 dst in
    let a32   = FStar.Int.Cast.uint8_to_uint32 a in
    let b32   = FStar.Int.Cast.uint8_to_uint32 b in
    if U32.lt dst32 p.lp_n_bools &&
       U32.lt a32   p.lp_n_bools &&
       U32.lt b32   p.lp_n_bools then
      let va = get_bool_low p a32 in
      let vb = get_bool_low p b32 in
      set_bool_low p dst32 (va || vb)
    else ()

  else if tag = tag_bool_xor_u8 && U16.gte len 3us then
    let dst = code.(poff) in
    let a   = code.(U32.add poff 1ul) in
    let b   = code.(U32.add poff 2ul) in
    let dst32 = FStar.Int.Cast.uint8_to_uint32 dst in
    let a32   = FStar.Int.Cast.uint8_to_uint32 a in
    let b32   = FStar.Int.Cast.uint8_to_uint32 b in
    if U32.lt dst32 p.lp_n_bools &&
       U32.lt a32   p.lp_n_bools &&
       U32.lt b32   p.lp_n_bools then
      let va = get_bool_low p a32 in
      let vb = get_bool_low p b32 in
      set_bool_low p dst32 (va <> vb)
    else ()

  else if tag = tag_copy_bool_u8 && U16.gte len 2us then
    let dst = code.(poff) in
    let src = code.(U32.add poff 1ul) in
    let dst32 = FStar.Int.Cast.uint8_to_uint32 dst in
    let src32 = FStar.Int.Cast.uint8_to_uint32 src in
    if U32.lt dst32 p.lp_n_bools &&
       U32.lt src32 p.lp_n_bools then
      let v = get_bool_low p src32 in
      set_bool_low p dst32 v
    else ()

  (* S6.3 Integer Arithmetic - wrapping per DDR-003 *)
  else if tag = tag_int_add_u8 && U16.gte len 3us then
    let dst = code.(poff) in
    let a   = code.(U32.add poff 1ul) in
    let b   = code.(U32.add poff 2ul) in
    let dst32 = FStar.Int.Cast.uint8_to_uint32 dst in
    let a32   = FStar.Int.Cast.uint8_to_uint32 a in
    let b32   = FStar.Int.Cast.uint8_to_uint32 b in
    if U32.lt dst32 p.lp_n_ints &&
       U32.lt a32   p.lp_n_ints &&
       U32.lt b32   p.lp_n_ints then
      let va = get_int_low p a32 in
      let vb = get_int_low p b32 in
      set_int_low p dst32 (add_wrap_low va vb)
    else ()

  else if tag = tag_int_sub_u8 && U16.gte len 3us then
    let dst = code.(poff) in
    let a   = code.(U32.add poff 1ul) in
    let b   = code.(U32.add poff 2ul) in
    let dst32 = FStar.Int.Cast.uint8_to_uint32 dst in
    let a32   = FStar.Int.Cast.uint8_to_uint32 a in
    let b32   = FStar.Int.Cast.uint8_to_uint32 b in
    if U32.lt dst32 p.lp_n_ints &&
       U32.lt a32   p.lp_n_ints &&
       U32.lt b32   p.lp_n_ints then
      let va = get_int_low p a32 in
      let vb = get_int_low p b32 in
      set_int_low p dst32 (sub_wrap_low va vb)
    else ()

  else if tag = tag_int_mul_u8 && U16.gte len 3us then
    let dst = code.(poff) in
    let a   = code.(U32.add poff 1ul) in
    let b   = code.(U32.add poff 2ul) in
    let dst32 = FStar.Int.Cast.uint8_to_uint32 dst in
    let a32   = FStar.Int.Cast.uint8_to_uint32 a in
    let b32   = FStar.Int.Cast.uint8_to_uint32 b in
    if U32.lt dst32 p.lp_n_ints &&
       U32.lt a32   p.lp_n_ints &&
       U32.lt b32   p.lp_n_ints then
      let va = get_int_low p a32 in
      let vb = get_int_low p b32 in
      set_int_low p dst32 (mul_wrap_low va vb)
    else ()

  else if tag = tag_copy_int_u8 && U16.gte len 2us then
    let dst = code.(poff) in
    let src = code.(U32.add poff 1ul) in
    let dst32 = FStar.Int.Cast.uint8_to_uint32 dst in
    let src32 = FStar.Int.Cast.uint8_to_uint32 src in
    if U32.lt dst32 p.lp_n_ints &&
       U32.lt src32 p.lp_n_ints then
      let v = get_int_low p src32 in
      set_int_low p dst32 v
    else ()

  (* S6.4 Integer Comparisons *)
  else if tag = tag_cmp_eq_u8 && U16.gte len 3us then
    let dst = code.(poff) in
    let a   = code.(U32.add poff 1ul) in
    let b   = code.(U32.add poff 2ul) in
    let dst32 = FStar.Int.Cast.uint8_to_uint32 dst in
    let a32   = FStar.Int.Cast.uint8_to_uint32 a in
    let b32   = FStar.Int.Cast.uint8_to_uint32 b in
    if U32.lt dst32 p.lp_n_bools &&
       U32.lt a32   p.lp_n_ints &&
       U32.lt b32   p.lp_n_ints then
      let va = get_int_low p a32 in
      let vb = get_int_low p b32 in
      set_bool_low p dst32 (va = vb)
    else ()

  else if tag = tag_cmp_ne_u8 && U16.gte len 3us then
    let dst = code.(poff) in
    let a   = code.(U32.add poff 1ul) in
    let b   = code.(U32.add poff 2ul) in
    let dst32 = FStar.Int.Cast.uint8_to_uint32 dst in
    let a32   = FStar.Int.Cast.uint8_to_uint32 a in
    let b32   = FStar.Int.Cast.uint8_to_uint32 b in
    if U32.lt dst32 p.lp_n_bools &&
       U32.lt a32   p.lp_n_ints &&
       U32.lt b32   p.lp_n_ints then
      let va = get_int_low p a32 in
      let vb = get_int_low p b32 in
      set_bool_low p dst32 (not (va = vb))
    else ()

  else if tag = tag_cmp_lt_u8 && U16.gte len 3us then
    let dst = code.(poff) in
    let a   = code.(U32.add poff 1ul) in
    let b   = code.(U32.add poff 2ul) in
    let dst32 = FStar.Int.Cast.uint8_to_uint32 dst in
    let a32   = FStar.Int.Cast.uint8_to_uint32 a in
    let b32   = FStar.Int.Cast.uint8_to_uint32 b in
    if U32.lt dst32 p.lp_n_bools &&
       U32.lt a32   p.lp_n_ints &&
       U32.lt b32   p.lp_n_ints then
      let va = get_int_low p a32 in
      let vb = get_int_low p b32 in
      set_bool_low p dst32 (I16.lt va vb)
    else ()

  else if tag = tag_cmp_gt_u8 && U16.gte len 3us then
    let dst = code.(poff) in
    let a   = code.(U32.add poff 1ul) in
    let b   = code.(U32.add poff 2ul) in
    let dst32 = FStar.Int.Cast.uint8_to_uint32 dst in
    let a32   = FStar.Int.Cast.uint8_to_uint32 a in
    let b32   = FStar.Int.Cast.uint8_to_uint32 b in
    if U32.lt dst32 p.lp_n_bools &&
       U32.lt a32   p.lp_n_ints &&
       U32.lt b32   p.lp_n_ints then
      let va = get_int_low p a32 in
      let vb = get_int_low p b32 in
      set_bool_low p dst32 (I16.gt va vb)
    else ()

  else if tag = tag_cmp_le_u8 && U16.gte len 3us then
    let dst = code.(poff) in
    let a   = code.(U32.add poff 1ul) in
    let b   = code.(U32.add poff 2ul) in
    let dst32 = FStar.Int.Cast.uint8_to_uint32 dst in
    let a32   = FStar.Int.Cast.uint8_to_uint32 a in
    let b32   = FStar.Int.Cast.uint8_to_uint32 b in
    if U32.lt dst32 p.lp_n_bools &&
       U32.lt a32   p.lp_n_ints &&
       U32.lt b32   p.lp_n_ints then
      let va = get_int_low p a32 in
      let vb = get_int_low p b32 in
      set_bool_low p dst32 (I16.lte va vb)
    else ()

  else if tag = tag_cmp_ge_u8 && U16.gte len 3us then
    let dst = code.(poff) in
    let a   = code.(U32.add poff 1ul) in
    let b   = code.(U32.add poff 2ul) in
    let dst32 = FStar.Int.Cast.uint8_to_uint32 dst in
    let a32   = FStar.Int.Cast.uint8_to_uint32 a in
    let b32   = FStar.Int.Cast.uint8_to_uint32 b in
    if U32.lt dst32 p.lp_n_bools &&
       U32.lt a32   p.lp_n_ints &&
       U32.lt b32   p.lp_n_ints then
      let va = get_int_low p a32 in
      let vb = get_int_low p b32 in
      set_bool_low p dst32 (I16.gte va vb)
    else ()

  else ()  (* Unknown/FB/compound - handled by caller *)


(** {1 FB State Accessors}

    Read/write FB instance state from pool buffers.
    Each FB type has a flat struct stored in a buffer. *)

val get_sr_state_low: p:low_pool -> idx:U32.t ->
  Stack sr_flat
    (requires fun h -> pool_live h p /\ pool_lengths_ok p /\ U32.v idx < U32.v p.lp_n_sr)
    (ensures fun h0 _ h1 -> h0 == h1)
let get_sr_state_low p idx = p.lp_srs.(idx)

val set_sr_state_low: p:low_pool -> idx:U32.t -> v:sr_flat ->
  Stack unit
    (requires fun h -> pool_live h p /\ pool_lengths_ok p /\ U32.v idx < U32.v p.lp_n_sr)
    (ensures fun h0 () h1 -> B.modifies (B.loc_buffer p.lp_srs) h0 h1 /\ B.live h1 p.lp_srs)
let set_sr_state_low p idx v = p.lp_srs.(idx) <- v

val get_rs_state_low: p:low_pool -> idx:U32.t ->
  Stack rs_flat
    (requires fun h -> pool_live h p /\ pool_lengths_ok p /\ U32.v idx < U32.v p.lp_n_rs)
    (ensures fun h0 _ h1 -> h0 == h1)
let get_rs_state_low p idx = p.lp_rss.(idx)

val set_rs_state_low: p:low_pool -> idx:U32.t -> v:rs_flat ->
  Stack unit
    (requires fun h -> pool_live h p /\ pool_lengths_ok p /\ U32.v idx < U32.v p.lp_n_rs)
    (ensures fun h0 () h1 -> B.modifies (B.loc_buffer p.lp_rss) h0 h1 /\ B.live h1 p.lp_rss)
let set_rs_state_low p idx v = p.lp_rss.(idx) <- v

val get_rtrig_state_low: p:low_pool -> idx:U32.t ->
  Stack rtrig_flat
    (requires fun h -> pool_live h p /\ pool_lengths_ok p /\ U32.v idx < U32.v p.lp_n_rtrig)
    (ensures fun h0 _ h1 -> h0 == h1)
let get_rtrig_state_low p idx = p.lp_rtrigs.(idx)

val set_rtrig_state_low: p:low_pool -> idx:U32.t -> v:rtrig_flat ->
  Stack unit
    (requires fun h -> pool_live h p /\ pool_lengths_ok p /\ U32.v idx < U32.v p.lp_n_rtrig)
    (ensures fun h0 () h1 -> B.modifies (B.loc_buffer p.lp_rtrigs) h0 h1 /\ B.live h1 p.lp_rtrigs)
let set_rtrig_state_low p idx v = p.lp_rtrigs.(idx) <- v

val get_ftrig_state_low: p:low_pool -> idx:U32.t ->
  Stack ftrig_flat
    (requires fun h -> pool_live h p /\ pool_lengths_ok p /\ U32.v idx < U32.v p.lp_n_ftrig)
    (ensures fun h0 _ h1 -> h0 == h1)
let get_ftrig_state_low p idx = p.lp_ftrigs.(idx)

val set_ftrig_state_low: p:low_pool -> idx:U32.t -> v:ftrig_flat ->
  Stack unit
    (requires fun h -> pool_live h p /\ pool_lengths_ok p /\ U32.v idx < U32.v p.lp_n_ftrig)
    (ensures fun h0 () h1 -> B.modifies (B.loc_buffer p.lp_ftrigs) h0 h1 /\ B.live h1 p.lp_ftrigs)
let set_ftrig_state_low p idx v = p.lp_ftrigs.(idx) <- v

val get_ctu_state_low: p:low_pool -> idx:U32.t ->
  Stack ctu_flat
    (requires fun h -> pool_live h p /\ pool_lengths_ok p /\ U32.v idx < U32.v p.lp_n_ctu)
    (ensures fun h0 _ h1 -> h0 == h1)
let get_ctu_state_low p idx = p.lp_ctus.(idx)

val set_ctu_state_low: p:low_pool -> idx:U32.t -> v:ctu_flat ->
  Stack unit
    (requires fun h -> pool_live h p /\ pool_lengths_ok p /\ U32.v idx < U32.v p.lp_n_ctu)
    (ensures fun h0 () h1 -> B.modifies (B.loc_buffer p.lp_ctus) h0 h1 /\ B.live h1 p.lp_ctus)
let set_ctu_state_low p idx v = p.lp_ctus.(idx) <- v

val get_ctd_state_low: p:low_pool -> idx:U32.t ->
  Stack ctd_flat
    (requires fun h -> pool_live h p /\ pool_lengths_ok p /\ U32.v idx < U32.v p.lp_n_ctd)
    (ensures fun h0 _ h1 -> h0 == h1)
let get_ctd_state_low p idx = p.lp_ctds.(idx)

val set_ctd_state_low: p:low_pool -> idx:U32.t -> v:ctd_flat ->
  Stack unit
    (requires fun h -> pool_live h p /\ pool_lengths_ok p /\ U32.v idx < U32.v p.lp_n_ctd)
    (ensures fun h0 () h1 -> B.modifies (B.loc_buffer p.lp_ctds) h0 h1 /\ B.live h1 p.lp_ctds)
let set_ctd_state_low p idx v = p.lp_ctds.(idx) <- v

val get_ctud_state_low: p:low_pool -> idx:U32.t ->
  Stack ctud_flat
    (requires fun h -> pool_live h p /\ pool_lengths_ok p /\ U32.v idx < U32.v p.lp_n_ctud)
    (ensures fun h0 _ h1 -> h0 == h1)
let get_ctud_state_low p idx = p.lp_ctuds.(idx)

val set_ctud_state_low: p:low_pool -> idx:U32.t -> v:ctud_flat ->
  Stack unit
    (requires fun h -> pool_live h p /\ pool_lengths_ok p /\ U32.v idx < U32.v p.lp_n_ctud)
    (ensures fun h0 () h1 -> B.modifies (B.loc_buffer p.lp_ctuds) h0 h1 /\ B.live h1 p.lp_ctuds)
let set_ctud_state_low p idx v = p.lp_ctuds.(idx) <- v

val get_ton_state_low: p:low_pool -> idx:U32.t ->
  Stack ton_flat
    (requires fun h -> pool_live h p /\ pool_lengths_ok p /\ U32.v idx < U32.v p.lp_n_ton)
    (ensures fun h0 _ h1 -> h0 == h1)
let get_ton_state_low p idx = p.lp_tons.(idx)

val set_ton_state_low: p:low_pool -> idx:U32.t -> v:ton_flat ->
  Stack unit
    (requires fun h -> pool_live h p /\ pool_lengths_ok p /\ U32.v idx < U32.v p.lp_n_ton)
    (ensures fun h0 () h1 -> B.modifies (B.loc_buffer p.lp_tons) h0 h1 /\ B.live h1 p.lp_tons)
let set_ton_state_low p idx v = p.lp_tons.(idx) <- v

val get_tof_state_low: p:low_pool -> idx:U32.t ->
  Stack tof_flat
    (requires fun h -> pool_live h p /\ pool_lengths_ok p /\ U32.v idx < U32.v p.lp_n_tof)
    (ensures fun h0 _ h1 -> h0 == h1)
let get_tof_state_low p idx = p.lp_tofs.(idx)

val set_tof_state_low: p:low_pool -> idx:U32.t -> v:tof_flat ->
  Stack unit
    (requires fun h -> pool_live h p /\ pool_lengths_ok p /\ U32.v idx < U32.v p.lp_n_tof)
    (ensures fun h0 () h1 -> B.modifies (B.loc_buffer p.lp_tofs) h0 h1 /\ B.live h1 p.lp_tofs)
let set_tof_state_low p idx v = p.lp_tofs.(idx) <- v

val get_tp_state_low: p:low_pool -> idx:U32.t ->
  Stack tp_flat
    (requires fun h -> pool_live h p /\ pool_lengths_ok p /\ U32.v idx < U32.v p.lp_n_tp)
    (ensures fun h0 _ h1 -> h0 == h1)
let get_tp_state_low p idx = p.lp_tps.(idx)

val set_tp_state_low: p:low_pool -> idx:U32.t -> v:tp_flat ->
  Stack unit
    (requires fun h -> pool_live h p /\ pool_lengths_ok p /\ U32.v idx < U32.v p.lp_n_tp)
    (ensures fun h0 () h1 -> B.modifies (B.loc_buffer p.lp_tps) h0 h1 /\ B.live h1 p.lp_tps)
let set_tp_state_low p idx v = p.lp_tps.(idx) <- v


(** {1 FB Call Dispatch}

    Handles the 10 FB opcodes: timers (3), counters (3),
    bistables (2), edge detectors (2).
    Uses pure combinator step functions inline - KaRaMeL inlines them. *)

val exec_fb_call_low: code:B.buffer U8.t -> code_len:U32.t ->
  off:U32.t -> tag:U8.t -> len:U16.t -> p:low_pool ->
  Stack unit
    (requires fun h ->
      B.live h code /\ pool_valid h p /\
      B.loc_disjoint (B.loc_buffer code) (pool_loc p) /\
      B.length code = U32.v code_len /\
      U32.v off + 3 <= U32.v code_len /\
      U32.v off + 3 + U16.v len <= U32.v code_len)
    (ensures fun h0 () h1 ->
      B.modifies (pool_loc p) h0 h1 /\
      pool_live h1 p)

let exec_fb_call_low code code_len off tag len p =
  let poff = U32.add off 3ul in

  (* S6.8 Bistable: SR *)
  if tag = tag_sr_call_u8 && U16.gte len 4us then
    let idx_raw = code.(poff) in
    let idx = FStar.Int.Cast.uint8_to_uint32 idx_raw in
    if U32.lt idx p.lp_n_sr then
      let _r1 = code.(U32.add poff 1ul) in

      let s1_b = FStar.Int.Cast.uint8_to_uint32 _r1 in
      let _r2 = code.(U32.add poff 2ul) in

      let r_b = FStar.Int.Cast.uint8_to_uint32 _r2 in
      let _r3 = code.(U32.add poff 3ul) in

      let q1_b = FStar.Int.Cast.uint8_to_uint32 _r3 in
      if U32.lt s1_b p.lp_n_bools && U32.lt r_b p.lp_n_bools &&
         U32.lt q1_b p.lp_n_bools then
        let s = get_sr_state_low p idx in
        let old_q = s.srf_q <> 0uy in
        let s1_v = get_bool_low p s1_b in
        let r_v  = get_bool_low p r_b in
        let new_q = s1_v || (not r_v && old_q) in
        set_sr_state_low p idx { srf_q = (if new_q then 1uy else 0uy) };
        set_bool_low p q1_b new_q
      else ()
    else ()

  (* S6.8 Bistable: RS *)
  else if tag = tag_rs_call_u8 && U16.gte len 4us then
    let idx_raw = code.(poff) in
    let idx = FStar.Int.Cast.uint8_to_uint32 idx_raw in
    if U32.lt idx p.lp_n_rs then
      let _r4 = code.(U32.add poff 1ul) in

      let s_b = FStar.Int.Cast.uint8_to_uint32 _r4 in
      let _r5 = code.(U32.add poff 2ul) in

      let r1_b = FStar.Int.Cast.uint8_to_uint32 _r5 in
      let _r6 = code.(U32.add poff 3ul) in

      let q1_b = FStar.Int.Cast.uint8_to_uint32 _r6 in
      if U32.lt s_b p.lp_n_bools && U32.lt r1_b p.lp_n_bools &&
         U32.lt q1_b p.lp_n_bools then
        let s = get_rs_state_low p idx in
        let old_q = s.rsf_q <> 0uy in
        let s_v  = get_bool_low p s_b in
        let r1_v = get_bool_low p r1_b in
        let new_q = not r1_v && (s_v || old_q) in
        set_rs_state_low p idx { rsf_q = (if new_q then 1uy else 0uy) };
        set_bool_low p q1_b new_q
      else ()
    else ()

  (* S6.8 Edge: R_TRIG *)
  else if tag = tag_rtrig_call_u8 && U16.gte len 3us then
    let idx_raw = code.(poff) in
    let idx = FStar.Int.Cast.uint8_to_uint32 idx_raw in
    if U32.lt idx p.lp_n_rtrig then
      let _r7 = code.(U32.add poff 1ul) in

      let clk_b = FStar.Int.Cast.uint8_to_uint32 _r7 in
      let _r8 = code.(U32.add poff 2ul) in

      let q_b = FStar.Int.Cast.uint8_to_uint32 _r8 in
      if U32.lt clk_b p.lp_n_bools && U32.lt q_b p.lp_n_bools then
        let s = get_rtrig_state_low p idx in
        let old_m = s.rtf_m <> 0uy in
        let clk = get_bool_low p clk_b in
        let q = clk && not old_m in
        set_rtrig_state_low p idx { rtf_m = (if clk then 1uy else 0uy) };
        set_bool_low p q_b q
      else ()
    else ()

  (* S6.8 Edge: F_TRIG *)
  else if tag = tag_ftrig_call_u8 && U16.gte len 3us then
    let idx_raw = code.(poff) in
    let idx = FStar.Int.Cast.uint8_to_uint32 idx_raw in
    if U32.lt idx p.lp_n_ftrig then
      let _r9 = code.(U32.add poff 1ul) in

      let clk_b = FStar.Int.Cast.uint8_to_uint32 _r9 in
      let _r10 = code.(U32.add poff 2ul) in

      let q_b = FStar.Int.Cast.uint8_to_uint32 _r10 in
      if U32.lt clk_b p.lp_n_bools && U32.lt q_b p.lp_n_bools then
        let s = get_ftrig_state_low p idx in
        let old_m = s.ftf_m <> 0uy in
        let clk = get_bool_low p clk_b in
        let q = not clk && old_m in
        set_ftrig_state_low p idx { ftf_m = (if clk then 1uy else 0uy) };
        set_bool_low p q_b q
      else ()
    else ()

  (* S6.7 Counter: CTU *)
  else if tag = tag_ctu_call_u8 && U16.gte len 6us then
    let idx_raw = code.(poff) in
    let idx = FStar.Int.Cast.uint8_to_uint32 idx_raw in
    if U32.lt idx p.lp_n_ctu then
      let _r11 = code.(U32.add poff 1ul) in

      let cu_b = FStar.Int.Cast.uint8_to_uint32 _r11 in
      let _r12 = code.(U32.add poff 2ul) in

      let r_b = FStar.Int.Cast.uint8_to_uint32 _r12 in
      let _r13 = code.(U32.add poff 3ul) in

      let pv_i = FStar.Int.Cast.uint8_to_uint32 _r13 in
      let _r14 = code.(U32.add poff 4ul) in

      let q_b = FStar.Int.Cast.uint8_to_uint32 _r14 in
      let _r15 = code.(U32.add poff 5ul) in

      let cv_i = FStar.Int.Cast.uint8_to_uint32 _r15 in
      if U32.lt cu_b p.lp_n_bools && U32.lt r_b p.lp_n_bools &&
         U32.lt pv_i p.lp_n_ints && U32.lt q_b p.lp_n_bools &&
         U32.lt cv_i p.lp_n_ints then
        let s = get_ctu_state_low p idx in
        let cu = get_bool_low p cu_b in
        let r  = get_bool_low p r_b in
        let pv = get_int_low p pv_i in
        let prev_cu = s.ctuf_prev_cu <> 0uy in
        let cv = s.ctuf_cv in
        let new_cv =
          if r then 0s
          else if cu && not prev_cu && I16.lt cv pv then add_wrap_low cv 1s
          else cv in
        set_ctu_state_low p idx { ctuf_cv = new_cv; ctuf_prev_cu = (if cu then 1uy else 0uy) };
        set_bool_low p q_b (I16.gte new_cv pv);
        set_int_low p cv_i new_cv
      else ()
    else ()

  (* S6.7 Counter: CTD *)
  else if tag = tag_ctd_call_u8 && U16.gte len 6us then
    let idx_raw = code.(poff) in
    let idx = FStar.Int.Cast.uint8_to_uint32 idx_raw in
    if U32.lt idx p.lp_n_ctd then
      let _r16 = code.(U32.add poff 1ul) in

      let cd_b = FStar.Int.Cast.uint8_to_uint32 _r16 in
      let _r17 = code.(U32.add poff 2ul) in

      let ld_b = FStar.Int.Cast.uint8_to_uint32 _r17 in
      let _r18 = code.(U32.add poff 3ul) in

      let pv_i = FStar.Int.Cast.uint8_to_uint32 _r18 in
      let _r19 = code.(U32.add poff 4ul) in

      let q_b = FStar.Int.Cast.uint8_to_uint32 _r19 in
      let _r20 = code.(U32.add poff 5ul) in

      let cv_i = FStar.Int.Cast.uint8_to_uint32 _r20 in
      if U32.lt cd_b p.lp_n_bools && U32.lt ld_b p.lp_n_bools &&
         U32.lt pv_i p.lp_n_ints && U32.lt q_b p.lp_n_bools &&
         U32.lt cv_i p.lp_n_ints then
        let s = get_ctd_state_low p idx in
        let cd = get_bool_low p cd_b in
        let ld = get_bool_low p ld_b in
        let pv = get_int_low p pv_i in
        let prev_cd = s.ctdf_prev_cd <> 0uy in
        let cv = s.ctdf_cv in
        let new_cv =
          if ld then pv
          else if cd && not prev_cd && I16.gt cv 0s then sub_wrap_low cv 1s
          else cv in
        set_ctd_state_low p idx { ctdf_cv = new_cv; ctdf_prev_cd = (if cd then 1uy else 0uy) };
        set_bool_low p q_b (I16.lte new_cv 0s);
        set_int_low p cv_i new_cv
      else ()
    else ()

  (* S6.7 Counter: CTUD *)
  else if tag = tag_ctud_call_u8 && U16.gte len 9us then
    let idx_raw = code.(poff) in
    let idx = FStar.Int.Cast.uint8_to_uint32 idx_raw in
    if U32.lt idx p.lp_n_ctud then
      let _r21 = code.(U32.add poff 1ul) in

      let cu_b = FStar.Int.Cast.uint8_to_uint32 _r21 in
      let _r22 = code.(U32.add poff 2ul) in

      let cd_b = FStar.Int.Cast.uint8_to_uint32 _r22 in
      let _r23 = code.(U32.add poff 3ul) in

      let r_b = FStar.Int.Cast.uint8_to_uint32 _r23 in
      let _r24 = code.(U32.add poff 4ul) in

      let ld_b = FStar.Int.Cast.uint8_to_uint32 _r24 in
      let _r25 = code.(U32.add poff 5ul) in

      let pv_i = FStar.Int.Cast.uint8_to_uint32 _r25 in
      let _r26 = code.(U32.add poff 6ul) in

      let qu_b = FStar.Int.Cast.uint8_to_uint32 _r26 in
      let _r27 = code.(U32.add poff 7ul) in

      let qd_b = FStar.Int.Cast.uint8_to_uint32 _r27 in
      let _r28 = code.(U32.add poff 8ul) in

      let cv_i = FStar.Int.Cast.uint8_to_uint32 _r28 in
      if U32.lt cu_b p.lp_n_bools && U32.lt cd_b p.lp_n_bools &&
         U32.lt r_b p.lp_n_bools && U32.lt ld_b p.lp_n_bools &&
         U32.lt pv_i p.lp_n_ints && U32.lt qu_b p.lp_n_bools &&
         U32.lt qd_b p.lp_n_bools && U32.lt cv_i p.lp_n_ints then
        let s = get_ctud_state_low p idx in
        let cu = get_bool_low p cu_b in
        let cd = get_bool_low p cd_b in
        let r  = get_bool_low p r_b in
        let ld = get_bool_low p ld_b in
        let pv = get_int_low p pv_i in
        let rising_cu = cu && not (s.ctudf_prev_cu <> 0uy) in
        let rising_cd = cd && not (s.ctudf_prev_cd <> 0uy) in
        let cv = s.ctudf_cv in
        let new_cv =
          if r then 0s
          else if ld then pv
          else if rising_cu && rising_cd then cv  (* simultaneous: cancel *)
          else
            let v1 = if rising_cu && I16.lt cv pv then add_wrap_low cv 1s else cv in
            if rising_cd && I16.gt v1 0s then sub_wrap_low v1 1s else v1 in
        set_ctud_state_low p idx { ctudf_cv = new_cv;
                                    ctudf_prev_cu = (if cu then 1uy else 0uy);
                                    ctudf_prev_cd = (if cd then 1uy else 0uy) };
        set_bool_low p qu_b (I16.gte new_cv pv);
        set_bool_low p qd_b (I16.lte new_cv 0s);
        set_int_low p cv_i new_cv
      else ()
    else ()

  (* S6.6 Timer: TON - inline step logic using flat state *)
  else if tag = tag_ton_call_u8 && U16.gte len 7us then
    let idx_raw = code.(poff) in
    let idx = FStar.Int.Cast.uint8_to_uint32 idx_raw in
    if U32.lt idx p.lp_n_ton then
      let _r29 = code.(U32.add poff 1ul) in

      let in_b = FStar.Int.Cast.uint8_to_uint32 _r29 in
      let pt_us = read_u32_le_low code (U32.add poff 2ul) in
      let _r30 = code.(U32.add poff 6ul) in

      let q_b = FStar.Int.Cast.uint8_to_uint32 _r30 in
      if U32.lt in_b p.lp_n_bools && U32.lt q_b p.lp_n_bools then
        let pt_ticks = pt_us_to_ticks_low pt_us p.lp_scan_period_us in
        let s = get_ton_state_low p idx in
        if U32.gte p.lp_tick s.tf_start then
          let in_v = get_bool_low p in_b in
          let phase = s.tf_phase in
          if phase = 0uy then begin (* IDLE *)
            if in_v then begin
              set_ton_state_low p idx { tf_phase = 1uy; tf_start = p.lp_tick };
              set_bool_low p q_b false
            end else
              set_bool_low p q_b false
          end
          else if phase = 1uy then begin (* TIMING *)
            if not in_v then begin
              set_ton_state_low p idx { tf_phase = 0uy; tf_start = 0ul };
              set_bool_low p q_b false
            end else begin
              let elapsed = U32.sub p.lp_tick s.tf_start in
              if U32.gte elapsed pt_ticks then begin
                set_ton_state_low p idx { tf_phase = 2uy; tf_start = s.tf_start };
                set_bool_low p q_b true
              end else
                set_bool_low p q_b false
            end
          end
          else begin (* DONE *)
            if not in_v then begin
              set_ton_state_low p idx { tf_phase = 0uy; tf_start = 0ul };
              set_bool_low p q_b false
            end else
              set_bool_low p q_b true
          end
        else ()
      else ()
    else ()

  (* S6.6 Timer: TOF - inline step logic *)
  else if tag = tag_tof_call_u8 && U16.gte len 7us then
    let idx_raw = code.(poff) in
    let idx = FStar.Int.Cast.uint8_to_uint32 idx_raw in
    if U32.lt idx p.lp_n_tof then
      let _r31 = code.(U32.add poff 1ul) in

      let in_b = FStar.Int.Cast.uint8_to_uint32 _r31 in
      let pt_us = read_u32_le_low code (U32.add poff 2ul) in
      let _r32 = code.(U32.add poff 6ul) in

      let q_b = FStar.Int.Cast.uint8_to_uint32 _r32 in
      if U32.lt in_b p.lp_n_bools && U32.lt q_b p.lp_n_bools then
        let pt_ticks = pt_us_to_ticks_low pt_us p.lp_scan_period_us in
        let s = get_tof_state_low p idx in
        if U32.gte p.lp_tick s.tff_start then
          let in_v = get_bool_low p in_b in
          let phase = s.tff_phase in
          if phase = 0uy then begin (* IDLE *)
            if in_v then begin
              set_tof_state_low p idx { tff_phase = 0uy; tff_prev_in = 1uy; tff_start = s.tff_start };
              set_bool_low p q_b true
            end else if s.tff_prev_in <> 0uy then begin
              set_tof_state_low p idx { tff_phase = 1uy; tff_prev_in = 0uy; tff_start = p.lp_tick };
              set_bool_low p q_b true
            end else
              set_bool_low p q_b false
          end
          else if phase = 1uy then begin (* TIMING *)
            if in_v then begin
              set_tof_state_low p idx { tff_phase = 0uy; tff_prev_in = 1uy; tff_start = s.tff_start };
              set_bool_low p q_b true
            end else begin
              let elapsed = U32.sub p.lp_tick s.tff_start in
              if U32.gte elapsed pt_ticks then begin
                set_tof_state_low p idx { tff_phase = 0uy; tff_prev_in = 0uy; tff_start = s.tff_start };
                set_bool_low p q_b false
              end else
                set_bool_low p q_b true
            end
          end
          else
            set_bool_low p q_b false
        else ()
      else ()
    else ()

  (* S6.6 Timer: TP - inline step logic *)
  else if tag = tag_tp_call_u8 && U16.gte len 7us then
    let idx_raw = code.(poff) in
    let idx = FStar.Int.Cast.uint8_to_uint32 idx_raw in
    if U32.lt idx p.lp_n_tp then
      let _r33 = code.(U32.add poff 1ul) in

      let in_b = FStar.Int.Cast.uint8_to_uint32 _r33 in
      let pt_us = read_u32_le_low code (U32.add poff 2ul) in
      let _r34 = code.(U32.add poff 6ul) in

      let q_b = FStar.Int.Cast.uint8_to_uint32 _r34 in
      if U32.lt in_b p.lp_n_bools && U32.lt q_b p.lp_n_bools then
        let pt_ticks = pt_us_to_ticks_low pt_us p.lp_scan_period_us in
        let s = get_tp_state_low p idx in
        if U32.gte p.lp_tick s.tpf_start then
          let in_v = get_bool_low p in_b in
          let phase = s.tpf_phase in
          if phase = 0uy then begin (* IDLE *)
            if in_v && not (s.tpf_prev_in <> 0uy) then begin
              set_tp_state_low p idx { tpf_phase = 1uy; tpf_prev_in = 1uy; tpf_start = p.lp_tick };
              set_bool_low p q_b true
            end else begin
              set_tp_state_low p idx { tpf_phase = 0uy;
                                       tpf_prev_in = (if in_v then 1uy else 0uy);
                                       tpf_start = s.tpf_start };
              set_bool_low p q_b false
            end
          end
          else if phase = 1uy then begin (* TIMING *)
            let elapsed = U32.sub p.lp_tick s.tpf_start in
            if U32.gte elapsed pt_ticks then begin
              set_tp_state_low p idx { tpf_phase = 0uy;
                                       tpf_prev_in = (if in_v then 1uy else 0uy);
                                       tpf_start = s.tpf_start };
              set_bool_low p q_b false
            end else begin
              set_tp_state_low p idx { tpf_phase = 1uy;
                                       tpf_prev_in = (if in_v then 1uy else 0uy);
                                       tpf_start = s.tpf_start };
              set_bool_low p q_b true
            end
          end
          else
            set_bool_low p q_b false
        else ()
      else ()
    else ()

  else ()  (* Unknown opcode - skip *)


(** {1 TLV Scanner Loop - Mutually Recursive Stack Functions}

    Four mutually recursive functions mirroring LowPLC.Interp.D5:
      exec_stmts_low     -- scan TLV stream, dispatch each opcode
      exec_if_block_low  -- IF_BLOCK compound opcode
      exec_for_block_low -- FOR_BLOCK compound opcode
      exec_for_iter_low  -- FOR iteration helper

    All use Stack effect with (decreases (U32.v gas)) for termination. *)

let rec exec_stmts_low
  (code: B.buffer U8.t) (code_len: U32.t)
  (off: U32.t) (rem: U32.t) (p: low_pool) (gas: U32.t)
  : Stack unit
      (requires fun h ->
        B.live h code /\ pool_valid h p /\
        B.loc_disjoint (B.loc_buffer code) (pool_loc p) /\
        B.length code = U32.v code_len /\
        U32.v off + U32.v rem <= U32.v code_len)
      (ensures fun h0 () h1 ->
        B.modifies (pool_loc p) h0 h1 /\
        pool_live h1 p)
      (decreases (U32.v gas))
  =
  if gas = 0ul || U32.lt rem 3ul then ()
  else if U32.gt (U32.add off 3ul) code_len then ()
  else
    let r0 = code.(off) in
    let tag = r0 in
    let len16 = read_u16_le_low code (U32.add off 1ul) in
    let len32 = FStar.Int.Cast.uint16_to_uint32 len16 in
    if U32.gt (U32.add 3ul len32) rem then ()
    else if U32.gt len32 (U32.sub (U32.sub code_len off) 3ul) then ()
    else begin
      let next_off = U32.add (U32.add off 3ul) len32 in
      let consumed = U32.add 3ul len32 in
      let new_rem = U32.sub rem consumed in
      if tag = tag_if_block_u8 then
        exec_if_block_low code code_len (U32.add off 3ul) len16 p (U32.sub gas 1ul)
      else if tag = tag_for_block_u8 then
        exec_for_block_low code code_len (U32.add off 3ul) len16 p (U32.sub gas 1ul)
      else begin
        exec_simple_low code code_len off tag len16 p;
        exec_fb_call_low code code_len off tag len16 p
      end;
      exec_stmts_low code code_len next_off new_rem p (U32.sub gas 1ul)
    end

and exec_if_block_low
  (code: B.buffer U8.t) (code_len: U32.t)
  (poff: U32.t) (plen: U16.t) (p: low_pool) (gas: U32.t)
  : Stack unit
      (requires fun h ->
        B.live h code /\ pool_valid h p /\
        B.loc_disjoint (B.loc_buffer code) (pool_loc p) /\
        B.length code = U32.v code_len /\
        U32.v poff + U16.v plen <= U32.v code_len)
      (ensures fun h0 () h1 ->
        B.modifies (pool_loc p) h0 h1 /\
        pool_live h1 p)
      (decreases (U32.v gas))
  =
  if gas = 0ul then ()
  else if U32.lt (U32.sub code_len poff) 7ul then ()
  else
    let cond_raw = code.(poff) in
    let cond_idx = FStar.Int.Cast.uint8_to_uint32 cond_raw in
    let then_len16 = read_u16_le_low code (U32.add poff 3ul) in
    let else_len16 = read_u16_le_low code (U32.add poff 5ul) in
    let then_len = FStar.Int.Cast.uint16_to_uint32 then_len16 in
    let else_len = FStar.Int.Cast.uint16_to_uint32 else_len16 in
    let then_off = U32.add poff 7ul in
    if U32.gt then_len (U32.sub code_len then_off) then ()
    else
      let else_off = U32.add then_off then_len in
      if U32.gt else_len (U32.sub code_len else_off) then ()
      else if U32.lt cond_idx p.lp_n_bools then
        let cond_v = get_bool_low p cond_idx in
        if cond_v then
          exec_stmts_low code code_len then_off then_len p (U32.sub gas 1ul)
        else if U32.gt else_len 0ul then
          exec_stmts_low code code_len else_off else_len p (U32.sub gas 1ul)
        else ()
      else ()

and exec_for_block_low
  (code: B.buffer U8.t) (code_len: U32.t)
  (poff: U32.t) (plen: U16.t) (p: low_pool) (gas: U32.t)
  : Stack unit
      (requires fun h ->
        B.live h code /\ pool_valid h p /\
        B.loc_disjoint (B.loc_buffer code) (pool_loc p) /\
        B.length code = U32.v code_len /\
        U32.v poff + U16.v plen <= U32.v code_len)
      (ensures fun h0 () h1 ->
        B.modifies (pool_loc p) h0 h1 /\
        pool_live h1 p)
      (decreases (U32.v gas))
  =
  if gas = 0ul then ()
  else if U32.lt (U32.sub code_len poff) 8ul then ()
  else
    let idx_raw = code.(poff) in
    let idx32 = FStar.Int.Cast.uint8_to_uint32 idx_raw in
    let start_v = read_i16_le_low code (U32.add poff 1ul) in
    let end_v   = read_i16_le_low code (U32.add poff 3ul) in
    let body_len16 = read_u16_le_low code (U32.add poff 6ul) in
    let body_len = FStar.Int.Cast.uint16_to_uint32 body_len16 in
    let body_off = U32.add poff 8ul in
    if U32.gt body_len (U32.sub code_len body_off) then ()
    else if U32.lt idx32 p.lp_n_ints then begin
      set_int_low p idx32 start_v;
      let iters =
        if I16.gte end_v start_v then
          let e32 = FStar.Int.Cast.int16_to_int32 end_v in
          let s32 = FStar.Int.Cast.int16_to_int32 start_v in
          let diff_plus_1 = FStar.Int32.add (FStar.Int32.sub e32 s32) 1l in
          if FStar.Int32.gte diff_plus_1 0l then
            FStar.Int.Cast.int32_to_uint32 diff_plus_1
          else 0ul
        else 0ul in
      exec_for_iter_low code code_len body_off body_len idx32 end_v p iters (U32.sub gas 1ul)
    end else ()

and exec_for_iter_low
  (code: B.buffer U8.t) (code_len: U32.t)
  (body_off: U32.t) (body_len: U32.t)
  (idx_i: U32.t) (end_v: I16.t) (p: low_pool)
  (iters_left: U32.t) (gas: U32.t)
  : Stack unit
      (requires fun h ->
        B.live h code /\ pool_valid h p /\
        B.loc_disjoint (B.loc_buffer code) (pool_loc p) /\
        B.length code = U32.v code_len /\
        U32.v body_off + U32.v body_len <= U32.v code_len /\
        U32.v idx_i < U32.v p.lp_n_ints)
      (ensures fun h0 () h1 ->
        B.modifies (pool_loc p) h0 h1 /\
        pool_live h1 p)
      (decreases (U32.v gas))
  =
  if gas = 0ul || iters_left = 0ul then ()
  else begin
    exec_stmts_low code code_len body_off body_len p (U32.sub gas 1ul);
    let cur = get_int_low p idx_i in
    let next = add_wrap_low cur 1s in
    set_int_low p idx_i next;
    exec_for_iter_low code code_len body_off body_len idx_i end_v p
                      (U32.sub iters_left 1ul) (U32.sub gas 1ul)
  end

#pop-options
