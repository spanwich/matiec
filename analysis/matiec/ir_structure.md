# DELIVERABLE 2: matiec IR Structure Deep-Dive

## AST Node Representation

### Class Hierarchy

All AST nodes inherit from `symbol_c` (defined in `absyntax/absyntax.hh`):

```
symbol_c                        # Base: location, annotations, accept()
├── token_c                     # Terminal: const char *value
│   └── SYM_TOKEN classes       # identifier_c, integer_c, real_c, etc.
├── list_c                      # Ordered list: elements[], count
│   └── SYM_LIST classes        # statement_list_c, var_declarations_list_c, etc.
└── SYM_REF0..6 classes         # Non-terminals with 0-6 child references
    ├── SYM_REF0               # Leaf non-terminals (boolean_true_c, exit_statement_c)
    ├── SYM_REF1               # 1 child (neg_real_c, var_declarations_c)
    ├── SYM_REF2               # 2 children (assignment_statement_c, while_statement_c)
    ├── SYM_REF3               # 3 children (program_declaration_c, fb_invocation_c)
    ├── SYM_REF4               # 4 children (if_statement_c, function_declaration_c)
    ├── SYM_REF5               # 5 children (for_statement_c, configuration_declaration_c)
    └── SYM_REF6               # 6 children (instance_specific_init_c)
```

### Node Count by Type

Total: **323 AST node classes** defined in `absyntax/absyntax.def`.

```
SYM_TOKEN:  ~25 classes  (terminals: identifiers, literals)
SYM_REF0:   ~40 classes  (leaf non-terminals: keywords, empty optionals)
SYM_REF1:   ~40 classes  (single-child wrappers)
SYM_REF2:   ~80 classes  (binary: assignment, while, declarations)
SYM_REF3:   ~50 classes  (ternary: program_decl, fb_invocation)
SYM_REF4:   ~35 classes  (if_statement, function_declaration)
SYM_REF5:   ~25 classes  (for_statement, configuration_declaration)
SYM_REF6:   ~10 classes  (instance_specific_init)
SYM_LIST:   ~20 classes  (statement lists, declaration lists)
```

### SYM_* Macro Expansion

The macros in `absyntax/absyntax.hh` generate classes automatically:

```c
// SYM_TOKEN(identifier_c) expands to:
class identifier_c: public token_c {
  public:
    identifier_c(const char *value, int fl=0, int fc=0, ...);
    virtual void *accept(visitor_c &visitor);
    virtual const char *absyntax_cname(void) {return "identifier_c";};
};

// SYM_REF2(assignment_statement_c, l_exp, r_exp) expands to:
class assignment_statement_c: public symbol_c {
  public:
    symbol_c *l_exp;
    symbol_c *r_exp;
    assignment_statement_c(symbol_c *l_exp=NULL, symbol_c *r_exp=NULL, ...);
    virtual void *accept(visitor_c &visitor);
    virtual const char *absyntax_cname(void) {return "assignment_statement_c";};
};

// SYM_LIST(statement_list_c) expands to:
class statement_list_c: public list_c {
  public:
    statement_list_c(...);
    statement_list_c(symbol_c *elem, ...);
    virtual void *accept(visitor_c &visitor);
    virtual const char *absyntax_cname(void) {return "statement_list_c";};
};
```

### Extra Fields in SYM_* Macros

Some nodes carry additional fields beyond the standard refs, declared via the variadic `__VA_ARGS__`:

```c
// fb_invocation_c has a called_fb_declaration pointer (filled by stage3)
SYM_REF3(fb_invocation_c, fb_name, formal_param_list, nonformal_param_list,
         symbol_c *called_fb_declaration;)

// function_invocation_c has extra stage3 annotations
SYM_REF3(function_invocation_c, function_name, formal_param_list, nonformal_param_list,
         symbol_c *called_function_declaration;
         int extensible_param_count;
         std::vector<symbol_c *> candidate_functions;)

// library_c and others have enum value tables
SYM_LIST(library_c, enumvalue_symtable_t enumvalue_symtable;)
```

## Base Class Annotations (`symbol_c`)

Every AST node carries these fields (from `absyntax/absyntax.hh:161-231`):

### Stage 1_2 Annotations (set during parsing)
```c
symbol_c *parent;           // Parent node in AST
token_c  *token;            // Identifying token (e.g., function name)
int first_line, first_column;
const char *first_file;
long int first_order;       // Lexical order (for reordering)
int last_line, last_column;
const char *last_file;
long int last_order;
```

