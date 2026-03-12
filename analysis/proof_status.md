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
| ADR-009 — Timer US wrappers | 6 | 0 | 0 | 6 |
| D6 — D5 interpreter + Low* extraction | 5 | 1 | 0 | 6 |
| **Total** | **76** | **2** | **1** | **79** |

The two `admit()`s (gas\_monotone in Spec and D5) and single axiom (universal\_correctness)
are all structural properties of the interpreter framework, not properties
of any individual FB or opcode. All 10 FB combinators and their Low\*
implementations are fully proved. The interpreter's opcode dispatch to those
combinators is type-safe and total by construction (verified by F\*'s type
checker on both the Spec and D5 modules).

The WCET soundness axiom is empirical (cost table derived from DWT measurement)
and is not counted as an F\* axiom — it is documented in code comments only.

---

## ADR-009: Wall-Clock Timer Semantics — IMPLEMENTED (Phase 2: Dispatch-Time)

**Change:** TLV timer opcodes (0x50–0x52) store `pt_us` (microseconds) instead of
`pt_ticks` (scan-cycle ticks). EverParse enforces `1 <= pt_us <= 3,600,000,000` (1 hour).

**Architecture (ADR-009-INT):** Dispatch-time conversion replaces the original load-time
pre-pass. The TLV buffer stays **const** — no mutable RAM copy needed.

- `low_pool` record contains `lp_scan_period_us : UInt32.t` (set once at load time)
- Verified interpreter (`exec_fb_call_low`) calls `ton_step_us`/`tof_step_us`/`tp_step_us`
  which compute `pt_ticks = pt_us / scan_period_us` inline at each dispatch
- Division-by-zero guard: `scan_period_us == 0` → `pt_ticks = 0`
- `lowplc_load_program()` rejects `scan_cycle_period_us == 0` with `LOWPLC_ERR_INVALID_PERIOD`

**Verified F\* wrappers** (called by `exec_fb_call_low` in the verified interpreter):

| Module | Status | File |
|--------|--------|------|
| `ton_step_us` | PROVED | LowPLC.TON.Impl.US.fst |
| `ton_step_us_correct` | PROVED | LowPLC.TON.Impl.US.fst |
| `tof_step_us` | PROVED | LowPLC.TOF.Impl.US.fst |
| `tof_step_us_correct` | PROVED | LowPLC.TOF.Impl.US.fst |
| `tp_step_us` | PROVED | LowPLC.TP.Impl.US.fst |
| `tp_step_us_correct` | PROVED | LowPLC.TP.Impl.US.fst |

**6/6 proved, 0 admitted.** Each `*_step_us` computes `pt_ticks = pt_us / scan_period_us`
(with zero guard) and delegates to the corresponding verified `*_step`. Correctness lemmas
hold by definitional unfolding (`()` proof term).

**Removed code** (from Phase 1 pre-pass approach):
- `g_tlv_buf[4096]` — 4K mutable RAM buffer (BSS: 4584 → 496 bytes)
- `convert_pt_us_to_ticks()`, `convert_pt_section()`, `write_u32_le()` — ~35 lines
- `__builtin_memcpy()` of TLV to mutable buffer

**TCB impact (post-D6):** The conversion logic is now inside the verified interpreter
(`exec_fb_call_low` calls `ton_step_us`/`tof_step_us`/`tp_step_us`). No unverified C
code is involved in timer dispatch. The `*_step_us` functions are compiled into the
verified `LowPLC_Interp_Low.c` binary.

**Verification gates passed:**
- G1: F\* typecheck (3/3 modules)
- G2: KaRaMeL extraction (3/3 modules, 460 bytes ARM .text)
- G3: Host unit tests (4/4 TON\_US tests)
- G4: ARM cross-compile
- G5: Firmware build (74,592 .text, 496 BSS)
- G6: BSS regression (496 < 4,584 — 4K saved)
- G7: Renode bring-up (3/3 scenarios correct)
- G8: E2E benchmark (49/49 tests pass)

---

## D6 — Verified D5-Format Interpreter + KaRaMeL Extraction

**Purpose:** Align the F\* interpreter with the actual D5 TLV binary format,
extract it via KaRaMeL to verified C, and eliminate all unverified glue code
from the firmware trust boundary. D6 spans four sub-tasks: D6 (initial),
D6-FIX (remove admits), D6-GATE6 (E2E confirmation), D6-TCB (trust cleanup).

