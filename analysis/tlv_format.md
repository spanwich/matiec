# LowPLC TLV Binary Format Specification v1.0

## 1. Overview

The LowPLC TLV format is the binary encoding consumed by the verified F*
interpreter.  A **TLV serializer** (Task D4) compiles a LowPLC-ST AST into this
format; the **verified interpreter** (Task D3) executes it.

Design goals:

1. **Flat opcodes** — no expression sub-trees.  Compound expressions are flattened
   by the serializer into a sequence of scratch-variable operations.  The
   interpreter is a simple sequential scan, not a tree-walker.
2. **Uniform TLV framing** — every opcode is `[tag:1][len:2][payload:len]`.
   The 2-byte length field lets the interpreter skip unknown opcodes and provides
   the decreasing measure for F\*'s termination proof.
3. **Typed variable pools** — variables are addressed by pool type + index.
   BOOL and INT are separate pools.  Each FB type has its own state pool.
4. **Compile-time tick conversion** — TIME literals are converted to `uint32`
   tick counts by the serializer using DDR-006 from `lowplcst_spec.md`.
5. **Little-endian** — all multi-byte integers are stored least-significant byte
   first, matching the ARM Cortex-M7 target.

---

## 2. File Structure

A TLV binary consists of three contiguous sections:

```
┌──────────────────────────┐  offset 0
│         HEADER           │  20 bytes (fixed)
├──────────────────────────┤  offset 20
│      INIT section        │  header.init_len bytes
├──────────────────────────┤  offset 20 + init_len
│      BODY section        │  header.body_len bytes
└──────────────────────────┘
```

- **HEADER**: pool counts and section lengths.
- **INIT**: sequence of TLV opcodes (LOAD_TRUE, LOAD_FALSE, LOAD_INT) executed
  **once** at program load to set non-default initial values.
- **BODY**: sequence of TLV opcodes executed **every scan cycle**.

Total file size: `20 + init_len + body_len` bytes.

---

## 3. Header Format (20 bytes)

| Offset | Size | Field       | Description |
|--------|------|-------------|-------------|
| 0      | 2    | `magic`     | `0x4C 0x50` ("LP") |
| 2      | 1    | `version`   | `0x01` |
| 3      | 1    | `flags`     | Reserved, must be `0x00` |
| 4      | 1    | `n_bools`   | Number of BOOL pool entries (user + scratch) |
| 5      | 1    | `n_ints`    | Number of INT pool entries (user + scratch) |
| 6      | 1    | `n_ton`     | Number of TON instances |
| 7      | 1    | `n_tof`     | Number of TOF instances |
| 8      | 1    | `n_tp`      | Number of TP instances |
| 9      | 1    | `n_ctu`     | Number of CTU instances |
| 10     | 1    | `n_ctd`     | Number of CTD instances |
| 11     | 1    | `n_ctud`    | Number of CTUD instances |
| 12     | 1    | `n_sr`      | Number of SR instances |
| 13     | 1    | `n_rs`      | Number of RS instances |
| 14     | 1    | `n_rtrig`   | Number of R_TRIG instances |
| 15     | 1    | `n_ftrig`   | Number of F_TRIG instances |
| 16     | 2    | `init_len`  | Byte length of INIT section (LE) |
| 18     | 2    | `body_len`  | Byte length of BODY section (LE) |

**Bounds** (enforced by EverParse validator):

| Pool     | Max entries | Rationale |
|----------|-------------|-----------|
| `n_bools`  | 64        | B5 uses ~30 bools; 64 gives ample scratch room |
| `n_ints`   | 32        | B5 uses ~12 ints |
| Each FB count | 8      | B5 uses at most 1 per type; 8 handles multi-instance |
| `n_ctud`   | 4         | CTUD is heaviest FB (220 bytes .text) |
| `n_tp`     | 4         | TP not in benchmarks; small limit |

All pool entries are initialized to zero/false at program load, before the INIT
section runs.  FB state is initialized per-type: phase=0 (IDLE), prev_in=false,
start_tick=0, cv=0, prev_cu=false, prev_cd=false, q=false, m=false.

---

## 4. Variable Pool Layout

