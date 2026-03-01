(**
 * LowPLC.R_TRIG — Verified rising edge detector combinator
 *
 * Source: analysis/lowplcst_spec.md §5.3
 * State: M = previous CLK value
 * Q = CLK AND NOT M  (TRUE for exactly one scan on FALSE→TRUE transition)
 *)
module LowPLC.R_TRIG

type r_trig_state = { m: bool }
let r_trig_init : r_trig_state = { m = false }
type r_trig_outputs = { q: bool }

let r_trig_step (s: r_trig_state) (clk: bool) : (r_trig_state & r_trig_outputs) =
  let rising = clk && not s.m in
  ({ m = clk }, { q = rising })

(** Lemma: Q is TRUE iff current CLK=TRUE and previous M=FALSE *)
let r_trig_output_iff_edge (s: r_trig_state) (clk: bool)
    : Lemma ((snd (r_trig_step s clk)).q = (clk && not s.m)) = ()

(** Lemma: State after step records current CLK *)
let r_trig_state_update (s: r_trig_state) (clk: bool)
    : Lemma ((fst (r_trig_step s clk)).m = clk) = ()

(** Lemma: Steady TRUE produces no edge *)
let r_trig_steady_false (s: r_trig_state)
    : Lemma (requires s.m = true)
            (ensures (snd (r_trig_step s true)).q = false) = ()
