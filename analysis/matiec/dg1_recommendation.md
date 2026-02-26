# DELIVERABLE 5: DG-1 Recommendation — LowPLC Parser/Frontend Strategy

## Decision Context

LowPLC is a verified PLC interpreter targeting baremetal STM32. We need to decide how to get IEC 61131-3 programs into our verified F* execution engine. Four options are assessed.

## Option Assessment

### Option A: Write a Verified Parser from Scratch

**Scope**: Reimplement IEC 61131-3 parsing in F* or Low*.

**What we'd need to build**:
- Lexer: matiec's `iec_flex.ll` is 2,300 lines of Flex with context-sensitive tokenization
- Parser: matiec's `iec_bison.yy` is 8,911 lines of Bison grammar
- AST: 323 node types defined in `absyntax/absyntax.def`
- Semantic analysis: 9 passes (enum check, flow control, constant folding, type safety × 4 sub-passes, lvalue check, array range check, case check)

**Complexity analysis**:
- The IEC 61131-3 grammar is NOT context-free — Flex and Bison cooperate bidirectionally. The lexer returns different token types for the same identifier based on prior declarations (e.g., `variable_name_token` vs `derived_function_name_token`). A standard parser generator cannot handle this.
- Even matiec deviates from the IEC spec to resolve LALR(1) conflicts (documented extensively in the `readme`)
- Standard function names (LIMIT, MIN, MAX, ABS, SEL, ADD, etc.) are keywords, creating naming collisions that would need careful handling

**Verdict**: **REJECT**. Enormous scope, high risk, no verification advantage over simpler approaches. A verified parser for the full IEC 61131-3 language would be a multi-year research project in itself.

---

### Option B: Consume matiec IR (Add Serialization Backend)

**Approach**: Add a new stage4 backend to matiec that serializes the annotated AST to a binary format (e.g., TLV, CBOR, protobuf). LowPLC reads this format.

**Feasibility**:
- Stage4 is cleanly replaceable — proven by `iec2iec` backend
- Factory pattern in `stage4.cc`: implement `new_code_generator()` + `delete_code_generator()`
- Would need to traverse ~50-100 relevant AST node types (for ST-only)
- Serialization format would need to capture: node types, child relationships, identifier values, type annotations, constant values

**What the IR actually looks like**:
- NOT a simplified IR — it's the full parse tree with 323 node classes
- Every node carries: source location, candidate types, resolved type, constant value, scope
- FB invocations carry `called_fb_declaration` pointers (back-references into the AST)
- List nodes, optional children (NULL), and circular references (parent pointers)

**TCB implications**:
- matiec (all of it — 11K+ lines of flex/bison, C++ AST, 9 semantic passes) would be in the TCB
- Type annotations come from matiec's type checker — we'd be trusting its correctness
- Any matiec bug (misparse, wrong type annotation) becomes a LowPLC soundness hole

**What we'd still need**:
- A serialization backend for matiec (~1-2K lines C++)
- A deserialization parser in F*/Low* for the binary format
- A validator that re-checks: types, variable declarations, FB signatures, disallowed constructs
- If we're re-checking everything anyway, we've built most of a parser

**Verdict**: **VIABLE but suboptimal**. The IR is too complex for what we need, and trusting matiec's semantic analysis defeats the purpose of verification. If we add a validator to re-check everything, we've essentially built Option C with extra steps.

---

### Option C: Simplified DSL (Recommended)

**Approach**: Define a minimal grammar covering only the IEC 61131-3 constructs needed for Category A benchmarks. Write a verified parser and type checker in F* directly.

**Grammar scope** (Category A subset):

```
program     := PROGRAM id var_block stmt_list END_PROGRAM config
var_block   := VAR var_decl* END_VAR
var_decl    := id ':' type [':=' init_value] ';'
type        := BOOL | INT | TIME | fb_type
fb_type     := TON | TOF | TP | CTU | CTD | CTUD | SR | RS | R_TRIG | F_TRIG
stmt        := assign | if_stmt | for_stmt | fb_call
assign      := id ':=' expr ';'
if_stmt     := IF expr THEN stmt* (ELSIF expr THEN stmt*)* (ELSE stmt*)? END_IF ';'
for_stmt    := FOR id ':=' expr TO expr (BY expr)? DO stmt* END_FOR ';'
fb_call     := id '(' param_list ')' ';'
expr        := or_expr
or_expr     := and_expr (OR and_expr)*
and_expr    := cmp_expr (AND cmp_expr)*
cmp_expr    := add_expr (('=' | '<>' | '<' | '>' | '<=' | '>=') add_expr)?
add_expr    := mul_expr (('+' | '-') mul_expr)*
mul_expr    := unary_expr (('*' | '/' | MOD) unary_expr)*
unary_expr  := NOT? primary
primary     := id | id '.' id | integer | time_lit | bool_lit | '(' expr ')'
config      := CONFIGURATION id resource END_CONFIGURATION
resource    := RESOURCE id ON id task program_inst END_RESOURCE
task        := TASK id '(' INTERVAL ':=' time_lit ',' PRIORITY ':=' integer ')' ';'
program_inst:= PROGRAM id WITH id ':' id ';'
```

**Estimated complexity**: ~30-40 grammar rules, ~15-20 AST node types, ~5 type-checking rules.