### 4.1 Scalar Pools

**BOOL pool** — `bool bools[n_bools]`

Each entry is a single boolean.  Indices 0..n_bools-1.  The serializer assigns:
- Input variables first (set by the host before each scan)
- Internal and output variables next
- Scratch variables (for expression temporaries) last

**INT pool** — `int16_t ints[n_ints]`

Each entry is a 16-bit signed integer.  Same index assignment convention.

### 4.2 FB State Pools

Each FB type has a parallel-array state pool.  The interpreter indexes into
these arrays using the `idx` field from FB call opcodes.

| FB type | State fields (per instance) | Total bytes/instance |
|---------|---------------------------|---------------------|
| TON     | `phase: u8`, `start_tick: u32` | 5 |
| TOF     | `phase: u8`, `prev_in: u8`, `start_tick: u32` | 6 |
| TP      | `phase: u8`, `prev_in: u8`, `start_tick: u32` | 6 |
| CTU     | `cv: i16`, `prev_cu: u8` | 3 |
| CTD     | `cv: i16`, `prev_cd: u8` | 3 |
| CTUD    | `cv: i16`, `prev_cu: u8`, `prev_cd: u8` | 4 |
| SR      | `q: u8` | 1 |
| RS      | `q: u8` | 1 |
| R_TRIG  | `m: u8` | 1 |
| F_TRIG  | `m: u8` | 1 |

Field types correspond to the extracted C combinator buffer parameters.  For
example, `ton_step_c` takes `uint8_t *phase_buf, uint32_t *start_buf`, so
TON state is stored as parallel `u8 ton_phase[n_ton]` and `u32 ton_start[n_ton]`
arrays.

---

## 5. TLV Opcode Format

Every opcode uses Tag-Length-Value framing:

```
┌─────────┬─────────┬─────────────────┐
│ tag: 1  │ len: 2  │ payload: len    │
└─────────┴─────────┴─────────────────┘
```

- `tag`: 1-byte opcode identifier.
- `len`: 2-byte little-endian payload length (0..65535).
- `payload`: `len` bytes of opcode-specific data.

Total opcode size: `3 + len` bytes.  The interpreter consumes exactly `3 + len`
bytes per opcode, providing the termination measure for F\*'s `decreases`
annotation.

### 5.1 Operand Encoding

All operands in TLV payloads are **pool indices** (1-byte unsigned):
- `b` suffix: index into the BOOL pool (0..n_bools-1)
- `i` suffix: index into the INT pool (0..n_ints-1)

Literal values are loaded into pool slots via LOAD_TRUE, LOAD_FALSE, and
LOAD_INT opcodes.  FB call arguments reference pool slots, not inline values
(except for timer PT which is a compile-time constant encoded inline as uint32).

---

## 6. Opcode Reference

### 6.1 Literal Loads

| Tag  | Mnemonic    | Len | Payload          | Description |
|------|-------------|-----|------------------|-------------|
| 0x01 | LOAD_TRUE   | 1   | `[dst_b:1]`     | `bools[dst_b] := true` |
| 0x02 | LOAD_FALSE  | 1   | `[dst_b:1]`     | `bools[dst_b] := false` |
| 0x03 | LOAD_INT    | 3   | `[dst_i:1][val:2]` | `ints[dst_i] := val` (i16, LE) |

### 6.2 Boolean Operations

| Tag  | Mnemonic    | Len | Payload               | Description |
|------|-------------|-----|-----------------------|-------------|
| 0x10 | BOOL_NOT    | 2   | `[dst_b:1][src_b:1]`  | `bools[dst] := NOT bools[src]` |
| 0x11 | BOOL_AND    | 3   | `[dst_b:1][a_b:1][b_b:1]` | `bools[dst] := bools[a] AND bools[b]` |
| 0x12 | BOOL_OR     | 3   | `[dst_b:1][a_b:1][b_b:1]` | `bools[dst] := bools[a] OR bools[b]` |
| 0x13 | BOOL_XOR    | 3   | `[dst_b:1][a_b:1][b_b:1]` | `bools[dst] := bools[a] XOR bools[b]` |
| 0x14 | COPY_BOOL   | 2   | `[dst_b:1][src_b:1]`  | `bools[dst] := bools[src]` |