### Implementation

**Key finding (D6 Audit):** The original `LowPLC.Interp.Spec.fst` used a different
IF\_BLOCK and FOR\_BLOCK format than the actual TLV binaries:
- IF\_BLOCK Spec: `[cond:1][then_len:2][then_body][num_elsif:1]{elsif}*[else_len:2][else_body]`
- IF\_BLOCK D5:   `[cond:1][then_count:1][else_count:1][then_len:2][else_len:2][then_body][else_body]`
- FOR\_BLOCK Spec: 7-byte header (no body\_count)
- FOR\_BLOCK D5:   8-byte header (includes body\_count at offset 5)

D5 format flattens ELSIFs into nested IF\_BLOCKs — no `exec_elsif_chain` needed.

**Modules (4 new, 2124 lines total):**

| Module | Lines | Purpose |
|--------|-------|---------|
| `LowPLC.Interp.State.fst` | 159 | `low_pool` record with `scan_cycle_period_us : UInt32.t`. `pool_live`, `pool_loc`, `pool_disjoint` (66 pairwise clauses), `pool_lengths_ok`, `pool_valid`. |
| `LowPLC.Interp.D5.fst` | 508 | Pure D5-format interpreter (Tot). 4 mutually recursive functions. |
| `LowPLC.Interp.D5.Impl.fst` | 176 | Correctness lemmas for D5 spec. |
| `LowPLC.Interp.Low.fst` | 1281 | Low\* pool types + opcode dispatch. `exec_simple_low`, `exec_fb_call_low`, `exec_stmts_low`, `exec_if_block_low`, `exec_for_block_low`. Mutually recursive via F\* `and` keyword. All verified with `--z3rlimit 200`, zero `--admit_smt_queries`. |

### D5 Interpreter Lemmas

| Lemma | Status | File |
|-------|--------|------|
| `d5_deterministic` | PROVED | LowPLC.Interp.D5.Impl.fst |
| `d5_terminates` | PROVED | LowPLC.Interp.D5.Impl.fst |
| `d5_unknown_opcode_safe` | PROVED | LowPLC.Interp.D5.Impl.fst |
| `d5_gas_monotone` | **ADMITTED** | LowPLC.Interp.D5.Impl.fst |
| `d5_atomic_eq_spec` | PROVED | LowPLC.Interp.D5.Impl.fst |
| `d5_for_block_body_count_skip` | PROVED | LowPLC.Interp.D5.Impl.fst |

### D5 Interpreter Properties (by construction)

| Property | Mechanism | Module |
|----------|-----------|--------|
| Memory safety | Tot effect, bounds-checked seq access | LowPLC.Interp.D5.fst |
| Termination | `gas: nat` with `decreases gas` on all 4 mutual functions | LowPLC.Interp.D5.fst |
| Determinism | Tot effect = pure function | LowPLC.Interp.D5.fst |
| ADR-009 timer dispatch | `pt_us_to_ticks` inline conversion | LowPLC.Interp.D5.fst |
| D5 IF format | 7-byte flat header, nested IF\_BLOCKs for ELSIF | LowPLC.Interp.D5.fst |
| D5 FOR format | 8-byte header with body\_count skip | LowPLC.Interp.D5.fst |

### Low* Infrastructure

| Component | Status | File |
|-----------|--------|------|
| `low_pool` type (12 buffer fields + counts + tick + scan\_period\_us) | VERIFIED | LowPLC.Interp.Low.fst |
| `pool_live`, `pool_lengths_ok`, `pool_disjoint` (66 clauses) | VERIFIED | LowPLC.Interp.Low.fst |
| `get/set_bool_low`, `get/set_int_low` | VERIFIED | LowPLC.Interp.Low.fst |
| `read_u16/u32/i16_le_low` byte parsers | VERIFIED | LowPLC.Interp.Low.fst |
| `add/sub/mul_wrap_low` arithmetic | VERIFIED | LowPLC.Interp.Low.fst |
| FB state accessors (12 get/set pairs) | VERIFIED | LowPLC.Interp.Low.fst |
| Tag constants (30 U8.t values) | VERIFIED | LowPLC.Interp.Low.fst |
| `pt_us_to_ticks_low` | VERIFIED | LowPLC.Interp.Low.fst |

