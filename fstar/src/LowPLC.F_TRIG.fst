(**
 * LowPLC.F_TRIG — Verified falling edge detector combinator
 *
 * Source: analysis/lowplcst_spec.md §5.3
 * State: M = NOT (previous CLK)  (matiec stores inverted previous)
 * Q = NOT CLK AND NOT M  (TRUE for one scan on TRUE→FALSE transition)
 *
 * Equivalently: Q = (CLK=FALSE AND prev_clk=TRUE) when prev_clk = NOT M.
 *)
module LowPLC.F_TRIG

type f_trig_state = { m: bool }
let f_trig_init : f_trig_state = { m = false }
type f_trig_outputs = { q: bool }

let f_trig_step (s: f_trig_state) (clk: bool) : (f_trig_state & f_trig_outputs) =
  let falling = not clk && not s.m in
  let new_m = not clk in
  ({ m = new_m }, { q = falling })

(** Lemma: Q is TRUE iff CLK=FALSE and M=FALSE (prev CLK was TRUE) *)
let f_trig_output_iff_edge (s: f_trig_state) (clk: bool)
    : Lemma ((snd (f_trig_step s clk)).q = (not clk && not s.m)) = ()

(** Lemma: State after step records NOT CLK *)
let f_trig_state_update (s: f_trig_state) (clk: bool)
    : Lemma ((fst (f_trig_step s clk)).m = not clk) = ()

(** Lemma: Steady FALSE produces no edge *)
let f_trig_steady_false (s: f_trig_state)
    : Lemma (requires s.m = true)
            (ensures (snd (f_trig_step s false)).q = false) = ()
