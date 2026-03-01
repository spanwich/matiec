(**
 * LowPLC.CTD — Verified CTD (down counter) combinator
 *
 * Source: analysis/lowplcst_spec.md §5.3
 * CD: count-down input (rising-edge triggered)
 * LD: load PV into CV (level-based, priority over CD)
 * PV: preset value to load
 * CV: current value, decremented on CD rising edge, stops at 0
 * Q: TRUE when CV <= 0
 *)
module LowPLC.CTD

open FStar.Int16

type ctd_state = {
  cv:      Int16.t;
  prev_cd: bool;
}

let ctd_init : ctd_state = { cv = 0s; prev_cd = false }

type ctd_outputs = {
  q:       bool;
  cv_out:  Int16.t;
}

let ctd_step (s: ctd_state) (cd: bool) (ld: bool) (pv: Int16.t)
    : (ctd_state & ctd_outputs) =
  let rising_cd = cd && not s.prev_cd in
  let new_cv =
    if ld then pv
    else if rising_cd then
      if s.cv `Int16.gt` 0s then Int16.sub s.cv 1s
      else s.cv
    else s.cv
  in
  let new_q = new_cv `Int16.lte` 0s in
  ({ cv = new_cv; prev_cd = cd }, { q = new_q; cv_out = new_cv })

(** Lemma: LD=TRUE loads PV into CV *)
let ctd_load_pv (s: ctd_state) (cd: bool) (pv: Int16.t)
    : Lemma ((fst (ctd_step s cd true pv)).cv = pv) = ()

(** Lemma: Q = TRUE iff CV <= 0 after step *)
let ctd_q_iff_cv_le_zero (s: ctd_state) (cd: bool) (ld: bool) (pv: Int16.t)
    : Lemma
      (let (new_s, out) = ctd_step s cd ld pv in
       out.q = (v new_s.cv <= 0)) = ()

(** Lemma: CV is bounded *)
let ctd_cv_bounded (s: ctd_state) (cd: bool) (ld: bool) (pv: Int16.t)
    : Lemma (let new_cv = (fst (ctd_step s cd ld pv)).cv in
             v new_cv >= -32768 /\ v new_cv <= 32767) = ()

(** Lemma: No rising edge and no load → CV unchanged *)
let ctd_no_edge_no_decrement (s: ctd_state) (pv: Int16.t)
    : Lemma (requires s.prev_cd = true)
            (ensures (fst (ctd_step s true false pv)).cv = s.cv) = ()