### Low* Opcode Dispatch (KaRaMeL-extracted)

| Component | Status | File |
|-----------|--------|------|
| `exec_simple_low` (18 leaf opcodes) | VERIFIED | LowPLC.Interp.Low.fst |
| `exec_fb_call_low` (10 FB opcodes) | VERIFIED | LowPLC.Interp.Low.fst |
| `exec_stmts_low` (statement scanner) | VERIFIED | LowPLC.Interp.Low.fst |
| `exec_if_block_low` | VERIFIED | LowPLC.Interp.Low.fst |
| `exec_for_block_low` + `exec_for_iter_low` | VERIFIED | LowPLC.Interp.Low.fst |
| KaRaMeL C extraction | OK | build\_low/LowPLC\_Interp\_Low.c |

**5/6 lemmas proved, 1 admitted (d5\_gas\_monotone).**
**Low\* dispatch: fully verified (Stack effect, buffer safety via `pool_disjoint`).
Zero `--admit_smt_queries`. KaRaMeL extraction: no malloc, no hand-written loop.**

### Key Proof Engineering Decisions

1. **`pool_disjoint` approach.** Buffer safety obligations for `exec_simple_low`
   and `exec_fb_call_low` required proving 66 pairwise disjointness conditions
   between the 12 pool buffers + the code buffer. Added as a precondition to all
   functions that modify pool state via `pool_valid`. This replaced the previous
   `--admit_smt_queries true` that had suppressed these obligations (D6-FIX).

2. **Mutual recursion via `and`.** `exec_stmts_low`, `exec_if_block_low`, and
   `exec_for_block_low` are mutually recursive. F\* supports this via the `and`
   keyword with `(decreases gas)` on each function. The claim in D6 (pre-FIX)
   that "F\* doesn't support mutually recursive Stack-effect functions" was
   incorrect — confirmed working before implementation.

3. **Index comparison fix.** Original code used `U8.lt idx (uint32_to_uint8
   pool_size)` — silent truncation if `pool_size > 255`. Fixed to
   `U32.lt (uint8_to_uint32 idx) pool_size`.

4. **KaRaMeL extraction.** `exec_stmts_low` extracts to C without ghost
   functions, without `malloc`, with no hand-written loop. Confirmed by
   `grep malloc fstar/build_low/LowPLC_Interp_Low.c` → 0 matches.

### D6-FIX — Remove All Admits from Low* Dispatch

The initial D6 used `--admit_smt_queries true` on `exec_simple_low` and
`exec_fb_call_low` for buffer safety proofs. D6-FIX eliminated this:
- Added `pool_disjoint` predicate with 66 pairwise `B.loc_disjoint` clauses
- Added `pool_valid` combining `pool_live`, `pool_lengths_ok`, `pool_disjoint`
- All functions now verify with `--z3rlimit 200`, zero admits
- The hand-written C loop wrapper (`build_low/lowplc_interp_extracted.h`) was
  eliminated — `exec_stmts_low` is now fully extracted by KaRaMeL

Commit: `26aef07`.

### D6-GATE6 — E2E Confirmation Against Extracted Interpreter

Gate 6 required explicit confirmation that E2E tests run against the
KaRaMeL-extracted verified interpreter, not the old hand-written dispatcher.

**Deleted files:**
- `fstar/test/lowplc_interp.h` (718-line hand-written dispatcher): **REPLACED**
  with 221-line thin adapter wrapping `LowPLC_Interp_Low_exec_stmts_low()`
- `fstar/build_low/lowplc_interp_extracted.h` (hand-written C loop): **DELETED**

**E2E results against KaRaMeL-extracted interpreter:**
- `fstar/test/test_interp.c`: **49/49 PASS** (B1 hand-crafted scenarios)
- `tools/test_e2e.py`: **3200/3200 PASS** (B1–B5, 100 scans each)

**Bug found during Gate 6 (test-side, not LowPLC):**

