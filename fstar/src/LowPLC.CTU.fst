(**
 * LowPLC.CTU — Verified CTU (up counter) combinator
 *
 * Source: analysis/lowplcst_spec.md §5.3
 * CU: count-up input (rising-edge triggered via internal R_TRIG)
 * R: reset to 0 (level-based, priority over CU)
 * PV: preset value
 * CV: current value, incremented on CU rising edge, stops at PV
 * Q: TRUE when CV >= PV
 *
 * DDR-013: Counter values saturate (not wrap) at Int16 bounds.
 *)
module LowPLC.CTU

open FStar.Int16

type ctu_state = {
  cv:      Int16.t;
  prev_cu: bool;
}

let ctu_init : ctu_state = { cv = 0s; prev_cu = false }

type ctu_outputs = {
  q:       bool;
  cv_out:  Int16.t;
}

let ctu_step (s: ctu_state) (cu: bool) (r: bool) (pv: Int16.t)
    : (ctu_state & ctu_outputs) =
  let rising_cu = cu && not s.prev_cu in
  let new_cv =
    if r then 0s
    else if rising_cu then
      if s.cv `Int16.lt` pv then Int16.add s.cv 1s
      else s.cv
    else s.cv
  in
  let new_q = new_cv `Int16.gte` pv in
  ({ cv = new_cv; prev_cu = cu }, { q = new_q; cv_out = new_cv })

(** Lemma: R=TRUE always resets CV to 0 *)
let ctu_reset_zeroes (s: ctu_state) (cu: bool) (pv: Int16.t)
    : Lemma ((fst (ctu_step s cu true pv)).cv = 0s) = ()

(** Lemma: Q = TRUE iff CV >= PV after step *)
let ctu_q_iff_cv_ge_pv (s: ctu_state) (cu: bool) (r: bool) (pv: Int16.t)
    : Lemma
      (let (new_s, out) = ctu_step s cu r pv in
       out.q = (v new_s.cv >= v pv)) = ()

(** Lemma: CV is bounded (no overflow beyond range) *)
let ctu_cv_bounded (s: ctu_state) (cu: bool) (r: bool) (pv: Int16.t)
    : Lemma (let new_cv = (fst (ctu_step s cu r pv)).cv in
             v new_cv >= -32768 /\ v new_cv <= 32767) = ()

(** Lemma: No rising edge → CV does not change (when R=FALSE) *)
let ctu_no_edge_no_increment (s: ctu_state) (pv: Int16.t)
    : Lemma (requires s.prev_cu = true)
            (ensures (fst (ctu_step s true false pv)).cv = s.cv) = ()
