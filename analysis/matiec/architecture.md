# DELIVERABLE 1: matiec Pipeline Architecture

## Overview

matiec is an IEC 61131-3 compiler that translates Structured Text (ST), Instruction List (IL), and Sequential Function Chart (SFC) programs into ANSI C. It produces two executables: `iec2c` (IEC→C) and `iec2iec` (IEC→IEC, for debugging/normalization).

Based on **FINAL DRAFT — IEC 61131-3, 2nd Ed. (2001-12-10)**.

## Directory Structure

```
matiec/
├── main.cc                      # Entry point: orchestrates all stages
├── main.hh                      # Shared macros (ERROR, ERROR_MSG), type defs
│
├── absyntax/                    # AST node class definitions
│   ├── absyntax.def             # ~323 AST node definitions (SYM_* macros)
│   ├── absyntax.hh              # symbol_c base class, SYM_* macro expansion
│   ├── absyntax.cc              # symbol_c, token_c, list_c implementations
│   └── visitor.hh               # Visitor interface + 5 base classes
│
├── absyntax_utils/              # AST search/analysis utilities
│   ├── absyntax_utils.cc        # absyntax_utils_init() — populates symbol tables
│   ├── get_datatype_info.cc     # Type comparison/inspection utilities
│   ├── search_var_instance_decl.cc  # Find variable declarations by name
│   └── function_param_iterator.cc   # Iterate FB/function parameters
│
├── stage1_2/                    # Lexer + Parser (run cooperatively, single pass)
│   ├── iec_flex.ll              # Flex lexer (2300 lines)
│   ├── iec_bison.yy             # Bison grammar (8911 lines)
│   └── stage1_2.cc              # Entry: stage1_2(filename, &tree_root)
│
├── stage3/                      # Semantic analysis (9 ordered passes)
│   ├── stage3.cc                # Entry: stage3(tree_root, &ordered_tree_root)
│   ├── enum_declaration_check.cc    # Pass 1: Enum validity
│   ├── flow_control_analysis.cc     # Pass 2: IL instruction flow
│   ├── constant_folding.cc          # Pass 3: Compile-time constant evaluation
│   ├── declaration_check.cc         # Pass 4: Variable declaration validity
│   ├── fill_candidate_datatypes.cc  # Pass 5a: Enumerate possible types
│   ├── narrow_candidate_datatypes.cc # Pass 5b: Resolve to single type
│   ├── forced_narrow_candidate_datatypes.cc # Pass 5c: Force remaining types
│   ├── print_datatypes_error.cc     # Pass 5d: Report type errors
│   ├── lvalue_check.cc             # Pass 6: L-value validity
│   ├── array_range_check.cc        # Pass 7: Array bounds
│   ├── case_elements_check.cc      # Pass 8: CASE statement validity
│   └── remove_forward_dependencies.cc # Pass 9: Reorder for forward refs
│
├── stage4/                      # Code generation (visitor-based)
│   ├── stage4.cc                # Entry: stage4(tree_root, builddir) + factory
│   ├── stage4.hh                # stage4out_c output helper class
│   ├── generate_c/              # C code generator backend
│   │   ├── generate_c.cc        # Top-level C generator (new_code_generator)
│   │   ├── generate_c_st.cc     # ST → C translation
│   │   ├── generate_c_il.cc     # IL → C translation
│   │   ├── generate_c_sfc.cc    # SFC → C translation
│   │   └── generate_c_base.cc   # Shared C generation utilities
│   └── generate_iec/            # IEC code regenerator backend
│       └── generate_iec.cc      # AST → IEC text (used by iec2iec)
│
├── lib/                         # IEC standard library definitions
│   ├── standard_functions.txt   # Standard function declarations (parsed at startup)
│   └── C/                       # C implementations of IEC standard library
│       ├── iec_types.h          # IEC type → C type mappings
│       ├── iec_std_lib.h        # Standard functions (ADD, MUL, comparisons, etc.)
│       ├── iec_std_FB.h         # Standard FBs (TON, TOF, TP, CTU, CTD, SR, RS, etc.)
│       └── accessor.h           # __GET_VAR/__SET_VAR/__DECLARE_VAR macros
│
├── util/                        # Symbol table implementations
│   ├── symtable.cc              # Case-insensitive symbol table
│   └── dsymtable.cc             # Duplicate-key symbol table
│
└── tests/                       # Test suite
    ├── build.sh                 # Test runner (IEC → C → binary via gcc)
    └── syntax/                  # Test .test files
```

## Pipeline Data Flow

```
                              symbol tables
                              (bidirectional)
                                  ↕
┌─────────┐  tokens  ┌─────────┐  AST   ┌─────────────┐  annotated  ┌─────────┐
│ Stage 1  │ ──────→ │ Stage 2  │ ────→ │  Stage 3     │    AST     │ Stage 4  │
│ Flex     │ ←────── │ Bison    │        │  Semantics   │ ─────────→ │ Code Gen │
│ Lexer    │         │ Parser   │        │  (9 passes)  │            │ (visitor)│
└─────────┘         └─────────┘        └─────────────┘            └─────────┘
 iec_flex.ll        iec_bison.yy         stage3.cc                 generate_c/
                                                                    generate_iec/
     │                  │                     │                         │
     └──── Pass 1 ──────┘                     │                         │
           (cooperative,                  Pass 2                    Pass 3
            single pass)              (multiple sub-passes)     (visitor traversal)
```

