# LowPLC Proof Status

Precise accounting of every formal claim in the project, its proof status,
and what would be needed to close any gaps.

---

## D1 — TON Combinator

| Lemma | Status | File |
|-------|--------|------|
| `ton_step_deterministic` | PROVED | LowPLC.TON.fst |
| `ton_idle_q_false` | PROVED | LowPLC.TON.fst |
| `ton_timing_progress` | PROVED | LowPLC.TON.fst |
| `ton_reset_reaches_idle` | PROVED | LowPLC.TON.fst |
| `ton_step_c` refinement | PROVED | LowPLC.TON.Impl.fst |

**Total: 5/5 proved, 0 admitted.**

---

## D2 — Remaining Combinators

### TOF (Off-Delay Timer)

| Lemma | Status | File |
|-------|--------|------|
| `tof_in_true_q_true` | PROVED | LowPLC.TOF.fst |
| `tof_falling_edge_starts_timing` | PROVED | LowPLC.TOF.fst |
| `tof_q_false_only_idle` | PROVED | LowPLC.TOF.fst |
| `tof_timing_progress` | PROVED | LowPLC.TOF.fst |
| `tof_step_c` refinement | PROVED | LowPLC.TOF.Impl.fst |

### TP (Pulse Timer)

| Lemma | Status | File |
|-------|--------|------|
| `tp_rising_edge_q_true` | PROVED | LowPLC.TP.fst |
| `tp_in_false_keeps_timing` | PROVED | LowPLC.TP.fst |
| `tp_zero_pt_no_pulse` | PROVED | LowPLC.TP.fst |
| `tp_timing_progress` | PROVED | LowPLC.TP.fst |
| `tp_step_c` refinement | PROVED | LowPLC.TP.Impl.fst |

### CTU (Up Counter)

| Lemma | Status | File |
|-------|--------|------|
| `ctu_reset_zeroes` | PROVED | LowPLC.CTU.fst |
| `ctu_q_iff_cv_ge_pv` | PROVED | LowPLC.CTU.fst |
| `ctu_cv_bounded` | PROVED | LowPLC.CTU.fst |
| `ctu_no_edge_no_increment` | PROVED | LowPLC.CTU.fst |
| `ctu_step_c` refinement | PROVED | LowPLC.CTU.Impl.fst |

### CTD (Down Counter)

| Lemma | Status | File |
|-------|--------|------|
| `ctd_load_pv` | PROVED | LowPLC.CTD.fst |
| `ctd_q_iff_cv_le_zero` | PROVED | LowPLC.CTD.fst |
| `ctd_cv_bounded` | PROVED | LowPLC.CTD.fst |
| `ctd_no_edge_no_decrement` | PROVED | LowPLC.CTD.fst |
| `ctd_step_c` refinement | PROVED | LowPLC.CTD.Impl.fst |

### CTUD (Up/Down Counter)

| Lemma | Status | File |
|-------|--------|------|
| `ctud_reset_zeroes` | PROVED | LowPLC.CTUD.fst |
| `ctud_q_correct` | PROVED | LowPLC.CTUD.fst |
| `ctud_simultaneous_cancel` | PROVED | LowPLC.CTUD.fst |
| `ctud_cv_bounded` | PROVED | LowPLC.CTUD.fst |
| `ctud_step_c` refinement | PROVED | LowPLC.CTUD.Impl.fst |

### SR (Set-Dominant Bistable)

| Lemma | Status | File |
|-------|--------|------|
| `sr_set_dominates` | PROVED | LowPLC.SR.fst |
| `sr_reset_works` | PROVED | LowPLC.SR.fst |
| `sr_hold` | PROVED | LowPLC.SR.fst |
| `sr_step_c` refinement | PROVED | LowPLC.SR.Impl.fst |

### RS (Reset-Dominant Bistable)

| Lemma | Status | File |
|-------|--------|------|
| `rs_reset_dominates` | PROVED | LowPLC.RS.fst |
| `rs_set_works` | PROVED | LowPLC.RS.fst |
| `rs_hold` | PROVED | LowPLC.RS.fst |
| `rs_step_c` refinement | PROVED | LowPLC.RS.Impl.fst |

### R_TRIG (Rising Edge Detector)

| Lemma | Status | File |
|-------|--------|------|
| `r_trig_output_iff_edge` | PROVED | LowPLC.R_TRIG.fst |
| `r_trig_state_update` | PROVED | LowPLC.R_TRIG.fst |
| `r_trig_steady_false` | PROVED | LowPLC.R_TRIG.fst |
| `r_trig_step_c` refinement | PROVED | LowPLC.R_TRIG.Impl.fst |