### 6.3 Integer Arithmetic

| Tag  | Mnemonic    | Len | Payload               | Description |
|------|-------------|-----|-----------------------|-------------|
| 0x20 | INT_ADD     | 3   | `[dst_i:1][a_i:1][b_i:1]` | `ints[dst] := ints[a] + ints[b]` (wraps) |
| 0x21 | INT_SUB     | 3   | `[dst_i:1][a_i:1][b_i:1]` | `ints[dst] := ints[a] - ints[b]` (wraps) |
| 0x22 | INT_MUL     | 3   | `[dst_i:1][a_i:1][b_i:1]` | `ints[dst] := ints[a] * ints[b]` (wraps) |
| 0x23 | COPY_INT    | 2   | `[dst_i:1][src_i:1]`  | `ints[dst] := ints[src]` |

INT arithmetic wraps modulo 2^16 per DDR-003.

### 6.4 Integer Comparisons

| Tag  | Mnemonic | Len | Payload               | Description |
|------|----------|-----|-----------------------|-------------|
| 0x30 | CMP_EQ   | 3   | `[dst_b:1][a_i:1][b_i:1]` | `bools[dst] := (ints[a] = ints[b])` |
| 0x31 | CMP_NE   | 3   | `[dst_b:1][a_i:1][b_i:1]` | `bools[dst] := (ints[a] <> ints[b])` |
| 0x32 | CMP_LT   | 3   | `[dst_b:1][a_i:1][b_i:1]` | `bools[dst] := (ints[a] < ints[b])` |
| 0x33 | CMP_GT   | 3   | `[dst_b:1][a_i:1][b_i:1]` | `bools[dst] := (ints[a] > ints[b])` |
| 0x34 | CMP_LE   | 3   | `[dst_b:1][a_i:1][b_i:1]` | `bools[dst] := (ints[a] <= ints[b])` |
| 0x35 | CMP_GE   | 3   | `[dst_b:1][a_i:1][b_i:1]` | `bools[dst] := (ints[a] >= ints[b])` |

### 6.5 Control Flow

#### IF_BLOCK (0x40)

```
Tag: 0x40
Payload:
  [cond_b: 1]                     — BOOL pool index of IF condition
  [then_len: 2]                   — byte length of THEN body (LE)
  [then_body: then_len bytes]     — nested TLV opcodes for THEN branch
  [num_elsif: 1]                  — number of ELSIF clauses (0..255)
  For each ELSIF clause:
    [elsif_cond_b: 1]             — BOOL pool index of ELSIF condition
    [elsif_len: 2]                — byte length of ELSIF body (LE)
    [elsif_body: elsif_len bytes] — nested TLV opcodes for ELSIF branch
  [else_len: 2]                   — byte length of ELSE body (LE, 0 = no ELSE)
  [else_body: else_len bytes]     — nested TLV opcodes for ELSE branch
```

**Semantics**: The interpreter checks `bools[cond_b]`.  If true, executes
`then_body`.  Otherwise, checks each ELSIF condition in order; executes the
first matching body.  If no condition matches and `else_len > 0`, executes
`else_body`.

**Condition pre-computation**: All condition expressions (IF and ELSIF) are
computed into BOOL pool scratch slots by opcodes placed **before** the IF_BLOCK
in the opcode stream.  Since LowPLC-ST expressions have no side effects
(DDR-008: no short-circuit), pre-computing all conditions is semantically
correct.

**Nesting**: The `then_body`, `elsif_body`, and `else_body` sections may contain
further IF_BLOCK and FOR_BLOCK opcodes.  Maximum nesting depth is bounded by the
benchmark programs (max depth 2 in B2, B4, B5).

#### FOR_BLOCK (0x41)

```
Tag: 0x41
Payload:
  [idx_i: 1]                     — INT pool index of loop control variable
  [start: 2]                     — start value (i16, LE)
  [end: 2]                       — end value (i16, LE)
  [body_len: 2]                  — byte length of loop body (LE)
  [body: body_len bytes]         — nested TLV opcodes for loop body
```

