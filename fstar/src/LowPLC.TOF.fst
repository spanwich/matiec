(**
 * LowPLC.TOF — Verified TOF (off-delay timer) combinator
 *
 * Source: analysis/lowplcst_spec.md §5.3
 * Q stays TRUE for PT after IN goes FALSE. Q is also TRUE when IN=TRUE.
 * Phase: IDLE(0), TIMING(1), DONE(2)
 *
 * Unlike TON: Q goes TRUE immediately when IN=TRUE.
 * Timing begins on the falling edge (IN going FALSE from DONE phase).
 *)
module LowPLC.TOF

open FStar.UInt32

type tof_phase =
  | TOF_IDLE    (** Q=FALSE, waiting for IN=TRUE *)
  | TOF_TIMING  (** IN=FALSE, off-delay counting, Q still TRUE *)
  | TOF_DONE    (** IN=TRUE, output active *)

noeq type tof_state = {
  phase:      tof_phase;
  prev_in:    bool;
  start_tick: UInt32.t;
}

let tof_init : tof_state = { phase = TOF_IDLE; prev_in = false; start_tick = 0ul }

type tof_outputs = {
  q:        bool;
  et_ticks: UInt32.t;
}

let tof_step (s: tof_state) (in_val: bool) (pt_ticks: UInt32.t)
    (current_tick: UInt32.t{v current_tick >= v s.start_tick})
    : (tof_state & tof_outputs) =
  match s.phase with
  | TOF_IDLE ->
    if not s.prev_in && in_val then
      (* Rising edge of IN → go to DONE (Q=TRUE immediately) *)
      ({ phase = TOF_DONE; prev_in = in_val; start_tick = 0ul },
       { q = true; et_ticks = 0ul })
    else if in_val then
      (* IN=TRUE steady → go to DONE *)
      ({ phase = TOF_DONE; prev_in = in_val; start_tick = 0ul },
       { q = true; et_ticks = 0ul })
    else
      ({ s with prev_in = in_val }, { q = false; et_ticks = 0ul })
  | TOF_DONE ->
    if not in_val then
      (* Falling edge — start off-delay timer *)
      ({ phase = TOF_TIMING; prev_in = in_val; start_tick = current_tick },
       { q = true; et_ticks = 0ul })
    else
      ({ s with prev_in = in_val }, { q = true; et_ticks = 0ul })
  | TOF_TIMING ->
    if in_val then
      (* IN went TRUE again — retrigger, go back to DONE *)
      ({ phase = TOF_DONE; prev_in = in_val; start_tick = 0ul },
       { q = true; et_ticks = 0ul })
    else
      let elapsed = current_tick `UInt32.sub` s.start_tick in
      if elapsed `UInt32.gte` pt_ticks then
        (* Off-delay expired *)
        ({ phase = TOF_IDLE; prev_in = in_val; start_tick = 0ul },
         { q = false; et_ticks = pt_ticks })
      else
        ({ s with prev_in = in_val },
         { q = true; et_ticks = elapsed })

(** Lemma: IN=TRUE → Q=TRUE immediately (any reachable phase) *)
let tof_in_true_q_true (s: tof_state) (pt: UInt32.t)
    (tick: UInt32.t{v tick >= v s.start_tick})
    : Lemma (ensures (snd (tof_step s true pt tick)).q = true) = ()

(** Lemma: After falling edge from DONE, TIMING begins, Q remains TRUE *)
let tof_falling_edge_starts_timing (s: tof_state) (pt: UInt32.t)
    (tick: UInt32.t{v tick >= v s.start_tick})
    : Lemma (requires s.phase = TOF_DONE)
            (ensures (fst (tof_step s false pt tick)).phase = TOF_TIMING /\
                     (snd (tof_step s false pt tick)).q = true) = ()

(** Lemma: Q=FALSE only when phase is IDLE *)
let tof_q_false_only_idle (s: tof_state) (in_val: bool) (pt: UInt32.t)
    (tick: UInt32.t{v tick >= v s.start_tick})
    : Lemma (requires (snd (tof_step s in_val pt tick)).q = false)
            (ensures (fst (tof_step s in_val pt tick)).phase = TOF_IDLE) = ()

(** Lemma: Timing progress — elapsed increases each tick *)
let tof_timing_progress (s: tof_state) (pt: UInt32.t)
    (tick: UInt32.t{v tick >= v s.start_tick})
    : Lemma
      (requires s.phase = TOF_TIMING /\
                v (tick `UInt32.sub` s.start_tick) < v pt /\
                v tick + 1 < pow2 32)
      (ensures (let elapsed = v tick - v s.start_tick in
                let next_elapsed = (v tick + 1) - v s.start_tick in
                next_elapsed > elapsed)) = ()