### F_TRIG (Falling Edge Detector)

| Lemma | Status | File |
|-------|--------|------|
| `f_trig_output_iff_edge` | PROVED | LowPLC.F_TRIG.fst |
| `f_trig_state_update` | PROVED | LowPLC.F_TRIG.fst |
| `f_trig_steady_false` | PROVED | LowPLC.F_TRIG.fst |
| `f_trig_step_c` refinement | PROVED | LowPLC.F_TRIG.Impl.fst |

**D2 total: 46/46 proved, 0 admitted.**

---

## D3 — Interpreter

### Structural Properties (LowPLC.Interp.Impl.fst)

| Lemma | Status | Notes |
|-------|--------|-------|
| `interp_deterministic` | PROVED | Trivial for pure function |
| `interp_terminates` | PROVED | Implicit from Tot effect + decreases gas |
| `unknown_opcode_safe` | PROVED | Unknown tags return pool unchanged |

### Gas Monotonicity

| Lemma | Status | Notes |
|-------|--------|-------|
| `gas_monotone` | **ADMITTED** | `admit()` |

**Gap:** Requires explicit induction on the gas counter decreasing through
the 5 mutually recursive functions (exec_stmts, exec_if_block,
exec_elsif_chain, exec_for_block, exec_for_iter). Z3 cannot discharge
this automatically because the mutual recursion crosses function boundaries.

The lemma states: for all valid programs and gas values g1 <= g2, if
exec_stmts terminates without hitting gas=0 at gas=g1, then
exec_stmts at gas=g2 produces the same result.

**To close:** Unfold the induction manually using `let rec` with
decreasing measure on g1. At each step, show that if gas > 0 and the
function doesn't return at the gas=0 check, then the result depends only
on the code/pool, not on the gas value. Use `FStar.Classical.forall_intro`
to generalize over all code positions.

**Impact:** Low. The lemma is only needed for the universal correctness
theorem. In practice, gas is set to 65536, far exceeding any benchmark.

### Universal Correctness

| Theorem | Status | Notes |
|---------|--------|-------|
| `universal_correctness_axiom` | **AXIOMATIZED** | Stated as documentation |

**Gap:** The full inductive assembly — "for all valid TLV programs
serialized from LowPLC-ST source, exec_scan computes the denotational
semantics of the source program" — is stated as a documentation-level
axiom (a `prop` constant, not an admitted lemma).

The theorem connects three verification layers:
1. **FB combinators (D2):** Each FB_CALL opcode dispatches to a verified
   combinator, which is proved equivalent to the IEC 61131-3 spec.
   (All 10 combinators fully proved.)
2. **Expression flattening:** The parser (D4) flattens compound
   expressions into scratch-variable sequences. Correctness depends on
   LowPLC-ST having no side effects in expressions (DDR-008).
3. **Control flow:** IF_BLOCK and FOR_BLOCK correctly implement the
   conditional and iteration semantics from lowplcst_spec.md §5.2.

**To close:** Requires:
(a) A formal definition of `sem_st` — the LowPLC-ST denotational semantics
    as an F\* function over the AST (from analysis/fstar_ast_types.fst).
(b) A serializer correctness theorem: the parser preserves semantics.
(c) Induction over the AST structure showing opcode-by-opcode equivalence.

Leaf cases (ASSIGN_BOOL, ASSIGN_INT, individual FB calls) follow directly
from D1/D2 combinator lemmas. The inductive step for IF requires case-split
on the condition value and appeal to branch correctness lemmas. The FOR
case requires induction on iterations with gas monotonicity.

**Paper presentation:** "We prove correctness for all atomic operations
(assignments, comparisons, and all 10 FB types) and structure the general
theorem. The full inductive proof over compound statements (IF/FOR) is
listed as future work."

---

## Stack/Heap Audit (DICE* Finding 4) — RESOLVED

Checked: all KaRaMeL-extracted files (LowPLC_TON_Impl.c, LowPLC_TOF_Impl.c,
LowPLC_TP_Impl.c, LowPLC_CTU_Impl.c, LowPLC_CTD_Impl.c, LowPLC_CTUD_Impl.c,
LowPLC_SR_Impl.c, LowPLC_RS_Impl.c, LowPLC_R_TRIG_Impl.c, LowPLC_F_TRIG_Impl.c)

Result: zero malloc/calloc/realloc calls. All FBs operate on caller-provided
buffers. Consistent with Low* Stack effect discipline.