Total payload length: `7 + body_len`.

**Semantics**: Initialize `ints[idx_i] := start`.  While `ints[idx_i] <= end`,
execute `body`, then `ints[idx_i] := ints[idx_i] + 1`.  Step is always +1
(the serializer validates constant step = 1 for Category A programs; BY clause
support is deferred).

**Termination**: The interpreter uses a fuel counter initialized to
`max(0, end - start + 1)`.  F\*'s `decreases fuel` annotation proves
termination.  Maximum iterations across all benchmarks: 10 (B5).

### 6.6 Timer FB Calls

| Tag  | Mnemonic  | Len | Payload | Description |
|------|-----------|-----|---------|-------------|
| 0x50 | TON_CALL  | 7   | `[idx:1][in_b:1][pt:4][q_b:1]` | On-delay timer |
| 0x51 | TOF_CALL  | 7   | `[idx:1][in_b:1][pt:4][q_b:1]` | Off-delay timer |
| 0x52 | TP_CALL   | 7   | `[idx:1][in_b:1][pt:4][q_b:1]` | Pulse timer |

- `idx`: FB instance index (0..n_ton-1 etc.)
- `in_b`: BOOL pool index for IN input
- `pt`: timer preset in ticks (uint32, LE) — compile-time constant per DDR-006
- `q_b`: BOOL pool index where Q output is written

The interpreter calls the corresponding combinator step function:
```c
ton_step_c(&ton_phase[idx], &ton_start[idx],
           bools[in_b], pt, current_tick,
           &q_scratch, &et_scratch);
bools[q_b] = q_scratch;
```

Timer ET output is computed but not stored in the variable pool (no benchmark
reads ET directly).  The interpreter uses a local scratch buffer for ET.

### 6.7 Counter FB Calls

| Tag  | Mnemonic   | Len | Payload | Description |
|------|------------|-----|---------|-------------|
| 0x60 | CTU_CALL   | 6   | `[idx:1][cu_b:1][r_b:1][pv_i:1][q_b:1][cv_i:1]` | Up counter |
| 0x61 | CTD_CALL   | 6   | `[idx:1][cd_b:1][ld_b:1][pv_i:1][q_b:1][cv_i:1]` | Down counter |
| 0x62 | CTUD_CALL  | 9   | `[idx:1][cu_b:1][cd_b:1][r_b:1][ld_b:1][pv_i:1][qu_b:1][qd_b:1][cv_i:1]` | Up/down counter |

- `idx`: FB instance index
- `cu_b`, `cd_b`, `r_b`, `ld_b`: BOOL pool indices for inputs
- `pv_i`: INT pool index for preset value
- `q_b`, `qu_b`, `qd_b`: BOOL pool indices for output(s)
- `cv_i`: INT pool index where CV output is written

Example dispatch for CTU:
```c
ctu_step_c(&ctu_cv[idx], &ctu_prev[idx],
           bools[cu_b], bools[r_b], ints[pv_i],
           &q_scratch, &cv_scratch);
bools[q_b] = q_scratch;
ints[cv_i] = cv_scratch;
```

### 6.8 Bistable and Edge Detector FB Calls

| Tag  | Mnemonic     | Len | Payload | Description |
|------|-------------|-----|---------|-------------|
| 0x70 | SR_CALL     | 4   | `[idx:1][s1_b:1][r_b:1][q1_b:1]` | Set-dominant bistable |
| 0x71 | RS_CALL     | 4   | `[idx:1][s_b:1][r1_b:1][q1_b:1]` | Reset-dominant bistable |
| 0x72 | R_TRIG_CALL | 3   | `[idx:1][clk_b:1][q_b:1]` | Rising edge detector |
| 0x73 | F_TRIG_CALL | 3   | `[idx:1][clk_b:1][q_b:1]` | Falling edge detector |

- `idx`: FB instance index
- Input bool references: S1, R (for SR); S, R1 (for RS); CLK (for triggers)
- `q1_b` / `q_b`: BOOL pool index where output is written

---

## 7. Opcode Summary Table