### Stage 3 Annotations (set during semantic analysis)
```c
std::vector<symbol_c *> candidate_datatypes;  // All possible types
symbol_c *datatype;                            // Resolved type (or NULL/invalid)
symbol_c *scope;                               // Enclosing POU declaration
const_value_c const_value;                     // Constant-folded value
```

### Stage 4 Annotations (set during code generation)
```c
std::map<std::string, symbol_c *> anotations_map;  // Stage4-specific metadata
```

### Constant Value Structure
```c
class const_value_c {
    const_value__<int64_t>   _int64;   // Integer constant
    const_value__<uint64_t>  _uint64;  // Unsigned integer constant
    const_value__<real64_t>  _real64;  // Real constant
    const_value__<bool>      _bool;    // Boolean constant
    // Each has status: cs_undefined | cs_non_const | cs_const_value | cs_overflow
};
```

## Visitor Pattern

### Visitor Interface (`absyntax/visitor.hh`)

The visitor interface is generated by re-including `absyntax.def` with different macro definitions:

```c
class visitor_c {
  public:
    virtual void *visit(identifier_c *symbol) = 0;
    virtual void *visit(integer_c *symbol) = 0;
    virtual void *visit(assignment_statement_c *symbol) = 0;
    // ... 323 pure virtual visit() methods, one per AST class
    virtual ~visitor_c(void);
};
```

### Five Visitor Base Classes

| Class | Behavior | Used By |
|-------|----------|---------|
| `null_visitor_c` | All visit() return NULL (no-op) | Stage3 passes (override specific nodes) |
| `iterator_visitor_c` | Visits all children recursively | `fill_candidate_datatypes_c`, many stage3 passes |
| `search_visitor_c` | Stops at first non-NULL return | `search_var_instance_decl_c`, utility searches |
| `fcall_visitor_c` | Calls `fcall()` for every node | (rarely used) |
| `fcall_iterator_visitor_c` | Iterator + pre/suffix fcall hooks | (rarely used) |

**Pattern**: Stage3 and Stage4 passes extend `iterator_visitor_c` or `null_visitor_c`, overriding visit() methods for the specific node types they care about.

## Key AST Nodes for ST Programs

### Program Structure
```
SYM_REF3(program_declaration_c,
         program_type_name,          // identifier_c: "test_simple"
         var_declarations,           // var_declarations_list_c
         function_block_body)        // statement_list_c

SYM_REF5(configuration_declaration_c,
         configuration_name,         // identifier_c: "test_config"
         global_var_declarations,
         resource_declarations,
         access_declarations,
         instance_specific_initializations)

SYM_REF4(resource_declaration_c,
         resource_name,              // identifier_c: "test_resource"
         resource_type_name,         // identifier_c: "PLC"
         global_var_declarations,
         resource_declaration)       // single_resource_declaration_c
```

### Statements
```
SYM_REF2(assignment_statement_c,
         l_exp,                      // symbolic_variable_c or structured_variable_c
         r_exp)                      // expression tree

SYM_REF4(if_statement_c,
         expression,                 // condition expression
         statement_list,             // THEN branch (statement_list_c)
         elseif_statement_list,      // ELSIF branches (list, may be NULL)
         else_statement_list)        // ELSE branch (statement_list_c, may be NULL)

SYM_REF5(for_statement_c,
         control_variable,           // symbolic_variable_c
         beg_expression,             // start value
         end_expression,             // end value
         by_expression,              // step (may be NULL → default 1)
         statement_list)             // loop body

SYM_REF2(while_statement_c,
         expression,                 // condition
         statement_list)             // body

SYM_REF2(repeat_statement_c,
         statement_list,             // body (executed first)
         expression)                 // UNTIL condition

SYM_REF3(fb_invocation_c,
         fb_name,                    // identifier_c: "timer1"
         formal_param_list,          // param_assignment_list_c
         nonformal_param_list,       // may be NULL
         symbol_c *called_fb_declaration;)  // stage3: points to FB declaration
```

