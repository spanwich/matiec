(**
 * LowPLC.TP — Verified TP (pulse timer) combinator
 *
 * Source: analysis/lowplcst_spec.md §5.3
 * Q goes TRUE immediately on rising edge of IN, stays TRUE for PT duration.
 * IN going FALSE does NOT stop the timer (unlike TON).
 * After pulse, waits for IN=FALSE before retriggering.
 *
 * prev_in is REQUIRED here (unlike TON) because TP stays in TIMING
 * regardless of IN — phase alone cannot encode the edge history.
 *)
module LowPLC.TP

open FStar.UInt32

type tp_phase =
  | TP_IDLE    (** Waiting for rising edge *)
  | TP_TIMING  (** Pulse active, Q=TRUE *)
  | TP_DONE    (** Pulse finished, waiting for IN=FALSE to rearm *)

noeq type tp_state = {
  phase:      tp_phase;
  prev_in:    bool;
  start_tick: UInt32.t;
}

let tp_init : tp_state = { phase = TP_IDLE; prev_in = false; start_tick = 0ul }

type tp_outputs = {
  q:        bool;
  et_ticks: UInt32.t;
}

let tp_step (s: tp_state) (in_val: bool) (pt_ticks: UInt32.t)
    (current_tick: UInt32.t{v current_tick >= v s.start_tick})
    : (tp_state & tp_outputs) =
  let rising_edge = in_val && not s.prev_in in
  match s.phase with
  | TP_IDLE ->
    if rising_edge && pt_ticks `UInt32.gt` 0ul then
      (* Start pulse *)
      ({ phase = TP_TIMING; prev_in = in_val; start_tick = current_tick },
       { q = true; et_ticks = 0ul })
    else
      ({ s with prev_in = in_val }, { q = false; et_ticks = 0ul })
  | TP_TIMING ->
    let elapsed = current_tick `UInt32.sub` s.start_tick in
    if elapsed `UInt32.gte` pt_ticks then
      (* Pulse done — go to DONE, wait for IN=FALSE *)
      if not in_val then
        (* IN already FALSE → go straight to IDLE for immediate rearm *)
        ({ phase = TP_IDLE; prev_in = in_val; start_tick = 0ul },
         { q = false; et_ticks = pt_ticks })
      else
        ({ phase = TP_DONE; prev_in = in_val; start_tick = 0ul },
         { q = false; et_ticks = pt_ticks })
    else
      (* Still pulsing — IN state doesn't matter *)
      ({ s with prev_in = in_val }, { q = true; et_ticks = elapsed })
  | TP_DONE ->
    (* Waiting for IN to go FALSE before rearming *)
    if not in_val then
      ({ phase = TP_IDLE; prev_in = in_val; start_tick = 0ul },
       { q = false; et_ticks = 0ul })
    else
      ({ s with prev_in = in_val }, { q = false; et_ticks = 0ul })

(** Lemma: Rising edge with PT>0 → Q=TRUE immediately *)
let tp_rising_edge_q_true (s: tp_state) (pt: UInt32.t)
    (tick: UInt32.t{v tick >= v s.start_tick})
    : Lemma (requires s.phase = TP_IDLE /\ not s.prev_in /\ v pt > 0)
            (ensures (snd (tp_step s true pt tick)).q = true) = ()

(** Lemma: IN going FALSE during TIMING does NOT change phase *)
let tp_in_false_keeps_timing (s: tp_state) (pt: UInt32.t)
    (tick: UInt32.t{v tick >= v s.start_tick})
    : Lemma (requires s.phase = TP_TIMING /\
                      v (tick `UInt32.sub` s.start_tick) < v pt)
            (ensures (fst (tp_step s false pt tick)).phase = TP_TIMING) = ()

(** Lemma: PT=0 → Q never TRUE from IDLE *)
let tp_zero_pt_no_pulse (s: tp_state) (in_val: bool)
    (tick: UInt32.t{v tick >= v s.start_tick})
    : Lemma (requires s.phase = TP_IDLE)
            (ensures (snd (tp_step s in_val 0ul tick)).q = false) = ()

(** Lemma: Timing progress *)
let tp_timing_progress (s: tp_state) (pt: UInt32.t)
    (tick: UInt32.t{v tick >= v s.start_tick})
    : Lemma
      (requires s.phase = TP_TIMING /\
                v (tick `UInt32.sub` s.start_tick) < v pt /\
                v tick + 1 < pow2 32)
      (ensures (let elapsed = v tick - v s.start_tick in
                let next_elapsed = (v tick + 1) - v s.start_tick in
                next_elapsed > elapsed)) = ()