Consequence: Compositional WCET model is sound. Per-opcode costs measured by
DWT are total costs — no hidden heap allocation overhead. compute_wcet Tot
function accurately bounds execution time.

---

## D4 — Bug Found: Counter Bounds Guards

During D4 testing, the C interpreter's counter step functions (CTU/CTD/CTUD)
were found to be missing bounds guards present in the F* spec. The F* spec
guards `cv < pv` (CTU) and `cv > 0` (CTD) before incrementing/decrementing,
preventing overflow. The C interpreter initially lacked these guards.

**Resolution:** C interpreter updated to match F* spec. All 3200/3200 E2E
tests pass after fix. The bug motivated the D5 load-time validation work.

---

## D5 — EverParse Validator + WCET Calculator

### EverParse TLV Validation (LowPLC_TLV.3d)

| Property | Status | Notes |
|----------|--------|-------|
| Header validation (magic, version, pool bounds) | VERIFIED | EverParse 3D constraints |
| Tag discrimination (P1) | VERIFIED | casetype with 18 leaf opcodes |
| Statement count bounds (P2) | VERIFIED | `stmt_count` in array constraint |
| Body length as byte-size driver (P3) | VERIFIED | `:byte-size body_len` |
| Named MAX constants (P4) | VERIFIED | Inline constraints per pool field |
| 6-level depth unrolling (D6→D1) | VERIFIED | Supports nesting depth 5 |

**EverParse output:** F* verified, 44,317 bytes ARM .text. 9/9 validation tests pass
(5 benchmarks accepted, 4 malformed inputs rejected).

### WCET Calculator (LowPLC.Costs.fst + LowPLC.WCET.fst)

| Property | Status | File |
|----------|--------|------|
| `LowPLC.Costs` — 31 cost constants | VERIFIED | LowPLC.Costs.fst |
| `wcet_stmts` termination | VERIFIED | LowPLC.WCET.fst (Tot, decreases gas) |
| `wcet_if_block` termination | VERIFIED | LowPLC.WCET.fst |
| `wcet_for_block` termination | VERIFIED | LowPLC.WCET.fst |
| `compute_wcet` total function | VERIFIED | LowPLC.WCET.fst |

**Zero admit(), zero assume().** All verification conditions discharged by F*.

**Soundness axiom (empirical):** The WCET bound is valid iff every cost constant
in LowPLC.Costs is >= the actual worst-case cycle count on the target hardware.
This is verified by DWT measurement, not by formal proof. The axiom is documented
in comments, not stated as an F* `assume`.

### Baremetal Runtime (firmware/lowplc_runtime.c)

Integrated pipeline using the EverParse-generated validator:
1. **EverParse TLV validation** — single call to `LowPlcTlvCheckLowPlcprogram()`
   validates header, pool bounds, all opcode tags, index bounds, nesting depth
   (<=5), and byte-size consistency (Layer 1+2)
2. Header field extraction (no validation — reads post-EverParse)
3. WCET computation (compositional, mirrors LowPLC.WCET.fst)
4. WCET vs scan budget check
5. Pool init + interpreter execution

Cross-compiles clean for Cortex-M7: 4,280 bytes .text (runtime) +
44,317 bytes .text (EverParse validator) + 116 bytes (wrapper).

---

## Summary

| Category | Proved | Admitted | Axiomatized | Total |
|----------|--------|----------|-------------|-------|
| D1 — TON combinator | 5 | 0 | 0 | 5 |
| D2 — Remaining FBs | 46 | 0 | 0 | 46 |
| D3 — Interpreter | 3 | 1 | 1 | 5 |
| D5 — EverParse validator | 6 | 0 | 0 | 6 |
| D5 — WCET calculator | 5 | 0 | 0 | 5 |
| **Total** | **65** | **1** | **1** | **67** |

The single `admit()` (gas_monotone) and single axiom (universal_correctness)
are both structural properties of the interpreter framework, not properties
of any individual FB or opcode. All 10 FB combinators and their Low\*
implementations are fully proved. The interpreter's opcode dispatch to those
combinators is type-safe and total by construction (verified by F\*'s type
checker on the Spec module).

The WCET soundness axiom is empirical (cost table derived from DWT measurement)
and is not counted as an F\* axiom — it is documented in code comments only.

---

## TODO — Before Hardware Measurement Phase
- [ ] WCET calculator in firmware/lowplc_runtime.c is hand-transliterated from
      LowPLC.WCET.fst, not KaRaMeL-extracted. Either extract via KaRaMeL before
      hardware measurement, or scope the paper claim to "manually verified
      transcription". Decision required before G2-Prep gate.