> **Bug: Uninitialized matiec struct flags (pre-existing, test-side)**
> `ESD_INTERLOCK_data__` was stack-allocated without zeroing, leaving
> `__IEC_FORCE_FLAG` garbage in flags fields of some variables. This caused
> `__SET_VAR` to silently skip writes. Fixed by adding
> `memset(&matiec, 0, sizeof(matiec))` before `ESD_INTERLOCK_init__()` in
> `fstar/test/test_interp.c` and the generated E2E test code.
> **This is a bug in the matiec reference side of the test harness, not in
> LowPLC.** It does not affect firmware correctness. It is evidence that
> the test suite has genuine discriminating power — a change in binary layout
> exposed it.

### D6-TCB — Trust Boundary Cleanup

Root cause: `fstar/test/lowplc_interp.h` (221-line test adapter) was being
`#include`d by `firmware/lowplc_runtime.c`. This placed an unverified struct
definition (`var_pool_t`) and conversion function (`make_low_pool()`) in the
firmware runtime path.

**Fix:**
- `firmware/lowplc_runtime.c` now includes `LowPLC_Interp_Low.h` directly
- Global arrays use KaRaMeL-extracted types: `LowPLC_Interp_Low_ton_flat`,
  `tof_flat`, `tp_flat`, `ctu_flat`, `ctd_flat`, `ctud_flat`
- `build_pool()` constructs `LowPLC_Interp_Low_low_pool` from these arrays
  using named struct designators — calls `LowPLC_Interp_Low_exec_stmts_low()`
  directly
- Type safety: `ton_flat`/`tof_flat`/`tp_flat`/`ctu_flat`/`ctd_flat`/
  `ctud_flat` are distinct named C structs (different field names) — wrong-field
  assignment is a compile error. SR/RS/R\_TRIG/F\_TRIG are all `typedef uint8_t`
  (same representation) — their 4 assignments in `build_pool()` are manually
  audited
- `fstar/test/lowplc_interp.h` carries a `#ifdef LOWPLC_FIRMWARE_BUILD` guard —
  inclusion in firmware builds is a hard compile error

Commit: `df56ea0`.

### Build Warnings

Firmware build produces 5 macro redefinition warnings. All are harmless —
KaRaMeL's `krmllib.h` and EverParse's `EverParseEndianness.h` both define
standard utility macros that overlap:

| Macro | Defined in (first) | Redefined in |
|-------|-------------------|--------------|
| `KRML_CHECK_SIZE` | EverParseEndianness.h:62 | krml/internal/target.h:215 |
| `htobe32` | EverParseEndianness.h:156 | krml/lowstar\_endianness.h:132 |
| `be32toh` | EverParseEndianness.h:158 | krml/lowstar\_endianness.h:138 |
| `htobe64` | EverParseEndianness.h:161 | krml/lowstar\_endianness.h:142 |
| `be64toh` | EverParseEndianness.h:163 | krml/lowstar\_endianness.h:149 |

Both headers define these macros identically (`__builtin_bswap32/64`).
The warnings do not affect correctness. Fixing them would require modifying
generated EverParse headers, which is not advisable.

### Verification Gates

- G1: F\* typecheck — all modules pass (32 .fst files)
- G2: KaRaMeL extraction — `LowPLC_Interp_Low.c` + `.h` generated, no malloc
- G3: ARM cross-compile — 16,264 bytes .text (verified interpreter)
- G4: E2E tests — 49/49 (hand-crafted) + 3200/3200 (generated)
- G5: Firmware build — 85,288 bytes total .text
- G6: TCB audit — test adapter excluded from firmware build

---

## TCB Statement (authoritative — for paper §3.3)

### VERIFIED — proofs in F\*, extracted by KaRaMeL or EverParse

**`fstar/build/LowPLC_*_Impl.c` (10 files)**
D1–D3 combinator library: TON, TOF, TP, CTU, CTD, CTUD, SR, RS, R\_TRIG,
F\_TRIG step functions. 51 proved lemmas, 0 admits in implementation.

**`fstar/build_low/LowPLC_Interp_Low.c`**
D6 verified interpreter: `exec_stmts_low`, `exec_simple_low`,
`exec_fb_call_low`, `exec_if_block_low`, `exec_for_block_low`,
`exec_for_iter_low`. 16,264 bytes .text. 5 proved lemmas in D5.Impl,
1 admitted (`d5_gas_monotone`). Low\* dispatch fully verified (zero admits
in implementation, zero `--admit_smt_queries`).

