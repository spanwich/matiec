(**
 * LowPLC.SR — Verified SR (set-dominant bistable) combinator
 *
 * Source: analysis/lowplcst_spec.md §5.3
 * Q1 = S1 OR (NOT R AND Q1_prev)
 * S1 dominates: if S1=TRUE and R=TRUE, Q1=TRUE.
 *)
module LowPLC.SR

type sr_state = { q: bool }
let sr_init : sr_state = { q = false }
type sr_outputs = { q1: bool }

let sr_step (s: sr_state) (s1: bool) (r: bool) : (sr_state & sr_outputs) =
  let new_q = s1 || (not r && s.q) in
  ({ q = new_q }, { q1 = new_q })

(** Lemma: S1=TRUE always produces Q1=TRUE regardless of R and previous state *)
let sr_set_dominates (s: sr_state) (r: bool)
    : Lemma ((snd (sr_step s true r)).q1 = true) = ()

(** Lemma: R=TRUE and S1=FALSE always produces Q1=FALSE *)
let sr_reset_works (s: sr_state)
    : Lemma ((snd (sr_step s false true)).q1 = false) = ()

(** Lemma: S1=FALSE and R=FALSE holds state *)
let sr_hold (s: sr_state)
    : Lemma ((fst (sr_step s false false)).q = s.q) = ()