**What this covers**:
- All 4 benchmark programs (motor control, tank level, traffic light, batch)
- Boolean logic (AND, OR, NOT, XOR)
- Integer arithmetic (+, -, *, /, MOD) with comparison operators
- Timer FBs (TON, TOF, TP)
- Counter FBs (CTU, CTD, CTUD)
- Bistable FBs (SR, RS)
- Edge detectors (R_TRIG, F_TRIG)
- IF/ELSIF/ELSE, FOR loops
- FB invocation with formal parameter syntax
- Task scheduling with INTERVAL

**What this excludes**:
- WHILE / REPEAT loops (not needed; FOR is sufficient for bounded iteration)
- REAL / LREAL types (no floating point on baremetal target)
- STRING types (no dynamic allocation)
- Instruction List (IL) — legacy language
- Sequential Function Chart (SFC) — can be expressed as ST
- CASE statements (can be expressed as IF/ELSIF chains)
- Arrays (not needed for Category A benchmarks)
- User-defined types / STRUCTs
- FUNCTION / FUNCTION_BLOCK definitions (only standard library FBs)
- Multiple programs per resource
- Located variables (%IX, %QX)

**Verification properties**:
- Parser correctness: Every accepted program produces a well-typed AST
- Type safety: No type confusion between BOOL/INT/TIME
- Termination: FOR loops have bounded iteration (integer bounds, no WHILE)
- Memory safety: Fixed-size state struct, no heap allocation
- FB semantics: TON/CTU/etc. state machines verified against IEC spec

**Advantages**:
- Tiny grammar — realistic for F* verification in weeks, not years
- Complete control over semantics — no dependency on matiec correctness
- IEC 61131-3 compatible syntax — existing programs work with minimal modification
- No C/C++ in the TCB

**Verdict**: **RECOMMENDED**. Best balance of verification scope, implementation effort, and benchmark coverage.

---

### Option D: Hybrid — matiec Frontend + Verified Validator

**Approach**: Use matiec as an untrusted frontend. Parse with matiec, emit a simplified representation, then validate it with a verified checker before execution.

**Architecture**:
```
IEC source → matiec (untrusted) → simplified AST format → verified validator → LowPLC interpreter
```

**What the validator must check**:
1. All variables declared with valid types (BOOL, INT, TIME, or known FB types)
2. Type annotations match actual operand types (no `INT + BOOL`)
3. FB invocations match FB signatures (TON inputs: IN:BOOL, PT:TIME)
4. No disallowed constructs (WHILE, REAL, STRING, etc.)
5. FOR loop bounds are well-formed
6. All referenced variables are in scope
7. No function definitions (only standard library)

**Feasibility**:
- Stage4 replacement: proven feasible (see Option B analysis)
- Validator complexity: similar to Option C's type checker
- matiec stays outside the TCB — validator catches any matiec bugs

**Advantages over Option B**: matiec bugs don't compromise soundness
**Advantages over Option C**: Handles full IEC syntax (users don't need to learn a subset)
**Disadvantages**: Still need matiec installed, still need a serialization format, validator is ~same effort as a simple parser

**Verdict**: **VIABLE as fallback**. If benchmark programs need constructs beyond the simplified DSL (Option C), this provides a path to support them without expanding the verified parser.

---

## Recommendation

### Primary: Option C — Simplified DSL

Write a verified parser and type checker in F* for a minimal IEC 61131-3 subset. This gives us:

- **Minimal TCB**: No C/C++ code in the trusted base
- **Complete verification**: Parser → type checker → interpreter, all in F*
- **Sufficient coverage**: All Category A benchmarks expressible
- **Manageable scope**: ~30-40 grammar rules, feasible to verify

### Fallback: Option D — Hybrid matiec + Validator

If we discover that real-world programs need constructs beyond the DSL (arrays, CASE statements, multiple programs), add a matiec serialization backend and verified validator. The validator reuses most of the Option C type checker.

### Implementation Sequence

```
Phase 1: F* AST types for simplified DSL           (1 week)
Phase 2: F* parser for DSL grammar                  (2 weeks)
Phase 3: F* type checker / well-formedness checker  (1 week)
Phase 4: Validate against matiec output             (integration test)
Phase 5: (If needed) matiec stage4 serializer       (1 week C++)
Phase 6: (If needed) F* validator for Option D      (1 week)
```

### Key Findings Informing This Recommendation

1. **matiec's IR is not simplified** — it's the full 323-class parse tree with semantic annotations. There is no clean, compact intermediate form to consume.

2. **The grammar is context-sensitive** — the cooperative flex/bison architecture makes a faithful reimplementation extremely complex. A simplified grammar avoids this entirely.

3. **Stage4 is cleanly replaceable** — the factory pattern and iec2iec proof-of-concept mean Option D remains viable if needed later.

4. **Standard FBs are pure state machines** — TON, CTU, etc. are simple functions of (state, inputs) → (state, outputs). They can be directly modeled in F* without parsing their implementations.

5. **Runtime semantics are straightforward** — the generated C reveals that IF→`if`, FOR→`while`, assignment→struct field write, FB call→set inputs/call body/read outputs. No complex runtime machinery.

6. **Type universe is tiny for Category A** — BOOL (uint8), INT (int16), TIME ({int64 sec, int32 nsec}). Three types plus ~10 FB instance types.

7. **Keyword collisions exist** — standard function names are reserved (LIMIT, MIN, MAX, etc.). The simplified DSL avoids this by not having user-defined functions.