### Detailed Pipeline (from `main.cc:242-258`)

```c
// Pass 1: Lexing + Parsing (cooperative, single pass)
stage1_2(argv[optind], &tree_root);

// Interlude: Initialize global symbol tables for fast lookup
absyntax_utils_init(tree_root);

// Pass 2: Semantic analysis (9 ordered sub-passes)
stage3(tree_root, &ordered_tree_root);

// Pass 3: Code generation
stage4(ordered_tree_root, builddir);
```

### Stage 3 Pass Ordering (from `stage3/stage3.cc:158-168`)

```
1. enum_declaration_check  — Validate enum declarations
2. flow_control_analysis   — Fill prev_il_instruction (IL flow)
3. constant_propagation    — Constant folding (depends on 2)
4. declaration_safety      — Variable declaration checks (depends on 3)
5. type_safety             — Type analysis (depends on 2, 3):
   5a. fill_candidate_datatypes   — Enumerate possible types per node
   5b. narrow_candidate_datatypes — Resolve to single type
   5c. print_datatypes_error      — Report errors
   5d. forced_narrow_candidate_datatypes — Force remaining
6. lvalue_check            — L-value validation (depends on 5)
7. array_range_check       — Array bounds (depends on 3)
8. case_elements_check     — CASE validity (depends on 3)
9. remove_forward_dependencies — Reorder for -p flag
```

## Key Architectural Questions

### Q1: Is there a clean AST boundary between parsing and code generation?

**Yes, but it's an annotated parse tree, not a simplified IR.**

The boundary is the `symbol_c*` tree root passed from stage1_2 → stage3 → stage4. However:
- It's the **full parse tree** (~323 node types), not a simplified intermediate representation
- Stage 3 **mutates the tree in-place**, adding annotations to the same nodes:
  - `symbol_c::candidate_datatypes` — possible types (vector)
  - `symbol_c::datatype` — resolved type (single pointer)
  - `symbol_c::const_value` — constant-folded value
  - `symbol_c::scope` — enclosing POU declaration
- The tree preserves all syntactic structure (comments stripped, but else/elseif nesting, operator precedence nodes, etc. all present)

The AST is **not serializable** — it's a C++ object graph with raw pointers, no serialization format exists.

### Q2: Can the backend be replaced?

**Yes, proven by the iec2iec backend.**

The architecture uses a factory pattern in `stage4/stage4.cc:258-269`:

```c
int stage4(symbol_c *tree_root, const char *builddir) {
  stage4out_c s4o;
  visitor_c *generate_code = new_code_generator(&s4o, builddir);
  tree_root->accept(*generate_code);
  delete_code_generator(generate_code);
  return 0;
}
```

`new_code_generator()` is a forward-declared function implemented differently by each backend:
- `generate_c/generate_c.cc` implements it for C output
- `generate_iec/generate_iec.cc` implements it for IEC output
- The linker selects which backend based on the target (`iec2c` vs `iec2iec`)

**Replacing stage4 requires**: implementing `visitor_c` with visit methods for all ~323 AST node types (though in practice only ~50-100 are needed for ST-only code).

### Q3: What does the lexer/parser cooperation look like?

Flex and Bison run as a **single cooperative pass**. Bison feeds symbol tables back to Flex so the lexer can return **context-sensitive tokens**:

- `identifier_c` is NOT used as a generic token
- Instead, Flex returns `variable_name_token`, `derived_function_name_token`, `derived_fb_name_token`, etc. based on whether the identifier was previously declared as a variable, function, FB, etc.
- This avoids ambiguity in the grammar but makes it impossible to parse IEC 61131-3 with a standard context-free parser

### Q4: What files does iec2c generate?

For a program `test_simple` in configuration `test_config` with resource `test_resource`:

| File | Purpose |
|------|---------|
| `POUS.h` | Program/FB data struct typedefs (`TEST_SIMPLE_data__`) |
| `POUS.c` | Program/FB init and body functions (`TEST_SIMPLE_init__`, `TEST_SIMPLE_body__`) |
| `test_config.c` | Configuration init/run, tick time |
| `test_config.h` | Configuration declarations |
| `test_resource.c` | Resource-level task scheduling, POU instantiation |
| `LOCATED_VARIABLES.h` | I/O mapped variable declarations (empty if none) |
| `VARIABLES.csv` | Full variable list with paths and types |

## Build System

```bash
autoreconf -i   # Generate configure from configure.ac
./configure      # Detect flex, bison (>= 2.4), g++
make             # Build iec2c and iec2iec
```

Compiler flags: `-g -Wall -Wpointer-arith -Wwrite-strings -Wno-unused -Wno-overloaded-virtual`
Stage1_2 adds: `-fpermissive -DYY_BUF_SIZE=65536`
