(**
 * LowPLC.RS — Verified RS (reset-dominant bistable) combinator
 *
 * Source: analysis/lowplcst_spec.md §5.3
 * Q1 = NOT R1 AND (S OR Q1_prev)
 * R1 dominates: if S=TRUE and R1=TRUE, Q1=FALSE.
 *)
module LowPLC.RS

type rs_state = { q: bool }
let rs_init : rs_state = { q = false }
type rs_outputs = { q1: bool }

let rs_step (s: rs_state) (s_val: bool) (r1: bool) : (rs_state & rs_outputs) =
  let new_q = not r1 && (s_val || s.q) in
  ({ q = new_q }, { q1 = new_q })

(** Lemma: R1=TRUE always produces Q1=FALSE regardless of S and previous state *)
let rs_reset_dominates (s: rs_state) (s_val: bool)
    : Lemma ((snd (rs_step s s_val true)).q1 = false) = ()

(** Lemma: S=TRUE and R1=FALSE always produces Q1=TRUE *)
let rs_set_works (s: rs_state)
    : Lemma ((snd (rs_step s true false)).q1 = true) = ()

(** Lemma: S=FALSE and R1=FALSE holds state *)
let rs_hold (s: rs_state)
    : Lemma ((fst (rs_step s false false)).q = s.q) = ()
