(**
 * LowPLC.TON.Impl — Low* (stateful) implementation of TON timer
 *
 * Wraps the pure ton_step specification from LowPLC.TON with mutable
 * buffer operations suitable for KaRaMeL extraction to C.
 *
 * The refinement proof is by construction: we read state from buffers,
 * call the pure spec function, and write results back. KaRaMeL inlines
 * the pure function, producing efficient C.
 *)
module LowPLC.TON.Impl

open LowPLC.TON
open FStar.UInt32
open FStar.UInt8
open FStar.HyperStack.ST
open LowStar.Buffer
open LowStar.BufferOps
open FStar.HyperStack

module B = LowStar.Buffer
module U32 = FStar.UInt32
module U8 = FStar.UInt8
module HS = FStar.HyperStack
module ST = FStar.HyperStack.ST

(** Encode ton_phase as UInt8 for C representation *)
let phase_to_u8 (p: ton_phase) : U8.t =
  match p with
  | TON_IDLE   -> 0uy
  | TON_TIMING -> 1uy
  | TON_DONE   -> 2uy

let u8_to_phase (v: U8.t{U8.v v <= 2}) : ton_phase =
  if v = 0uy then TON_IDLE
  else if v = 1uy then TON_TIMING
  else TON_DONE

(** TON step — stateful Low* implementation.

    Reads current state from phase_buf and start_buf,
    calls the pure ton_step spec, writes results to output buffers.

    Parameters:
      phase_buf  : buffer holding current phase (0/1/2)
      start_buf  : buffer holding start_tick
      in_val     : current IN input
      pt_ticks   : preset time in ticks
      cur_tick   : current scan cycle tick
      q_out      : output buffer for Q
      et_out     : output buffer for ET (elapsed ticks)

    Postcondition: output buffers contain exactly what the pure
    ton_step function would return for the same pre-call state. *)
val ton_step_c:
    phase_buf:  B.buffer U8.t ->
    start_buf:  B.buffer U32.t ->
    in_val:     bool ->
    pt_ticks:   U32.t ->
    cur_tick:   U32.t ->
    q_out:      B.buffer U8.t ->
    et_out:     B.buffer U32.t ->
    Stack unit
      (requires fun h ->
        B.live h phase_buf /\ B.live h start_buf /\
        B.live h q_out /\ B.live h et_out /\
        B.length phase_buf = 1 /\ B.length start_buf = 1 /\
        B.length q_out = 1 /\ B.length et_out = 1 /\
        (* All buffers are disjoint *)
        B.disjoint phase_buf start_buf /\
        B.disjoint phase_buf q_out /\
        B.disjoint phase_buf et_out /\
        B.disjoint start_buf q_out /\
        B.disjoint start_buf et_out /\
        B.disjoint q_out et_out /\
        (* Phase is valid *)
        U8.v (Seq.index (B.as_seq h phase_buf) 0) <= 2 /\
        (* Tick monotonicity invariant *)
        U32.v cur_tick >= U32.v (Seq.index (B.as_seq h start_buf) 0))
      (ensures fun h0 () h1 ->
        B.live h1 phase_buf /\ B.live h1 start_buf /\
        B.live h1 q_out /\ B.live h1 et_out /\
        (* Only our buffers are modified *)
        B.modifies (B.loc_union (B.loc_union (B.loc_buffer phase_buf) (B.loc_buffer start_buf))
                    (B.loc_union (B.loc_buffer q_out) (B.loc_buffer et_out))) h0 h1 /\
        (* Refinement: results match pure spec *)
        (let old_phase = u8_to_phase (Seq.index (B.as_seq h0 phase_buf) 0) in
         let old_start = Seq.index (B.as_seq h0 start_buf) 0 in
         let old_s = { phase = old_phase; start_tick = old_start } in
         let (new_s, out) = ton_step old_s in_val pt_ticks cur_tick in
         Seq.index (B.as_seq h1 phase_buf) 0 = phase_to_u8 new_s.phase /\
         Seq.index (B.as_seq h1 start_buf) 0 = new_s.start_tick /\
         Seq.index (B.as_seq h1 q_out) 0 = (if out.q then 1uy else 0uy) /\
         Seq.index (B.as_seq h1 et_out) 0 = out.et_ticks))

let ton_step_c phase_buf start_buf in_val pt_ticks cur_tick q_out et_out =
  (* Read current state from buffers *)
  let phase_v = phase_buf.(0ul) in
  let start_v = start_buf.(0ul) in

  (* Reconstruct pure state *)
  let old_phase = u8_to_phase phase_v in
  let old_s : ton_state = { phase = old_phase; start_tick = start_v } in

  (* Call pure spec function *)
  let result = ton_step old_s in_val pt_ticks cur_tick in
  let new_s = fst result in
  let out = snd result in

  (* Write new state back to buffers *)
  phase_buf.(0ul) <- phase_to_u8 new_s.phase;
  start_buf.(0ul) <- new_s.start_tick;

  (* Write outputs *)
  q_out.(0ul) <- (if out.q then 1uy else 0uy);
  et_out.(0ul) <- out.et_ticks
