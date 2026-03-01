(**
 * LowPLC.TOF.Impl — Low* (stateful) implementation of TOF off-delay timer
 *
 * Wraps the pure tof_step specification from LowPLC.TOF with mutable
 * buffer operations suitable for KaRaMeL extraction to C.
 *)
module LowPLC.TOF.Impl

open LowPLC.TOF
open FStar.UInt32
open FStar.UInt8
open FStar.HyperStack.ST
open LowStar.Buffer
open LowStar.BufferOps

module B = LowStar.Buffer
module U32 = FStar.UInt32
module U8 = FStar.UInt8

let tof_phase_to_u8 (p: tof_phase) : U8.t =
  match p with
  | TOF_IDLE   -> 0uy
  | TOF_TIMING -> 1uy
  | TOF_DONE   -> 2uy

let u8_to_tof_phase (v: U8.t{U8.v v <= 2}) : tof_phase =
  if v = 0uy then TOF_IDLE
  else if v = 1uy then TOF_TIMING
  else TOF_DONE

val tof_step_c:
    phase_buf:    B.buffer U8.t ->
    prev_in_buf:  B.buffer U8.t ->
    start_buf:    B.buffer U32.t ->
    in_val:       bool ->
    pt_ticks:     U32.t ->
    cur_tick:     U32.t ->
    q_out:        B.buffer U8.t ->
    et_out:       B.buffer U32.t ->
    Stack unit
      (requires fun h ->
        B.live h phase_buf /\ B.live h prev_in_buf /\ B.live h start_buf /\
        B.live h q_out /\ B.live h et_out /\
        B.length phase_buf = 1 /\ B.length prev_in_buf = 1 /\
        B.length start_buf = 1 /\
        B.length q_out = 1 /\ B.length et_out = 1 /\
        B.disjoint phase_buf prev_in_buf /\
        B.disjoint phase_buf start_buf /\
        B.disjoint phase_buf q_out /\
        B.disjoint phase_buf et_out /\
        B.disjoint prev_in_buf start_buf /\
        B.disjoint prev_in_buf q_out /\
        B.disjoint prev_in_buf et_out /\
        B.disjoint start_buf q_out /\
        B.disjoint start_buf et_out /\
        B.disjoint q_out et_out /\
        U8.v (Seq.index (B.as_seq h phase_buf) 0) <= 2 /\
        U32.v cur_tick >= U32.v (Seq.index (B.as_seq h start_buf) 0))
      (ensures fun h0 () h1 ->
        B.live h1 phase_buf /\ B.live h1 prev_in_buf /\ B.live h1 start_buf /\
        B.live h1 q_out /\ B.live h1 et_out /\
        B.modifies (B.loc_union
          (B.loc_union (B.loc_buffer phase_buf)
            (B.loc_union (B.loc_buffer prev_in_buf) (B.loc_buffer start_buf)))
          (B.loc_union (B.loc_buffer q_out) (B.loc_buffer et_out))) h0 h1 /\
        (let old_phase = u8_to_tof_phase (Seq.index (B.as_seq h0 phase_buf) 0) in
         let old_prev = (Seq.index (B.as_seq h0 prev_in_buf) 0) <> 0uy in
         let old_start = Seq.index (B.as_seq h0 start_buf) 0 in
         let old_s = { phase = old_phase; prev_in = old_prev; start_tick = old_start } in
         let (new_s, out) = tof_step old_s in_val pt_ticks cur_tick in
         Seq.index (B.as_seq h1 phase_buf) 0 = tof_phase_to_u8 new_s.phase /\
         Seq.index (B.as_seq h1 prev_in_buf) 0 = (if new_s.prev_in then 1uy else 0uy) /\
         Seq.index (B.as_seq h1 start_buf) 0 = new_s.start_tick /\
         Seq.index (B.as_seq h1 q_out) 0 = (if out.q then 1uy else 0uy) /\
         Seq.index (B.as_seq h1 et_out) 0 = out.et_ticks))

let tof_step_c phase_buf prev_in_buf start_buf in_val pt_ticks cur_tick q_out et_out =
  let phase_v = phase_buf.(0ul) in
  let prev_v = prev_in_buf.(0ul) <> 0uy in
  let start_v = start_buf.(0ul) in
  let old_s : tof_state = { phase = u8_to_tof_phase phase_v; prev_in = prev_v; start_tick = start_v } in
  let result = tof_step old_s in_val pt_ticks cur_tick in
  let new_s = fst result in
  let out = snd result in
  phase_buf.(0ul) <- tof_phase_to_u8 new_s.phase;
  prev_in_buf.(0ul) <- (if new_s.prev_in then 1uy else 0uy);
  start_buf.(0ul) <- new_s.start_tick;
  q_out.(0ul) <- (if out.q then 1uy else 0uy);
  et_out.(0ul) <- out.et_ticks