### Expressions
```
SYM_REF2(or_expression_c, l_exp, r_exp)
SYM_REF2(xor_expression_c, l_exp, r_exp)
SYM_REF2(and_expression_c, l_exp, r_exp)
SYM_REF1(not_expression_c, exp)
SYM_REF2(equ_expression_c, l_exp, r_exp)       // =
SYM_REF2(notequ_expression_c, l_exp, r_exp)    // <>
SYM_REF2(lt_expression_c, l_exp, r_exp)        // <
SYM_REF2(gt_expression_c, l_exp, r_exp)        // >
SYM_REF2(le_expression_c, l_exp, r_exp)        // <=
SYM_REF2(ge_expression_c, l_exp, r_exp)        // >=
SYM_REF2(add_expression_c, l_exp, r_exp)       // +
SYM_REF2(sub_expression_c, l_exp, r_exp)       // -
SYM_REF2(mul_expression_c, l_exp, r_exp)       // *
SYM_REF2(div_expression_c, l_exp, r_exp)       // /
SYM_REF2(mod_expression_c, l_exp, r_exp)       // MOD
SYM_REF1(neg_expression_c, exp)                // unary -
SYM_REF3(function_invocation_c,                // function call
         function_name, formal_param_list, nonformal_param_list, ...)
```

### Variables
```
SYM_REF2(symbolic_variable_c, var_name, unused)  // simple variable: "count"
SYM_REF2(structured_variable_c, record_variable, field_selector)  // "timer1.Q"
SYM_REF2(array_variable_c, subscripted_variable, subscript_list)  // "arr[i]"
```

### Type Names (used in `symbol_c::datatype`)
```
SYM_REF0(bool_type_name_c)
SYM_REF0(sint_type_name_c)
SYM_REF0(int_type_name_c)
SYM_REF0(dint_type_name_c)
SYM_REF0(lint_type_name_c)
SYM_REF0(usint_type_name_c)
SYM_REF0(uint_type_name_c)
SYM_REF0(udint_type_name_c)
SYM_REF0(ulint_type_name_c)
SYM_REF0(real_type_name_c)
SYM_REF0(lreal_type_name_c)
SYM_REF0(time_type_name_c)
SYM_REF0(byte_type_name_c)
SYM_REF0(word_type_name_c)
SYM_REF0(dword_type_name_c)
SYM_REF0(lword_type_name_c)
SYM_REF0(string_type_name_c)
```

## FB Instance Representation

Function Block instances are **typed structs** in the generated C, not type-erased:

```c
// In POUS.h (for a program using TON):
typedef struct {
    __DECLARE_VAR(BOOL,START_SIGNAL)
    __DECLARE_VAR(TIME,DELAY_TIME)
    TON_data__ TIMER1;              // FB instance is a nested struct
    __DECLARE_VAR(BOOL,DONE)
    __DECLARE_VAR(TIME,ELAPSED)
} TEST_TIMER_data__;
```

The `TON_data__` struct (from `lib/C/iec_std_FB.h:389-406`):
```c
typedef struct {
    // Interface
    __DECLARE_VAR(BOOL,EN)      // Enable (auto-generated)
    __DECLARE_VAR(BOOL,ENO)     // Enable Out (auto-generated)
    __DECLARE_VAR(BOOL,IN)      // Timer input
    __DECLARE_VAR(TIME,PT)      // Preset time
    __DECLARE_VAR(BOOL,Q)       // Output: timer done
    __DECLARE_VAR(TIME,ET)      // Output: elapsed time
    // Private
    __DECLARE_VAR(SINT,STATE)   // State machine: 0=idle, 1=timing, 2=done
    __DECLARE_VAR(BOOL,PREV_IN) // Previous IN value (edge detection)
    __DECLARE_VAR(TIME,CURRENT_TIME)
    __DECLARE_VAR(TIME,START_TIME)
} TON_data__;
```

## AST Boundary: Same Tree, Annotations Added

The critical observation for LowPLC: **there is no distinct IR**. The same `symbol_c*` tree is:
1. **Built** by stage1_2 (parser constructs nodes, sets location fields)
2. **Annotated** by stage3 (semantic passes fill `datatype`, `candidate_datatypes`, `const_value`, `scope`, and node-specific fields like `called_fb_declaration`)
3. **Traversed** by stage4 (code generator reads the annotated tree)

This means consuming matiec's output would require either:
- Serializing the full ~323-class annotated parse tree (no format exists)
- Writing a new stage4 backend that emits a simpler serialization format
- Or bypassing matiec entirely and working from source

## Variable Access Macros

All variable access in generated C goes through macros (from `lib/C/accessor.h`):

```c
// __DECLARE_VAR(type, name) generates a struct with force-flag support:
//   struct { type value; uint8_t flags; } name;

// Reading: checks force flag, returns forced or actual value
#define __GET_VAR(var, ...) var.value

// Writing: checks force flag, skips write if forced
#define __SET_VAR(prefix, var, suffix, value) prefix var suffix.value = value
```

The force-flag mechanism supports runtime forcing of variables (a PLC debugging feature). For LowPLC, this can be ignored — the underlying access pattern is simply struct field read/write.
