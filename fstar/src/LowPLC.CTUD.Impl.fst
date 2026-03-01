module LowPLC.CTUD.Impl

open LowPLC.CTUD
open FStar.Int16
open FStar.UInt8
open FStar.HyperStack.ST
open LowStar.Buffer
open LowStar.BufferOps

module B = LowStar.Buffer
module U8 = FStar.UInt8
module I16 = FStar.Int16

val ctud_step_c:
    cv_state:      B.buffer I16.t ->
    prev_cu_state: B.buffer U8.t ->
    prev_cd_state: B.buffer U8.t ->
    cu:            bool ->
    cd:            bool ->
    r:             bool ->
    ld:            bool ->
    pv:            I16.t ->
    qu_out:        B.buffer U8.t ->
    qd_out:        B.buffer U8.t ->
    cv_out:        B.buffer I16.t ->
    Stack unit
      (requires fun h ->
        B.live h cv_state /\ B.live h prev_cu_state /\ B.live h prev_cd_state /\
        B.live h qu_out /\ B.live h qd_out /\ B.live h cv_out /\
        B.length cv_state = 1 /\ B.length prev_cu_state = 1 /\
        B.length prev_cd_state = 1 /\
        B.length qu_out = 1 /\ B.length qd_out = 1 /\ B.length cv_out = 1 /\
        B.disjoint cv_state prev_cu_state /\ B.disjoint cv_state prev_cd_state /\
        B.disjoint cv_state qu_out /\ B.disjoint cv_state qd_out /\
        B.disjoint cv_state cv_out /\
        B.disjoint prev_cu_state prev_cd_state /\
        B.disjoint prev_cu_state qu_out /\ B.disjoint prev_cu_state qd_out /\
        B.disjoint prev_cu_state cv_out /\
        B.disjoint prev_cd_state qu_out /\ B.disjoint prev_cd_state qd_out /\
        B.disjoint prev_cd_state cv_out /\
        B.disjoint qu_out qd_out /\ B.disjoint qu_out cv_out /\
        B.disjoint qd_out cv_out)
      (ensures fun h0 () h1 ->
        B.live h1 cv_state /\ B.live h1 prev_cu_state /\ B.live h1 prev_cd_state /\
        B.live h1 qu_out /\ B.live h1 qd_out /\ B.live h1 cv_out /\
        B.modifies (B.loc_union
          (B.loc_union (B.loc_buffer cv_state)
            (B.loc_union (B.loc_buffer prev_cu_state) (B.loc_buffer prev_cd_state)))
          (B.loc_union (B.loc_buffer qu_out)
            (B.loc_union (B.loc_buffer qd_out) (B.loc_buffer cv_out)))) h0 h1 /\
        (let old_cv = Seq.index (B.as_seq h0 cv_state) 0 in
         let old_pcu = (Seq.index (B.as_seq h0 prev_cu_state) 0) <> 0uy in
         let old_pcd = (Seq.index (B.as_seq h0 prev_cd_state) 0) <> 0uy in
         let old_s = { cv = old_cv; prev_cu = old_pcu; prev_cd = old_pcd } in
         let (new_s, out) = ctud_step old_s cu cd r ld pv in
         Seq.index (B.as_seq h1 cv_state) 0 = new_s.cv /\
         Seq.index (B.as_seq h1 prev_cu_state) 0 = (if new_s.prev_cu then 1uy else 0uy) /\
         Seq.index (B.as_seq h1 prev_cd_state) 0 = (if new_s.prev_cd then 1uy else 0uy) /\
         Seq.index (B.as_seq h1 qu_out) 0 = (if out.qu then 1uy else 0uy) /\
         Seq.index (B.as_seq h1 qd_out) 0 = (if out.qd then 1uy else 0uy) /\
         Seq.index (B.as_seq h1 cv_out) 0 = out.cv_out))

let ctud_step_c cv_state prev_cu_state prev_cd_state cu cd r ld pv qu_out qd_out cv_out =
  let old_cv = cv_state.(0ul) in
  let old_pcu = prev_cu_state.(0ul) <> 0uy in
  let old_pcd = prev_cd_state.(0ul) <> 0uy in
  let old_s : ctud_state = { cv = old_cv; prev_cu = old_pcu; prev_cd = old_pcd } in
  let result = ctud_step old_s cu cd r ld pv in
  let new_s = fst result in
  let out = snd result in
  cv_state.(0ul) <- new_s.cv;
  prev_cu_state.(0ul) <- (if new_s.prev_cu then 1uy else 0uy);
  prev_cd_state.(0ul) <- (if new_s.prev_cd then 1uy else 0uy);
  qu_out.(0ul) <- (if out.qu then 1uy else 0uy);
  qd_out.(0ul) <- (if out.qd then 1uy else 0uy);
  cv_out.(0ul) <- out.cv_out