| Tag  | Mnemonic     | Len | Total | Category |
|------|-------------|-----|-------|----------|
| 0x01 | LOAD_TRUE    | 1   | 4     | Load |
| 0x02 | LOAD_FALSE   | 1   | 4     | Load |
| 0x03 | LOAD_INT     | 3   | 6     | Load |
| 0x10 | BOOL_NOT     | 2   | 5     | Bool op |
| 0x11 | BOOL_AND     | 3   | 6     | Bool op |
| 0x12 | BOOL_OR      | 3   | 6     | Bool op |
| 0x13 | BOOL_XOR     | 3   | 6     | Bool op |
| 0x14 | COPY_BOOL    | 2   | 5     | Bool op |
| 0x20 | INT_ADD      | 3   | 6     | Int arith |
| 0x21 | INT_SUB      | 3   | 6     | Int arith |
| 0x22 | INT_MUL      | 3   | 6     | Int arith |
| 0x23 | COPY_INT     | 2   | 5     | Int arith |
| 0x30 | CMP_EQ       | 3   | 6     | Compare |
| 0x31 | CMP_NE       | 3   | 6     | Compare |
| 0x32 | CMP_LT       | 3   | 6     | Compare |
| 0x33 | CMP_GT       | 3   | 6     | Compare |
| 0x34 | CMP_LE       | 3   | 6     | Compare |
| 0x35 | CMP_GE       | 3   | 6     | Compare |
| 0x40 | IF_BLOCK     | *   | 3+*   | Control |
| 0x41 | FOR_BLOCK    | *   | 3+*   | Control |
| 0x50 | TON_CALL     | 7   | 10    | Timer FB |
| 0x51 | TOF_CALL     | 7   | 10    | Timer FB |
| 0x52 | TP_CALL      | 7   | 10    | Timer FB |
| 0x60 | CTU_CALL     | 6   | 9     | Counter FB |
| 0x61 | CTD_CALL     | 6   | 9     | Counter FB |
| 0x62 | CTUD_CALL    | 9   | 12    | Counter FB |
| 0x70 | SR_CALL      | 4   | 7     | Bistable FB |
| 0x71 | RS_CALL      | 4   | 7     | Bistable FB |
| 0x72 | R_TRIG_CALL  | 3   | 6     | Edge FB |
| 0x73 | F_TRIG_CALL  | 3   | 6     | Edge FB |

**Total: 30 opcodes** (3 loads, 5 bool ops, 4 int ops, 6 comparisons, 2 control
flow, 3 timer calls, 3 counter calls, 4 bistable/edge calls).

---

## 8. Encoding Examples

### 8.1 B1: ESD Interlock

Source: `benchmarks/b1_esd.st`

#### Variable Pool Mapping

```
BOOL pool (n_bools = 12):
  0: pressure_high     (input)
  1: temp_high         (input)
  2: flow_low          (input)
  3: manual_reset      (input)
  4: any_trip           (internal)
  5: latched            (internal)
  6: shutdown_cmd       (output)
  7: alarm_out          (output)
  8: [scratch] alarm_latch.Q1 — SR output
  9: [scratch] alarm_delay.Q  — TON output
 10: [scratch] expression temporaries
 11: [scratch] expression temporaries

INT pool (n_ints = 0)

FB instances:
  n_sr = 1:    [0] alarm_latch
  n_ton = 1:   [0] alarm_delay
  (all other FB counts = 0)
```

#### Opcode Sequence (INIT section: 0 bytes)

No non-default initializers in B1.

#### Opcode Sequence (BODY section)

