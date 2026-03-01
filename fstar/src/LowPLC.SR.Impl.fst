module LowPLC.SR.Impl

open LowPLC.SR
open FStar.UInt8
open FStar.HyperStack.ST
open LowStar.Buffer
open LowStar.BufferOps

module B = LowStar.Buffer
module U8 = FStar.UInt8

val sr_step_c:
    q_state: B.buffer U8.t ->
    s1:      bool ->
    r:       bool ->
    q_out:   B.buffer U8.t ->
    Stack unit
      (requires fun h ->
        B.live h q_state /\ B.live h q_out /\
        B.length q_state = 1 /\ B.length q_out = 1 /\
        B.disjoint q_state q_out)
      (ensures fun h0 () h1 ->
        B.live h1 q_state /\ B.live h1 q_out /\
        B.modifies (B.loc_union (B.loc_buffer q_state) (B.loc_buffer q_out)) h0 h1 /\
        (let old_q = (Seq.index (B.as_seq h0 q_state) 0) <> 0uy in
         let old_s = { q = old_q } in
         let (new_s, out) = sr_step old_s s1 r in
         Seq.index (B.as_seq h1 q_state) 0 = (if new_s.q then 1uy else 0uy) /\
         Seq.index (B.as_seq h1 q_out) 0 = (if out.q1 then 1uy else 0uy)))

let sr_step_c q_state s1 r q_out =
  let old_q = q_state.(0ul) <> 0uy in
  let old_s : sr_state = { q = old_q } in
  let result = sr_step old_s s1 r in
  let new_s = fst result in
  let out = snd result in
  q_state.(0ul) <- (if new_s.q then 1uy else 0uy);
  q_out.(0ul) <- (if out.q1 then 1uy else 0uy)