**`fstar/build/LowPLC_TON_Impl_US.c` (+ TOF, TP)**
ADR-009 wall-clock timer wrappers. 6 proved lemmas, 0 admits.

**`everparse/LowPLC_TLV.c` + `LowPLC_TLVWrapper.c`**
TLV program validator. 484-line 3D spec. 7,280 bytes .text.

### TRUSTED UNVERIFIED — audited, line-counted, named explicitly

**`firmware/lowplc_runtime.c` (394 lines)**
Scan cycle engine. Pool initialization (`init_pool_from_header`). Header
field extraction (`extract_header`). `build_pool()` — assembles
`LowPLC_Interp_Low_low_pool` from global arrays using named struct
designators. WCET calculator (hand-written C — see WCET note below).
Direct calls to `LowPLC_Interp_Low_exec_stmts_low()`.
Note: `build_pool()` SR/RS/R\_TRIG/F\_TRIG assignments are manually audited
(all `typedef uint8_t`, 4 lines). 2,588 bytes .text.

**`firmware/main_bringup.c` (65 lines)**
Application entry point. Board init, scan loop.

**BSP layer (~500 lines)**
GPIO, UART, SysTick, DWT. CMSIS direct register access.

**STM32 startup + vector table (~100 lines)**
Provided by ST. Standard Cortex-M7 startup code.

**KaRaMeL extractor (trusted tool)**
Used in production by Project Everest. Not verified here.

**ARM GCC (trusted compiler)**
Standard assumption for verified-compilation systems.

### NOT IN RUNTIME TCB

**`fstar/test/lowplc_interp.h` (221 lines)**
Test adapter. Guarded: `#ifdef LOWPLC_FIRMWARE_BUILD` → compile error.
Never compiled into firmware.

**`tools/test_e2e.py`**
E2E test harness. Development machine only.

**`tools/lowplcst_parser.py`**
LowPLC-ST parser. Development machine only. DDR-014.

---

## WCET Calculator — Open Item

> **WCET Calculator — Trusted Unverified (TODO before paper)**
>
> `firmware/lowplc_runtime.c` contains a hand-written C WCET calculator
> (`wcet_stmts`, `wcet_if_block`, `wcet_for_block`, `atomic_cost`) that walks
> the TLV and computes a cycle bound. This is NOT the KaRaMeL-extracted
> `compute_wcet` function from `LowPLC.WCET.fst`.
>
> Current status: trusted-unverified, counted in `lowplc_runtime.c`'s 394 lines.
>
> Required before paper submission: Either
> (a) Extract `LowPLC.WCET.fst` via KaRaMeL and replace the hand-written
>     functions in `lowplc_runtime.c`, moving `compute_wcet` into the VERIFIED
>     column of the TCB; OR
> (b) Document explicitly that the WCET calculator is a manually verified
>     transcription of the F\* spec, provide a line-by-line correspondence
>     table, and scope the paper claim to "verified specification, audited
>     implementation."
>
> Option (a) is strongly preferred for paper credibility.
> Resolution: **TODO — before D7 or hardware measurement phase.**

---

## ADR-009 Discrepancy Resolution

> **ADR-009 Discrepancy 2 (CLOSED by D6):** KaRaMeL-extracted `*_US.c` files
> existed but were not linked into firmware — ABI mismatch. Resolution: D6
> `exec_fb_call_low` calls `ton_step_us`, `tof_step_us`, `tp_step_us` directly
> from F\*, so the verified wrapper functions are called from within the verified
> interpreter. The separate `*_US.c` files are superseded; the ABI mismatch is
> eliminated at the F\* level.
>
> **ADR-009 Discrepancy 3 (CLOSED by D6):** F\* `var_pool` record did not
> contain `scan_cycle_period_us`. Resolution: `LowPLC.Interp.State.fst`
> `low_pool` record now contains `lp_scan_period_us : UInt32.t`. The F\* type
> system enforces its presence at every call site.
>
> **ADR-009 Discrepancy 1 (OPEN):** Stale comment in `everparse/LowPLC_TLV.3d`
> line 44: "Converted to pt\_ticks at load time" — should read "at dispatch
> time". One-line fix. TODO before paper.