```
Stmt: any_trip := pressure_high OR temp_high OR flow_low
  BOOL_OR   [10] [0] [1]     — scratch = pressure_high OR temp_high
  BOOL_OR   [4]  [10] [2]    — any_trip = scratch OR flow_low

Stmt: alarm_latch(S1 := any_trip, R := manual_reset)
  SR_CALL   [0] [4] [3] [8]  — SR idx=0, S1=any_trip, R=manual_reset, Q1→bool[8]

Stmt: latched := alarm_latch.Q1
  COPY_BOOL [5] [8]          — latched = alarm_latch.Q1

Stmt: alarm_delay(IN := latched, PT := T#3s)
  TON_CALL  [0] [5] [300:u32] [9]  — TON idx=0, IN=latched, PT=300 ticks, Q→bool[9]
  (300 ticks = 3000ms / 10ms interval)

Stmt: IF latched AND alarm_delay.Q THEN ... ELSIF ... ELSE ... END_IF
  Pre-compute conditions:
  BOOL_AND  [10] [5] [9]     — scratch = latched AND alarm_delay.Q
  BOOL_NOT  [11] [9]         — scratch = NOT alarm_delay.Q
  BOOL_AND  [11] [5] [11]    — scratch = latched AND NOT alarm_delay.Q
  IF_BLOCK  cond=[10]
    THEN (8 bytes):
      LOAD_TRUE  [6]         — shutdown_cmd := TRUE
      LOAD_TRUE  [7]         — alarm_out := TRUE
    ELSIF 1:  cond=[11]
      ELSIF body (8 bytes):
        LOAD_FALSE [6]       — shutdown_cmd := FALSE
        LOAD_FALSE [7]       — alarm_out := FALSE
    ELSE (8 bytes):
      LOAD_FALSE [6]         — shutdown_cmd := FALSE
      LOAD_FALSE [7]         — alarm_out := FALSE
```

#### Byte-level BODY Encoding

```
Offset  Hex                           Opcode
------  ---                           ------
0x00    12 03 00 0A 00 01             BOOL_OR  dst=10 a=0 b=1
0x06    12 03 00 04 0A 02             BOOL_OR  dst=4  a=10 b=2
0x0C    70 04 00 00 04 03 08          SR_CALL  idx=0 s1=4 r=3 q1=8
0x13    14 02 00 05 08                COPY_BOOL dst=5 src=8
0x18    50 07 00 00 05 2C010000 09    TON_CALL idx=0 in=5 pt=0x012C(300) q=9
0x22    11 03 00 0A 05 09             BOOL_AND dst=10 a=5 b=9
0x28    10 02 00 0B 09                BOOL_NOT dst=11 src=9
0x2D    11 03 00 0B 05 0B             BOOL_AND dst=11 a=5 b=11
0x33    40 XX XX                      IF_BLOCK (len = payload below)
          0A                            cond=10
          08 00                         then_len=8
          01 01 00 06                     LOAD_TRUE dst=6
          01 01 00 07                     LOAD_TRUE dst=7
          01                            num_elsif=1
          0B                              elsif_cond=11
          08 00                           elsif_len=8
          02 01 00 06                       LOAD_FALSE dst=6
          02 01 00 07                       LOAD_FALSE dst=7
          08 00                         else_len=8
          02 01 00 06                     LOAD_FALSE dst=6
          02 01 00 07                     LOAD_FALSE dst=7
```

IF_BLOCK payload = 1+2+8 + 1+(1+2+8) + 2+8 = 33 bytes.
IF_BLOCK total = 3+33 = 36 bytes.

**B1 BODY total**: 6+6+7+5+10+6+5+6+36 = **87 bytes**.
**B1 total file size**: 20 (header) + 0 (init) + 87 (body) = **107 bytes**.

### 8.2 B3: Conveyor with Part Counter and FOR Loop

Source: `benchmarks/b3_conveyor.st`

#### Variable Pool Mapping

```
BOOL pool (n_bools = 9):
  0: part_detect       (input)
  1: batch_reset       (input)
  2: run_cmd           (input)
  3: motor_latch       (internal)
  4: motor_on          (output)
  5: batch_complete    (output)
  6: [scratch] part_edge.Q     — R_TRIG output
  7: [scratch] part_counter.Q  — CTU output
  8: [scratch] temporaries

INT pool (n_ints = 5):
  0: batch_size        (internal, init=10)
  1: batch_total       (internal)
  2: idx               (FOR control variable)
  3: total_count       (output)
  4: [scratch] part_counter.CV — CTU output

FB instances:
  n_rtrig = 1:  [0] part_edge
  n_ctu = 1:    [0] part_counter
```

#### INIT Section

```
LOAD_INT [0] 10               — batch_size := 10
```

INIT section: 6 bytes.

#### BODY Opcode Sequence

