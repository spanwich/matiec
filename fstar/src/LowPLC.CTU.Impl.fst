module LowPLC.CTU.Impl

open LowPLC.CTU
open FStar.Int16
open FStar.UInt8
open FStar.HyperStack.ST
open LowStar.Buffer
open LowStar.BufferOps

module B = LowStar.Buffer
module U8 = FStar.UInt8
module I16 = FStar.Int16

val ctu_step_c:
    cv_state:    B.buffer I16.t ->
    prev_state:  B.buffer U8.t ->
    cu:          bool ->
    r:           bool ->
    pv:          I16.t ->
    q_out:       B.buffer U8.t ->
    cv_out:      B.buffer I16.t ->
    Stack unit
      (requires fun h ->
        B.live h cv_state /\ B.live h prev_state /\
        B.live h q_out /\ B.live h cv_out /\
        B.length cv_state = 1 /\ B.length prev_state = 1 /\
        B.length q_out = 1 /\ B.length cv_out = 1 /\
        B.disjoint cv_state prev_state /\
        B.disjoint cv_state q_out /\ B.disjoint cv_state cv_out /\
        B.disjoint prev_state q_out /\ B.disjoint prev_state cv_out /\
        B.disjoint q_out cv_out)
      (ensures fun h0 () h1 ->
        B.live h1 cv_state /\ B.live h1 prev_state /\
        B.live h1 q_out /\ B.live h1 cv_out /\
        B.modifies (B.loc_union
          (B.loc_union (B.loc_buffer cv_state) (B.loc_buffer prev_state))
          (B.loc_union (B.loc_buffer q_out) (B.loc_buffer cv_out))) h0 h1 /\
        (let old_cv = Seq.index (B.as_seq h0 cv_state) 0 in
         let old_prev = (Seq.index (B.as_seq h0 prev_state) 0) <> 0uy in
         let old_s = { cv = old_cv; prev_cu = old_prev } in
         let (new_s, out) = ctu_step old_s cu r pv in
         Seq.index (B.as_seq h1 cv_state) 0 = new_s.cv /\
         Seq.index (B.as_seq h1 prev_state) 0 = (if new_s.prev_cu then 1uy else 0uy) /\
         Seq.index (B.as_seq h1 q_out) 0 = (if out.q then 1uy else 0uy) /\
         Seq.index (B.as_seq h1 cv_out) 0 = out.cv_out))

let ctu_step_c cv_state prev_state cu r pv q_out cv_out =
  let old_cv = cv_state.(0ul) in
  let old_prev = prev_state.(0ul) <> 0uy in
  let old_s : ctu_state = { cv = old_cv; prev_cu = old_prev } in
  let result = ctu_step old_s cu r pv in
  let new_s = fst result in
  let out = snd result in
  cv_state.(0ul) <- new_s.cv;
  prev_state.(0ul) <- (if new_s.prev_cu then 1uy else 0uy);
  q_out.(0ul) <- (if out.q then 1uy else 0uy);
  cv_out.(0ul) <- out.cv_out
