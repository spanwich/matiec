module LowPLC.R_TRIG.Impl

open LowPLC.R_TRIG
open FStar.UInt8
open FStar.HyperStack.ST
open LowStar.Buffer
open LowStar.BufferOps

module B = LowStar.Buffer
module U8 = FStar.UInt8

val r_trig_step_c:
    m_state: B.buffer U8.t ->
    clk:     bool ->
    q_out:   B.buffer U8.t ->
    Stack unit
      (requires fun h ->
        B.live h m_state /\ B.live h q_out /\
        B.length m_state = 1 /\ B.length q_out = 1 /\
        B.disjoint m_state q_out)
      (ensures fun h0 () h1 ->
        B.live h1 m_state /\ B.live h1 q_out /\
        B.modifies (B.loc_union (B.loc_buffer m_state) (B.loc_buffer q_out)) h0 h1 /\
        (let old_m = (Seq.index (B.as_seq h0 m_state) 0) <> 0uy in
         let old_s = { m = old_m } in
         let (new_s, out) = r_trig_step old_s clk in
         Seq.index (B.as_seq h1 m_state) 0 = (if new_s.m then 1uy else 0uy) /\
         Seq.index (B.as_seq h1 q_out) 0 = (if out.q then 1uy else 0uy)))

let r_trig_step_c m_state clk q_out =
  let old_m = m_state.(0ul) <> 0uy in
  let old_s : r_trig_state = { m = old_m } in
  let result = r_trig_step old_s clk in
  let new_s = fst result in
  let out = snd result in
  m_state.(0ul) <- (if new_s.m then 1uy else 0uy);
  q_out.(0ul) <- (if out.q then 1uy else 0uy)