```
Stmt: part_edge(CLK := part_detect)
  R_TRIG_CALL [0] [0] [6]    — idx=0, clk=part_detect, q→bool[6]

Stmt: part_counter(CU := part_edge.Q, R := batch_reset, PV := batch_size)
  CTU_CALL [0] [6] [1] [0] [7] [4]
    — idx=0, cu=bool[6], r=bool[1], pv=int[0], q→bool[7], cv→int[4]

Stmt: IF run_cmd THEN motor_latch := TRUE; END_IF
  IF_BLOCK cond=[2]
    THEN (4 bytes): LOAD_TRUE [3]
    num_elsif=0
    else_len=0

Stmt: IF part_counter.Q THEN motor_latch := FALSE; END_IF
  IF_BLOCK cond=[7]
    THEN (4 bytes): LOAD_FALSE [3]
    num_elsif=0
    else_len=0

Stmt: motor_on := motor_latch
  COPY_BOOL [4] [3]

Stmt: batch_complete := part_counter.Q
  COPY_BOOL [5] [7]

Stmt: total_count := part_counter.CV
  COPY_INT [3] [4]

Stmt: IF batch_complete THEN batch_total := 0; FOR idx := 1 TO 5 DO ... END_FOR; END_IF
  IF_BLOCK cond=[5]
    THEN body:
      LOAD_INT [1] 0           — batch_total := 0
      FOR_BLOCK idx=2, start=1, end=5
        body:
          INT_ADD [1] [1] [0]  — batch_total := batch_total + batch_size
    num_elsif=0
    else_len=0
```

### 8.3 B5: Stress Test (Excerpt — All Comparison Operators)

Source: `benchmarks/b5_stress.st`

This excerpt covers the comparison operator section which exercises all 6
comparison opcodes.

#### Variable Pool Mapping (relevant subset)

```
INT pool (partial):
  4: inp_e    (init=0)
  5: inp_f    (init=50)

BOOL pool (partial):
  20: cmp_eq
  21: cmp_ne
  22: cmp_lt
  23: cmp_gt
  24: cmp_le
  25: cmp_ge
```

#### Opcode Sequence (excerpt)

```
Stmt: cmp_eq := (inp_e = inp_f)
  CMP_EQ  [20] [4] [5]       — bools[20] := (ints[4] = ints[5])

Stmt: cmp_ne := (inp_e <> inp_f)
  CMP_NE  [21] [4] [5]

Stmt: cmp_lt := (inp_e < inp_f)
  CMP_LT  [22] [4] [5]

Stmt: cmp_gt := (inp_e > inp_f)
  CMP_GT  [23] [4] [5]

Stmt: cmp_le := (inp_e <= inp_f)
  CMP_LE  [24] [4] [5]

Stmt: cmp_ge := (inp_e >= inp_f)
  CMP_GE  [25] [4] [5]
```

Total for 6 comparisons: 6 * 6 = **36 bytes**.

### 8.4 B5: Boolean Expression Flattening Example

The expression `bool_res := (inp_a AND inp_b) OR (NOT inp_c) XOR inp_d` is
flattened by the serializer into a sequence of scratch operations:

```
BOOL_AND  [s0] [0] [1]       — s0 = inp_a AND inp_b
BOOL_NOT  [s1] [2]           — s1 = NOT inp_c
BOOL_OR   [s0] [s0] [s1]    — s0 = s0 OR s1
BOOL_XOR  [26] [s0] [3]     — bool_res = s0 XOR inp_d
```

Total: 6+5+6+6 = **23 bytes** for one compound boolean expression.

---

## 9. Interpreter Execution Model

### 9.1 Entry Points

The interpreter exposes two entry points:

1. **`exec_init(program, pool)`** — executes the INIT section once to set
   non-default variable values.  Called at program load.

2. **`exec_scan(program, pool, tick)`** — executes the BODY section once per
   scan cycle.  Called by the PLC runtime with the current tick counter.

### 9.2 Dispatch Loop

