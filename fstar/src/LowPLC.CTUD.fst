(**
 * LowPLC.CTUD — Verified CTUD (up/down counter) combinator
 *
 * Source: analysis/lowplcst_spec.md §5.3
 * Priority: R > LD > count. Simultaneous CU+CD edges cancel (no change).
 * QU = (CV >= PV), QD = (CV <= 0)
 *)
module LowPLC.CTUD

open FStar.Int16

type ctud_state = {
  cv:      Int16.t;
  prev_cu: bool;
  prev_cd: bool;
}

let ctud_init : ctud_state = { cv = 0s; prev_cu = false; prev_cd = false }

type ctud_outputs = {
  qu:     bool;
  qd:     bool;
  cv_out: Int16.t;
}

let ctud_step (s: ctud_state) (cu cd r ld: bool) (pv: Int16.t)
    : (ctud_state & ctud_outputs) =
  let rising_cu = cu && not s.prev_cu in
  let rising_cd = cd && not s.prev_cd in
  let new_cv =
    if r then 0s
    else if ld then pv
    else if rising_cu && rising_cd then s.cv  (* simultaneous: cancel *)
    else if rising_cu then
      if s.cv `Int16.lt` pv then Int16.add s.cv 1s else s.cv
    else if rising_cd then
      if s.cv `Int16.gt` 0s then Int16.sub s.cv 1s else s.cv
    else s.cv
  in
  let qu = new_cv `Int16.gte` pv in
  let qd = new_cv `Int16.lte` 0s in
  ({ cv = new_cv; prev_cu = cu; prev_cd = cd },
   { qu = qu; qd = qd; cv_out = new_cv })

(** Lemma: R=TRUE → CV=0 regardless of other inputs *)
let ctud_reset_zeroes (s: ctud_state) (cu cd ld: bool) (pv: Int16.t)
    : Lemma ((fst (ctud_step s cu cd true ld pv)).cv = 0s) = ()

(** Lemma: QU = (CV >= PV), QD = (CV <= 0) *)
let ctud_q_correct (s: ctud_state) (cu cd r ld: bool) (pv: Int16.t)
    : Lemma
      (let (new_s, out) = ctud_step s cu cd r ld pv in
       out.qu = (v new_s.cv >= v pv) /\
       out.qd = (v new_s.cv <= 0)) = ()

(** Lemma: Simultaneous rising edges → CV unchanged (when R=FALSE, LD=FALSE) *)
let ctud_simultaneous_cancel (s: ctud_state) (pv: Int16.t)
    : Lemma
      (requires not s.prev_cu /\ not s.prev_cd)
      (ensures (fst (ctud_step s true true false false pv)).cv = s.cv) = ()

(** Lemma: CV bounded in all transitions *)
let ctud_cv_bounded (s: ctud_state) (cu cd r ld: bool) (pv: Int16.t)
    : Lemma (let new_cv = (fst (ctud_step s cu cd r ld pv)).cv in
             v new_cv >= -32768 /\ v new_cv <= 32767) = ()