```
exec_stmt_list(buf, offset, end, pool, tick):
  while offset < end:
    tag = buf[offset]
    len = read_u16_le(buf, offset+1)
    payload = buf[offset+3 .. offset+3+len]
    match tag:
      0x01 → pool.bools[payload[0]] := true
      0x11 → pool.bools[payload[0]] := pool.bools[payload[1]] AND pool.bools[payload[2]]
      0x50 → ton_step_c(&pool.ton_phase[payload[0]], &pool.ton_start[payload[0]],
                         pool.bools[payload[1]], read_u32_le(payload,2),
                         tick, &q_scratch, &et_scratch)
              pool.bools[payload[6]] := q_scratch
      0x40 → exec_if_block(payload, pool, tick)
      0x41 → exec_for_block(payload, pool, tick)
      ...
    offset := offset + 3 + len
```

### 9.3 Recursion and Stack Depth

IF_BLOCK and FOR_BLOCK contain nested opcode bodies.  The interpreter uses
recursion to execute nested bodies.  The maximum recursion depth equals the
maximum nesting depth of the source program:

| Benchmark | Max nesting depth |
|-----------|-------------------|
| B1        | 1 (IF at top level) |
| B2        | 2 (IF within IF/ELSIF) |
| B3        | 2 (FOR within IF) |
| B4        | 2 (IF within IF/ELSIF) |
| B5        | 2 (IF within IF/ELSIF) |

Maximum recursion depth across all benchmarks: **2**.  For the WCET analysis
(Task D5), this recursion is annotated with a depth bound of 5 (allowing
headroom for future programs).

### 9.4 Termination Proof Structure

The F\* spec function uses two decreasing measures:

1. **Statement list**: `decreases (remaining_bytes)` where `remaining_bytes =
   end - offset`.  Each opcode consumes `3 + len > 0` bytes, so the measure
   strictly decreases.

2. **FOR loop**: `decreases fuel` where `fuel = max(0, end_val - start_val + 1)`.
   Each iteration decrements fuel by 1.

Both measures are natural numbers and reach zero, guaranteeing termination.

---

## 10. Relationship to Other Tasks

| Task | Relationship |
|------|-------------|
| D2 (Combinators) | Opcodes 0x50-0x73 dispatch to the verified combinator step functions |
| D3 (Interpreter) | This document IS the input specification for the interpreter |
| D4 (Serializer) | The serializer produces TLV binaries conforming to this format |
| D5 (WCET) | Opcode sizes and nesting bounds feed the WCET calculator |
| A5 (EverParse) | The EverParse validator validates TLV binaries against this format |

---

## Appendix A: Design Decision Record

### TLV-DDR-001: Flat Expression Model

Expressions are flattened to scratch-variable operations rather than encoded as
sub-TLV trees.

**Rationale**: A flat opcode stream is simpler to verify (no recursive expression
evaluation), produces bounded-depth recursion (only control flow nests), and
generates predictable WCET per opcode.

### TLV-DDR-002: Timer PT as Inline Constant

Timer FB call opcodes carry PT as an inline 4-byte uint32 tick count, not as a
pool reference.

**Rationale**: Per DDR-006, TIME literals are converted to ticks at compile time.
All benchmark timer presets are literals (T#3s, T#5s, T#30s, etc.).  B5's
`inp_h : TIME := T#2s` is never modified, so the serializer can resolve it to
its constant value (200 ticks at 10ms interval).  This avoids needing a TIME
variable pool in the interpreter.

### TLV-DDR-003: No ET Output in Timer Opcodes

Timer call opcodes only expose the Q output.  ET (elapsed time) is computed
internally but not written to the variable pool.

**Rationale**: No benchmark reads ET directly.  Adding ET output would require a
uint32 pool (separate from the int16 INT pool).  If future programs need ET,
a variant opcode (TON_CALL_ET, 0x53) can be added.

### TLV-DDR-004: Pre-computed IF Conditions

All IF/ELSIF conditions are computed into scratch BOOL slots before the IF_BLOCK
opcode.  The IF_BLOCK references pre-computed results.

**Rationale**: LowPLC-ST expressions have no side effects (DDR-008).  Pre-computing
conditions before the IF_BLOCK is semantically identical to lazy evaluation but
simpler to verify: the IF_BLOCK interpreter only reads BOOL values, never
evaluates expressions.
