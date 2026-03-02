#!/usr/bin/env python3
"""
lowplcst_parser.py — LowPLC-ST to TLV compiler.

Translates a LowPLC-ST source file (.st) into TLV binary (.tlv) for the
verified interpreter.  Architecture:

    Source text (.st)
      → Lexer (tokenize)
      → Parser (recursive descent → AST)
      → Type checker (check against type_env)
      → TLV emitter (walk AST → bytes)
      → TLV binary (.tlv)

Part of deliverable D4 of the LowPLC project.
"""

import argparse
import re
import struct
import sys
from dataclasses import dataclass, field
from enum import Enum, auto
from typing import List, Optional, Tuple, Dict, Union

# ══════════════════════════════════════════════════════════════════════
# Section 1: Token Definitions
# ══════════════════════════════════════════════════════════════════════

class TT(Enum):
    """Token types."""
    # Literals
    INT_LIT   = auto()
    BOOL_LIT  = auto()
    TIME_LIT  = auto()
    IDENT     = auto()

    # Keywords
    PROGRAM   = auto()
    END_PROGRAM = auto()
    VAR       = auto()
    END_VAR   = auto()
    IF        = auto()
    THEN      = auto()
    ELSIF     = auto()
    ELSE      = auto()
    END_IF    = auto()
    FOR       = auto()
    TO        = auto()
    DO        = auto()
    END_FOR   = auto()
    CONFIGURATION = auto()
    END_CONFIGURATION = auto()
    RESOURCE  = auto()
    END_RESOURCE = auto()
    ON        = auto()
    TASK      = auto()
    WITH      = auto()
    INTERVAL  = auto()
    PRIORITY  = auto()

    # Types
    BOOL      = auto()
    INT       = auto()
    TIME      = auto()
    # FB types
    SR        = auto()
    RS        = auto()
    R_TRIG    = auto()
    F_TRIG    = auto()
    TON       = auto()
    TOF       = auto()
    TP        = auto()
    CTU       = auto()
    CTD       = auto()
    CTUD      = auto()

    # Operators
    AND       = auto()
    OR        = auto()
    NOT       = auto()
    XOR       = auto()
    TRUE      = auto()
    FALSE     = auto()

    # Symbols
    ASSIGN    = auto()   # :=
    COLON     = auto()   # :
    SEMI      = auto()   # ;
    LPAREN    = auto()   # (
    RPAREN    = auto()   # )
    DOT       = auto()   # .
    COMMA     = auto()   # ,
    PLUS      = auto()   # +
    MINUS     = auto()   # -
    STAR      = auto()   # *
    EQ        = auto()   # =
    NE        = auto()   # <>
    LT        = auto()   # <
    GT        = auto()   # >
    LE        = auto()   # <=
    GE        = auto()   # >=

    EOF       = auto()

@dataclass
class Token:
    type: TT
    value: object
    line: int
    col: int

    def __repr__(self):
        return f"Token({self.type.name}, {self.value!r}, L{self.line}:{self.col})"

# ══════════════════════════════════════════════════════════════════════
# Section 2: Lexer
# ══════════════════════════════════════════════════════════════════════

# Keywords map (case-insensitive)
KEYWORDS = {
    'PROGRAM': TT.PROGRAM, 'END_PROGRAM': TT.END_PROGRAM,
    'VAR': TT.VAR, 'END_VAR': TT.END_VAR,
    'IF': TT.IF, 'THEN': TT.THEN, 'ELSIF': TT.ELSIF,
    'ELSE': TT.ELSE, 'END_IF': TT.END_IF,
    'FOR': TT.FOR, 'TO': TT.TO, 'DO': TT.DO, 'END_FOR': TT.END_FOR,
    'CONFIGURATION': TT.CONFIGURATION, 'END_CONFIGURATION': TT.END_CONFIGURATION,
    'RESOURCE': TT.RESOURCE, 'END_RESOURCE': TT.END_RESOURCE,
    'ON': TT.ON, 'TASK': TT.TASK, 'WITH': TT.WITH,
    'INTERVAL': TT.INTERVAL, 'PRIORITY': TT.PRIORITY,
    'BOOL': TT.BOOL, 'INT': TT.INT, 'TIME': TT.TIME,
    'SR': TT.SR, 'RS': TT.RS,
    'R_TRIG': TT.R_TRIG, 'F_TRIG': TT.F_TRIG,
    'TON': TT.TON, 'TOF': TT.TOF, 'TP': TT.TP,
    'CTU': TT.CTU, 'CTD': TT.CTD, 'CTUD': TT.CTUD,
    'AND': TT.AND, 'OR': TT.OR, 'NOT': TT.NOT, 'XOR': TT.XOR,
    'TRUE': TT.TRUE, 'FALSE': TT.FALSE,
}

def parse_time_literal(s: str) -> int:
    """Parse a TIME literal like T#3s, T#10ms, T#1m30s into milliseconds."""
    s = s.upper()
    if not s.startswith('T#'):
        raise ValueError(f"Invalid TIME literal: {s}")
    body = s[2:]
    total_ms = 0
    # Match components: Xd, Xh, Xm, Xs, Xms
    pattern = re.compile(r'(\d+)(MS|M|S|H|D)', re.IGNORECASE)
    pos = 0
    for m in pattern.finditer(body):
        if m.start() != pos:
            raise ValueError(f"Invalid TIME literal: {s}")
        val = int(m.group(1))
        unit = m.group(2).upper()
        if unit == 'MS':
            total_ms += val
        elif unit == 'S':
            total_ms += val * 1000
        elif unit == 'M':
            total_ms += val * 60_000
        elif unit == 'H':
            total_ms += val * 3_600_000
        elif unit == 'D':
            total_ms += val * 86_400_000
        pos = m.end()
    if pos != len(body):
        raise ValueError(f"Invalid TIME literal: {s}")
    return total_ms


class Lexer:
    """Case-insensitive lexer for LowPLC-ST with nested comment support."""

    def __init__(self, source: str, filename: str = "<stdin>"):
        self.source = source
        self.filename = filename
        self.pos = 0
        self.line = 1
        self.col = 1
        self.tokens: List[Token] = []

    def error(self, msg: str):
        raise SyntaxError(f"{self.filename}:{self.line}:{self.col}: {msg}")

    def peek(self) -> str:
        if self.pos >= len(self.source):
            return ''
        return self.source[self.pos]

    def advance(self) -> str:
        ch = self.source[self.pos]
        self.pos += 1
        if ch == '\n':
            self.line += 1
            self.col = 1
        else:
            self.col += 1
        return ch

    def skip_whitespace_and_comments(self):
        while self.pos < len(self.source):
            # Whitespace
            if self.source[self.pos] in ' \t\r\n':
                self.advance()
                continue
            # Nested comments (* ... *)
            if self.pos + 1 < len(self.source) and self.source[self.pos:self.pos+2] == '(*':
                self.advance()  # (
                self.advance()  # *
                depth = 1
                while depth > 0 and self.pos < len(self.source):
                    if self.pos + 1 < len(self.source) and self.source[self.pos:self.pos+2] == '(*':
                        self.advance()
                        self.advance()
                        depth += 1
                    elif self.pos + 1 < len(self.source) and self.source[self.pos:self.pos+2] == '*)':
                        self.advance()
                        self.advance()
                        depth -= 1
                    else:
                        self.advance()
                if depth > 0:
                    self.error("Unterminated comment")
                continue
            break

    def read_number(self) -> int:
        start = self.pos
        while self.pos < len(self.source) and self.source[self.pos].isdigit():
            self.pos += 1
            self.col += 1
        return int(self.source[start:self.pos])

    def read_ident_or_keyword(self) -> Token:
        start = self.pos
        line, col = self.line, self.col
        while self.pos < len(self.source) and (self.source[self.pos].isalnum() or self.source[self.pos] == '_'):
            self.pos += 1
            self.col += 1
        text = self.source[start:self.pos]
        upper = text.upper()

        # Check for TIME literal: T#...
        if upper == 'T' and self.pos < len(self.source) and self.source[self.pos] == '#':
            self.pos += 1
            self.col += 1
            tstart = self.pos
            while self.pos < len(self.source) and (self.source[self.pos].isalnum()):
                self.pos += 1
                self.col += 1
            time_str = 'T#' + self.source[tstart:self.pos]
            ms = parse_time_literal(time_str)
            return Token(TT.TIME_LIT, ms, line, col)

        if upper == 'TRUE':
            return Token(TT.BOOL_LIT, True, line, col)
        if upper == 'FALSE':
            return Token(TT.BOOL_LIT, False, line, col)

        kw = KEYWORDS.get(upper)
        if kw is not None:
            return Token(kw, upper, line, col)
        return Token(TT.IDENT, text, line, col)

    def tokenize(self) -> List[Token]:
        while True:
            self.skip_whitespace_and_comments()
            if self.pos >= len(self.source):
                self.tokens.append(Token(TT.EOF, None, self.line, self.col))
                break

            line, col = self.line, self.col
            ch = self.source[self.pos]

            # Two-char operators
            if self.pos + 1 < len(self.source):
                two = self.source[self.pos:self.pos+2]
                if two == ':=':
                    self.pos += 2; self.col += 2
                    self.tokens.append(Token(TT.ASSIGN, ':=', line, col))
                    continue
                if two == '<>':
                    self.pos += 2; self.col += 2
                    self.tokens.append(Token(TT.NE, '<>', line, col))
                    continue
                if two == '<=':
                    self.pos += 2; self.col += 2
                    self.tokens.append(Token(TT.LE, '<=', line, col))
                    continue
                if two == '>=':
                    self.pos += 2; self.col += 2
                    self.tokens.append(Token(TT.GE, '>=', line, col))
                    continue

            # Single-char
            simple = {
                ':': TT.COLON, ';': TT.SEMI, '(': TT.LPAREN, ')': TT.RPAREN,
                '.': TT.DOT, ',': TT.COMMA, '+': TT.PLUS, '-': TT.MINUS,
                '*': TT.STAR, '=': TT.EQ, '<': TT.LT, '>': TT.GT,
            }
            if ch in simple:
                self.advance()
                self.tokens.append(Token(simple[ch], ch, line, col))
                continue

            # Numbers
            if ch.isdigit():
                val = self.read_number()
                self.tokens.append(Token(TT.INT_LIT, val, line, col))
                continue

            # Identifiers / keywords / TIME literals
            if ch.isalpha() or ch == '_':
                tok = self.read_ident_or_keyword()
                self.tokens.append(tok)
                continue

            self.error(f"Unexpected character: {ch!r}")

        return self.tokens


# ══════════════════════════════════════════════════════════════════════
# Section 3: AST Node Definitions
# ══════════════════════════════════════════════════════════════════════

class ExprType(Enum):
    BOOL = auto()
    INT  = auto()
    TIME = auto()

@dataclass
class ExprLitBool:
    value: bool
    etype: ExprType = ExprType.BOOL

@dataclass
class ExprLitInt:
    value: int
    etype: ExprType = ExprType.INT

@dataclass
class ExprLitTime:
    value_ms: int
    etype: ExprType = ExprType.TIME

@dataclass
class ExprVar:
    name: str
    etype: Optional[ExprType] = None

@dataclass
class ExprFieldAccess:
    obj: str    # FB instance name
    field: str  # Q, Q1, CV, QU, QD
    etype: Optional[ExprType] = None

@dataclass
class ExprBinOp:
    op: str     # AND, OR, XOR, +, -, *, =, <>, <, >, <=, >=
    left: 'Expr'
    right: 'Expr'
    etype: Optional[ExprType] = None

@dataclass
class ExprUnaryOp:
    op: str     # NOT, UNARY_MINUS
    operand: 'Expr'
    etype: Optional[ExprType] = None

Expr = Union[ExprLitBool, ExprLitInt, ExprLitTime, ExprVar,
             ExprFieldAccess, ExprBinOp, ExprUnaryOp]

@dataclass
class StmtAssign:
    target: str
    expr: Expr

@dataclass
class FBArg:
    name: str
    expr: Expr

@dataclass
class StmtFBCall:
    instance: str
    args: List[FBArg]

@dataclass
class ElsifClause:
    condition: Expr
    body: List['Stmt']

@dataclass
class StmtIf:
    condition: Expr
    then_body: List['Stmt']
    elsifs: List[ElsifClause]
    else_body: List['Stmt']

@dataclass
class StmtFor:
    var: str
    start: Expr
    end: Expr
    body: List['Stmt']

Stmt = Union[StmtAssign, StmtFBCall, StmtIf, StmtFor]

@dataclass
class VarDecl:
    name: str
    type_name: str      # BOOL, INT, TIME, SR, RS, etc.
    init_value: Optional[Expr] = None

@dataclass
class TaskDecl:
    name: str
    interval_ms: int
    priority: int

@dataclass
class ProgramBinding:
    instance_name: str
    task_name: str
    program_name: str

@dataclass
class ProgramAST:
    name: str
    vars: List[VarDecl]
    body: List[Stmt]
    task: Optional[TaskDecl] = None
    binding: Optional[ProgramBinding] = None


# ══════════════════════════════════════════════════════════════════════
# Section 4: Parser (Recursive Descent)
# ══════════════════════════════════════════════════════════════════════

FB_TYPES = {'SR', 'RS', 'R_TRIG', 'F_TRIG', 'TON', 'TOF', 'TP',
            'CTU', 'CTD', 'CTUD'}

class Parser:
    """Recursive descent parser for LowPLC-ST."""

    def __init__(self, tokens: List[Token], filename: str = "<stdin>"):
        self.tokens = tokens
        self.filename = filename
        self.pos = 0

    def error(self, msg: str):
        tok = self.current()
        raise SyntaxError(f"{self.filename}:{tok.line}:{tok.col}: {msg}")

    def current(self) -> Token:
        return self.tokens[self.pos]

    def peek_type(self) -> TT:
        return self.tokens[self.pos].type

    def advance(self) -> Token:
        tok = self.tokens[self.pos]
        self.pos += 1
        return tok

    def expect(self, tt: TT) -> Token:
        tok = self.current()
        if tok.type != tt:
            self.error(f"Expected {tt.name}, got {tok.type.name} ({tok.value!r})")
        return self.advance()

    def match(self, tt: TT) -> Optional[Token]:
        if self.current().type == tt:
            return self.advance()
        return None

    def at(self, tt: TT) -> bool:
        return self.current().type == tt

    def ident_upper(self) -> str:
        """Return the current IDENT value uppercased and advance."""
        tok = self.expect(TT.IDENT)
        return tok.value.upper()

    # ── Top-level ──

    def parse(self) -> ProgramAST:
        prog = self.parse_program()
        if self.at(TT.CONFIGURATION):
            self.parse_configuration(prog)
        self.expect(TT.EOF)
        return prog

    def parse_program(self) -> ProgramAST:
        self.expect(TT.PROGRAM)
        name = self.expect(TT.IDENT).value
        vars_ = self.parse_var_block()
        body = self.parse_stmt_list()
        self.expect(TT.END_PROGRAM)
        return ProgramAST(name=name, vars=vars_, body=body)

    def parse_var_block(self) -> List[VarDecl]:
        self.expect(TT.VAR)
        decls = []
        while not self.at(TT.END_VAR):
            decls.append(self.parse_var_decl())
        self.expect(TT.END_VAR)
        return decls

    def parse_var_decl(self) -> VarDecl:
        name = self.expect(TT.IDENT).value
        self.expect(TT.COLON)
        type_name = self.parse_type_name()
        init = None
        if self.match(TT.ASSIGN):
            init = self.parse_expr()
        self.expect(TT.SEMI)
        return VarDecl(name=name, type_name=type_name, init_value=init)

    def parse_type_name(self) -> str:
        tok = self.current()
        type_tokens = {TT.BOOL: 'BOOL', TT.INT: 'INT', TT.TIME: 'TIME',
                       TT.SR: 'SR', TT.RS: 'RS',
                       TT.R_TRIG: 'R_TRIG', TT.F_TRIG: 'F_TRIG',
                       TT.TON: 'TON', TT.TOF: 'TOF', TT.TP: 'TP',
                       TT.CTU: 'CTU', TT.CTD: 'CTD', TT.CTUD: 'CTUD'}
        if tok.type in type_tokens:
            self.advance()
            return type_tokens[tok.type]
        self.error(f"Expected type name, got {tok.type.name}")

    # ── Statements ──

    def parse_stmt_list(self) -> List[Stmt]:
        """Parse statements until we hit END_PROGRAM, END_IF, END_FOR, ELSIF, ELSE."""
        stmts = []
        stop = {TT.END_PROGRAM, TT.END_IF, TT.END_FOR, TT.ELSIF, TT.ELSE, TT.EOF}
        while self.peek_type() not in stop:
            stmts.append(self.parse_stmt())
        return stmts

    def parse_stmt(self) -> Stmt:
        if self.at(TT.IF):
            return self.parse_if()
        if self.at(TT.FOR):
            return self.parse_for()
        # Assignment or FB call: starts with IDENT
        if self.at(TT.IDENT):
            return self.parse_assign_or_fb_call()
        self.error(f"Expected statement, got {self.current().type.name}")

    def parse_assign_or_fb_call(self) -> Stmt:
        name_tok = self.expect(TT.IDENT)
        name = name_tok.value

        # FB call: ident '(' args ')'
        if self.at(TT.LPAREN):
            self.advance()
            args = []
            while not self.at(TT.RPAREN):
                arg_name = self.expect(TT.IDENT).value
                self.expect(TT.ASSIGN)
                arg_expr = self.parse_expr()
                args.append(FBArg(name=arg_name.upper(), expr=arg_expr))
                self.match(TT.COMMA)
            self.expect(TT.RPAREN)
            self.expect(TT.SEMI)
            return StmtFBCall(instance=name, args=args)

        # Assignment: ident ':=' expr ';'
        self.expect(TT.ASSIGN)
        expr = self.parse_expr()
        self.expect(TT.SEMI)
        return StmtAssign(target=name, expr=expr)

    def parse_if(self) -> StmtIf:
        self.expect(TT.IF)
        cond = self.parse_expr()
        self.expect(TT.THEN)
        then_body = self.parse_stmt_list()

        elsifs = []
        while self.at(TT.ELSIF):
            self.advance()
            econd = self.parse_expr()
            self.expect(TT.THEN)
            ebody = self.parse_stmt_list()
            elsifs.append(ElsifClause(condition=econd, body=ebody))

        else_body = []
        if self.match(TT.ELSE):
            else_body = self.parse_stmt_list()

        self.expect(TT.END_IF)
        self.expect(TT.SEMI)
        return StmtIf(condition=cond, then_body=then_body,
                      elsifs=elsifs, else_body=else_body)

    def parse_for(self) -> StmtFor:
        self.expect(TT.FOR)
        var = self.expect(TT.IDENT).value
        self.expect(TT.ASSIGN)
        start = self.parse_expr()
        self.expect(TT.TO)
        end = self.parse_expr()
        self.expect(TT.DO)
        body = self.parse_stmt_list()
        self.expect(TT.END_FOR)
        self.expect(TT.SEMI)
        return StmtFor(var=var, start=start, end=end, body=body)

    # ── Expressions (precedence climbing) ──
    # Precedence (low to high):
    #   1: OR
    #   2: XOR
    #   3: AND
    #   4: NOT (unary prefix)
    #   5: = <> < > <= >=
    #   6: + -
    #   7: * (only MUL; no DIV/MOD in LowPLC-ST)
    #   8: unary - , atoms

    def parse_expr(self) -> Expr:
        return self.parse_or()

    def parse_or(self) -> Expr:
        left = self.parse_xor()
        while self.at(TT.OR):
            self.advance()
            right = self.parse_xor()
            left = ExprBinOp(op='OR', left=left, right=right)
        return left

    def parse_xor(self) -> Expr:
        left = self.parse_and()
        while self.at(TT.XOR):
            self.advance()
            right = self.parse_and()
            left = ExprBinOp(op='XOR', left=left, right=right)
        return left

    def parse_and(self) -> Expr:
        left = self.parse_not()
        while self.at(TT.AND):
            self.advance()
            right = self.parse_not()
            left = ExprBinOp(op='AND', left=left, right=right)
        return left

    def parse_not(self) -> Expr:
        if self.at(TT.NOT):
            self.advance()
            operand = self.parse_not()
            return ExprUnaryOp(op='NOT', operand=operand)
        return self.parse_comparison()

    def parse_comparison(self) -> Expr:
        left = self.parse_add()
        cmp_ops = {TT.EQ: '=', TT.NE: '<>', TT.LT: '<', TT.GT: '>',
                   TT.LE: '<=', TT.GE: '>='}
        if self.peek_type() in cmp_ops:
            op = cmp_ops[self.peek_type()]
            self.advance()
            right = self.parse_add()
            left = ExprBinOp(op=op, left=left, right=right)
        return left

    def parse_add(self) -> Expr:
        left = self.parse_mul()
        while self.peek_type() in (TT.PLUS, TT.MINUS):
            op = '+' if self.peek_type() == TT.PLUS else '-'
            self.advance()
            right = self.parse_mul()
            left = ExprBinOp(op=op, left=left, right=right)
        return left

    def parse_mul(self) -> Expr:
        left = self.parse_unary()
        while self.at(TT.STAR):
            self.advance()
            right = self.parse_unary()
            left = ExprBinOp(op='*', left=left, right=right)
        return left

    def parse_unary(self) -> Expr:
        if self.at(TT.MINUS):
            self.advance()
            operand = self.parse_unary()
            return ExprUnaryOp(op='UNARY_MINUS', operand=operand)
        return self.parse_atom()

    def parse_atom(self) -> Expr:
        tok = self.current()

        if tok.type == TT.INT_LIT:
            self.advance()
            return ExprLitInt(value=tok.value)

        if tok.type == TT.BOOL_LIT:
            self.advance()
            return ExprLitBool(value=tok.value)

        if tok.type == TT.TIME_LIT:
            self.advance()
            return ExprLitTime(value_ms=tok.value)

        if tok.type == TT.IDENT:
            name = tok.value
            self.advance()
            # Field access: ident.field
            if self.at(TT.DOT):
                self.advance()
                field_tok = self.current()
                # Field names might be keywords (Q is not a keyword, but
                # Q1, CV, QU, QD are identifiers)
                if field_tok.type == TT.IDENT:
                    field_name = field_tok.value
                    self.advance()
                else:
                    self.error(f"Expected field name after '.', got {field_tok.type.name}")
                return ExprFieldAccess(obj=name, field=field_name)
            return ExprVar(name=name)

        if tok.type == TT.LPAREN:
            self.advance()
            expr = self.parse_expr()
            self.expect(TT.RPAREN)
            return expr

        self.error(f"Expected expression, got {tok.type.name} ({tok.value!r})")

    # ── Configuration block ──

    def parse_configuration(self, prog: ProgramAST):
        self.expect(TT.CONFIGURATION)
        _config_name = self.expect(TT.IDENT).value
        self.expect(TT.RESOURCE)
        _res_name = self.expect(TT.IDENT).value
        self.expect(TT.ON)
        _plc_name = self.expect(TT.IDENT).value

        # TASK task_name(INTERVAL := T#Xms, PRIORITY := N);
        self.expect(TT.TASK)
        task_name = self.expect(TT.IDENT).value
        self.expect(TT.LPAREN)
        interval_ms = 0
        priority = 0
        while not self.at(TT.RPAREN):
            param = self.current()
            if param.type == TT.INTERVAL:
                self.advance()
                self.expect(TT.ASSIGN)
                t = self.expect(TT.TIME_LIT)
                interval_ms = t.value
            elif param.type == TT.PRIORITY:
                self.advance()
                self.expect(TT.ASSIGN)
                priority = self.expect(TT.INT_LIT).value
            else:
                self.error(f"Expected INTERVAL or PRIORITY, got {param.type.name}")
            self.match(TT.COMMA)
        self.expect(TT.RPAREN)
        self.expect(TT.SEMI)
        prog.task = TaskDecl(name=task_name, interval_ms=interval_ms, priority=priority)

        # PROGRAM inst WITH task : program_name;
        self.expect(TT.PROGRAM)
        inst_name = self.expect(TT.IDENT).value
        self.expect(TT.WITH)
        ref_task = self.expect(TT.IDENT).value
        self.expect(TT.COLON)
        prog_type = self.expect(TT.IDENT).value
        self.expect(TT.SEMI)
        prog.binding = ProgramBinding(instance_name=inst_name,
                                       task_name=ref_task,
                                       program_name=prog_type)

        self.expect(TT.END_RESOURCE)
        self.expect(TT.END_CONFIGURATION)


# ══════════════════════════════════════════════════════════════════════
# Section 5: Type Checker
# ══════════════════════════════════════════════════════════════════════

# FB output types: field_name -> ExprType
FB_OUTPUT_TYPES = {
    'SR':     {'Q1': ExprType.BOOL},
    'RS':     {'Q1': ExprType.BOOL},
    'R_TRIG': {'Q': ExprType.BOOL},
    'F_TRIG': {'Q': ExprType.BOOL},
    'TON':    {'Q': ExprType.BOOL, 'ET': ExprType.TIME},
    'TOF':    {'Q': ExprType.BOOL, 'ET': ExprType.TIME},
    'TP':     {'Q': ExprType.BOOL, 'ET': ExprType.TIME},
    'CTU':    {'Q': ExprType.BOOL, 'CV': ExprType.INT},
    'CTD':    {'Q': ExprType.BOOL, 'CV': ExprType.INT},
    'CTUD':   {'QU': ExprType.BOOL, 'QD': ExprType.BOOL, 'CV': ExprType.INT},
}


class TypeChecker:
    """Simple type checker for LowPLC-ST programs."""

    def __init__(self, prog: ProgramAST, filename: str = "<stdin>"):
        self.prog = prog
        self.filename = filename
        self.var_types: Dict[str, ExprType] = {}
        self.fb_instances: Dict[str, str] = {}  # name -> FB type
        self.errors: List[str] = []

    def error(self, msg: str):
        self.errors.append(msg)

    def check(self):
        # Build type environment from declarations
        for v in self.prog.vars:
            upper_type = v.type_name.upper()
            upper_name = v.name.upper()
            if upper_type in FB_TYPES:
                self.fb_instances[upper_name] = upper_type
            elif upper_type == 'BOOL':
                self.var_types[upper_name] = ExprType.BOOL
            elif upper_type == 'INT':
                self.var_types[upper_name] = ExprType.INT
            elif upper_type == 'TIME':
                self.var_types[upper_name] = ExprType.TIME
            else:
                self.error(f"Unknown type: {v.type_name}")

        # Type-check body
        for stmt in self.prog.body:
            self.check_stmt(stmt)

        if self.errors:
            raise TypeError('\n'.join(self.errors))

    def check_stmt(self, stmt: Stmt):
        if isinstance(stmt, StmtAssign):
            expr_type = self.infer_expr(stmt.expr)
            target_upper = stmt.target.upper()
            if target_upper not in self.var_types:
                self.error(f"Undeclared variable: {stmt.target}")
                return
            target_type = self.var_types[target_upper]
            if expr_type is not None and target_type != expr_type:
                self.error(f"Type mismatch in assignment to {stmt.target}: "
                          f"expected {target_type.name}, got {expr_type.name}")

        elif isinstance(stmt, StmtFBCall):
            inst_upper = stmt.instance.upper()
            if inst_upper not in self.fb_instances:
                self.error(f"Undeclared FB instance: {stmt.instance}")
                return
            # Type-check arguments
            for arg in stmt.args:
                self.infer_expr(arg.expr)

        elif isinstance(stmt, StmtIf):
            cond_type = self.infer_expr(stmt.condition)
            if cond_type is not None and cond_type != ExprType.BOOL:
                self.error(f"IF condition must be BOOL, got {cond_type.name}")
            for s in stmt.then_body:
                self.check_stmt(s)
            for elsif in stmt.elsifs:
                et = self.infer_expr(elsif.condition)
                if et is not None and et != ExprType.BOOL:
                    self.error(f"ELSIF condition must be BOOL, got {et.name}")
                for s in elsif.body:
                    self.check_stmt(s)
            for s in stmt.else_body:
                self.check_stmt(s)

        elif isinstance(stmt, StmtFor):
            var_upper = stmt.var.upper()
            if var_upper not in self.var_types:
                self.error(f"Undeclared FOR variable: {stmt.var}")
            elif self.var_types[var_upper] != ExprType.INT:
                self.error(f"FOR variable must be INT")
            self.infer_expr(stmt.start)
            self.infer_expr(stmt.end)
            for s in stmt.body:
                self.check_stmt(s)

    def infer_expr(self, expr: Expr) -> Optional[ExprType]:
        if isinstance(expr, ExprLitBool):
            expr.etype = ExprType.BOOL
            return ExprType.BOOL
        if isinstance(expr, ExprLitInt):
            expr.etype = ExprType.INT
            return ExprType.INT
        if isinstance(expr, ExprLitTime):
            expr.etype = ExprType.TIME
            return ExprType.TIME
        if isinstance(expr, ExprVar):
            upper = expr.name.upper()
            if upper in self.var_types:
                expr.etype = self.var_types[upper]
                return expr.etype
            self.error(f"Undeclared variable: {expr.name}")
            return None
        if isinstance(expr, ExprFieldAccess):
            obj_upper = expr.obj.upper()
            field_upper = expr.field.upper()
            if obj_upper not in self.fb_instances:
                self.error(f"Undeclared FB instance: {expr.obj}")
                return None
            fb_type = self.fb_instances[obj_upper]
            outputs = FB_OUTPUT_TYPES.get(fb_type, {})
            if field_upper not in outputs:
                self.error(f"FB {fb_type} has no output '{expr.field}'")
                return None
            expr.etype = outputs[field_upper]
            return expr.etype
        if isinstance(expr, ExprBinOp):
            lt = self.infer_expr(expr.left)
            rt = self.infer_expr(expr.right)
            if expr.op in ('AND', 'OR', 'XOR'):
                expr.etype = ExprType.BOOL
                return ExprType.BOOL
            if expr.op in ('=', '<>', '<', '>', '<=', '>='):
                expr.etype = ExprType.BOOL
                return ExprType.BOOL
            if expr.op in ('+', '-', '*'):
                expr.etype = ExprType.INT
                return ExprType.INT
            return None
        if isinstance(expr, ExprUnaryOp):
            ot = self.infer_expr(expr.operand)
            if expr.op == 'NOT':
                expr.etype = ExprType.BOOL
                return ExprType.BOOL
            if expr.op == 'UNARY_MINUS':
                expr.etype = ExprType.INT
                return ExprType.INT
            return None
        return None


# ══════════════════════════════════════════════════════════════════════
# Section 6: TLV Emitter
# ══════════════════════════════════════════════════════════════════════

# Opcode tags
TAG_LOAD_TRUE   = 0x01
TAG_LOAD_FALSE  = 0x02
TAG_LOAD_INT    = 0x03
TAG_BOOL_NOT    = 0x10
TAG_BOOL_AND    = 0x11
TAG_BOOL_OR     = 0x12
TAG_BOOL_XOR    = 0x13
TAG_COPY_BOOL   = 0x14
TAG_INT_ADD     = 0x20
TAG_INT_SUB     = 0x21
TAG_INT_MUL     = 0x22
TAG_COPY_INT    = 0x23
TAG_CMP_EQ      = 0x30
TAG_CMP_NE      = 0x31
TAG_CMP_LT      = 0x32
TAG_CMP_GT      = 0x33
TAG_CMP_LE      = 0x34
TAG_CMP_GE      = 0x35
TAG_IF_BLOCK    = 0x40
TAG_FOR_BLOCK   = 0x41
TAG_TON_CALL    = 0x50
TAG_TOF_CALL    = 0x51
TAG_TP_CALL     = 0x52
TAG_CTU_CALL    = 0x60
TAG_CTD_CALL    = 0x61
TAG_CTUD_CALL   = 0x62
TAG_SR_CALL     = 0x70
TAG_RS_CALL     = 0x71
TAG_R_TRIG_CALL = 0x72
TAG_F_TRIG_CALL = 0x73


def wrap_i16(x: int) -> int:
    """Wrap integer to signed 16-bit range (DDR-003)."""
    return ((x + 32768) % 65536) - 32768


class TLVEmitter:
    """Walks the AST and emits TLV binary.

    Scratch allocation strategy: for bool binary ops (AND/OR/XOR), when one
    operand is a direct reference (variable or FB output) and the other is
    complex, the complex side is emitted directly to the destination slot,
    then the binary op reads from both the direct source and the destination.
    This minimizes scratch usage and matches the hand-encoded B1 TLV.

    When the LEFT operand is complex and RIGHT is direct: allocate one scratch
    for the left, then OP dst, scratch, right_direct.  When LEFT is direct and
    RIGHT is complex: emit right to dst, then OP dst, left_direct, dst.
    """

    def __init__(self, prog: ProgramAST, filename: str = "<stdin>"):
        self.prog = prog
        self.filename = filename

        # Pool indices
        self.bool_vars: Dict[str, int] = {}  # var_name_upper -> bool pool index
        self.int_vars: Dict[str, int] = {}   # var_name_upper -> int pool index
        self.fb_instances: Dict[str, Tuple[str, int]] = {}  # name_upper -> (type, index)

        # FB instance counts
        self.n_ton = 0
        self.n_tof = 0
        self.n_tp = 0
        self.n_ctu = 0
        self.n_ctd = 0
        self.n_ctud = 0
        self.n_sr = 0
        self.n_rs = 0
        self.n_rtrig = 0
        self.n_ftrig = 0

        # Scratch allocator base (after declared vars + FB outputs)
        self._scratch_bool_base = 0
        self._scratch_int_base = 0
        # Per-statement scratch counter
        self._bool_scratch_counter = 0
        self._int_scratch_counter = 0
        # Track max scratch index used across all statements
        self._max_bool_used = 0
        self._max_int_used = 0

        # FB output scratch slots
        self.fb_output_slots: Dict[str, Dict[str, Tuple[str, int]]] = {}
        # fb_name_upper -> {field_upper -> ('bool'|'int', pool_idx)}

        # Tick interval (ms per tick), derived from CONFIGURATION
        self.tick_ms = 10  # default

        # TIME variable values (resolved at compile time)
        self.time_var_values: Dict[str, int] = {}  # name_upper -> ms

    @property
    def next_bool(self) -> int:
        return self._max_bool_used

    @property
    def next_int(self) -> int:
        return self._max_int_used

    def emit(self) -> bytes:
        """Emit the complete TLV binary."""
        self._allocate_pools()
        init_bytes = self._emit_init()
        body_bytes = self._emit_body()
        header = self._emit_header(len(init_bytes), len(body_bytes))
        return header + init_bytes + body_bytes

    def _allocate_pools(self):
        """Assign pool indices to all variables and FB instances.

        Dead variable elimination: only BOOL/INT variables that are actually
        referenced in the program body get pool slots.
        """
        if self.prog.task:
            self.tick_ms = self.prog.task.interval_ms

        # Find all variable names referenced in the body
        used_vars = self._find_used_vars()

        # Pass 1: Assign indices to declared variables (declaration order)
        bool_idx = 0
        int_idx = 0

        for v in self.prog.vars:
            upper_type = v.type_name.upper()
            upper_name = v.name.upper()

            if upper_type == 'BOOL':
                if upper_name in used_vars:
                    self.bool_vars[upper_name] = bool_idx
                    bool_idx += 1
                # else: dead variable, skip
            elif upper_type == 'INT':
                if upper_name in used_vars:
                    self.int_vars[upper_name] = int_idx
                    int_idx += 1
            elif upper_type == 'TIME':
                ms = 0
                if v.init_value and isinstance(v.init_value, ExprLitTime):
                    ms = v.init_value.value_ms
                elif v.init_value and isinstance(v.init_value, ExprLitInt):
                    ms = v.init_value.value
                self.time_var_values[upper_name] = ms
            elif upper_type in FB_TYPES:
                fb_type = upper_type
                # Map FB type to counter attribute name
                fb_counter_map = {
                    'TON': 'n_ton', 'TOF': 'n_tof', 'TP': 'n_tp',
                    'CTU': 'n_ctu', 'CTD': 'n_ctd', 'CTUD': 'n_ctud',
                    'SR': 'n_sr', 'RS': 'n_rs',
                    'R_TRIG': 'n_rtrig', 'F_TRIG': 'n_ftrig',
                }
                counter_attr = fb_counter_map[fb_type]
                idx = getattr(self, counter_attr)
                setattr(self, counter_attr, idx + 1)
                self.fb_instances[upper_name] = (fb_type, idx)

        # Pass 2: Allocate scratch slots for ALL FB output parameters.
        # Every FB call opcode writes to its output slots, so we must allocate
        # them even if the outputs are never read via field access.
        FB_ALL_OUTPUTS = {
            'TON':    [('Q', ExprType.BOOL)],
            'TOF':    [('Q', ExprType.BOOL)],
            'TP':     [('Q', ExprType.BOOL)],
            'CTU':    [('Q', ExprType.BOOL), ('CV', ExprType.INT)],
            'CTD':    [('Q', ExprType.BOOL), ('CV', ExprType.INT)],
            'CTUD':   [('QU', ExprType.BOOL), ('QD', ExprType.BOOL), ('CV', ExprType.INT)],
            'SR':     [('Q1', ExprType.BOOL)],
            'RS':     [('Q1', ExprType.BOOL)],
            'R_TRIG': [('Q', ExprType.BOOL)],
            'F_TRIG': [('Q', ExprType.BOOL)],
        }
        for fb_name, (fb_type, _) in self.fb_instances.items():
            for field_name, etype in FB_ALL_OUTPUTS.get(fb_type, []):
                existing = self.fb_output_slots.get(fb_name, {}).get(field_name)
                if existing is not None:
                    continue  # already allocated
                if etype == ExprType.BOOL:
                    self.fb_output_slots.setdefault(fb_name, {})[field_name] = ('bool', bool_idx)
                    bool_idx += 1
                elif etype == ExprType.INT:
                    self.fb_output_slots.setdefault(fb_name, {})[field_name] = ('int', int_idx)
                    int_idx += 1

        # Scratch expression temps start after all declared + FB output slots
        self._scratch_bool_base = bool_idx
        self._scratch_int_base = int_idx
        self._max_bool_used = bool_idx
        self._max_int_used = int_idx

    def _find_used_vars(self) -> set:
        """Scan body for all variable names that are read or written."""
        used = set()
        self._scan_used_vars_stmts(self.prog.body, used)
        return used

    def _scan_used_vars_stmts(self, stmts: List[Stmt], used: set):
        for stmt in stmts:
            if isinstance(stmt, StmtAssign):
                used.add(stmt.target.upper())
                self._scan_used_vars_expr(stmt.expr, used)
            elif isinstance(stmt, StmtFBCall):
                # FB instance is tracked separately, not as a var
                for arg in stmt.args:
                    self._scan_used_vars_expr(arg.expr, used)
            elif isinstance(stmt, StmtIf):
                self._scan_used_vars_expr(stmt.condition, used)
                self._scan_used_vars_stmts(stmt.then_body, used)
                for elsif in stmt.elsifs:
                    self._scan_used_vars_expr(elsif.condition, used)
                    self._scan_used_vars_stmts(elsif.body, used)
                self._scan_used_vars_stmts(stmt.else_body, used)
            elif isinstance(stmt, StmtFor):
                used.add(stmt.var.upper())
                self._scan_used_vars_expr(stmt.start, used)
                self._scan_used_vars_expr(stmt.end, used)
                self._scan_used_vars_stmts(stmt.body, used)

    def _scan_used_vars_expr(self, expr: Expr, used: set):
        if isinstance(expr, ExprVar):
            used.add(expr.name.upper())
        elif isinstance(expr, ExprFieldAccess):
            pass  # FB outputs handled separately
        elif isinstance(expr, ExprBinOp):
            self._scan_used_vars_expr(expr.left, used)
            self._scan_used_vars_expr(expr.right, used)
        elif isinstance(expr, ExprUnaryOp):
            self._scan_used_vars_expr(expr.operand, used)

    def _find_needed_fb_outputs(self) -> Dict[Tuple[str, str], ExprType]:
        """Scan AST for all FB field accesses that need scratch slots."""
        needed = {}
        self._scan_stmts_for_outputs(self.prog.body, needed)
        return needed

    def _scan_stmts_for_outputs(self, stmts: List[Stmt],
                                 needed: Dict[Tuple[str, str], ExprType]):
        for stmt in stmts:
            if isinstance(stmt, StmtAssign):
                self._scan_expr_for_outputs(stmt.expr, needed)
            elif isinstance(stmt, StmtFBCall):
                for arg in stmt.args:
                    self._scan_expr_for_outputs(arg.expr, needed)
            elif isinstance(stmt, StmtIf):
                self._scan_expr_for_outputs(stmt.condition, needed)
                self._scan_stmts_for_outputs(stmt.then_body, needed)
                for elsif in stmt.elsifs:
                    self._scan_expr_for_outputs(elsif.condition, needed)
                    self._scan_stmts_for_outputs(elsif.body, needed)
                self._scan_stmts_for_outputs(stmt.else_body, needed)
            elif isinstance(stmt, StmtFor):
                self._scan_stmts_for_outputs(stmt.body, needed)

    def _scan_expr_for_outputs(self, expr, needed):
        if isinstance(expr, ExprFieldAccess):
            fb_upper = expr.obj.upper()
            field_upper = expr.field.upper()
            if fb_upper in self.fb_instances:
                fb_type = self.fb_instances[fb_upper][0]
                outputs = FB_OUTPUT_TYPES.get(fb_type, {})
                if field_upper in outputs:
                    needed[(fb_upper, field_upper)] = outputs[field_upper]
        elif isinstance(expr, ExprBinOp):
            self._scan_expr_for_outputs(expr.left, needed)
            self._scan_expr_for_outputs(expr.right, needed)
        elif isinstance(expr, ExprUnaryOp):
            self._scan_expr_for_outputs(expr.operand, needed)

    # ── Header ──

    def _emit_header(self, init_len: int, body_len: int) -> bytes:
        hdr = bytearray(20)
        hdr[0] = 0x4C  # 'L'
        hdr[1] = 0x50  # 'P'
        hdr[2] = 0x01  # version
        hdr[3] = 0x00  # flags
        hdr[4] = self._max_bool_used   # n_bools
        hdr[5] = self._max_int_used    # n_ints
        hdr[6] = self.n_ton
        hdr[7] = self.n_tof
        hdr[8] = self.n_tp
        hdr[9] = self.n_ctu
        hdr[10] = self.n_ctd
        hdr[11] = self.n_ctud
        hdr[12] = self.n_sr
        hdr[13] = self.n_rs
        hdr[14] = self.n_rtrig
        hdr[15] = self.n_ftrig
        struct.pack_into('<H', hdr, 16, init_len)
        struct.pack_into('<H', hdr, 18, body_len)
        return bytes(hdr)

    # ── Init section ──

    def _emit_init(self) -> bytes:
        """Emit INIT section: LOAD_INT for non-zero INT initial values."""
        buf = bytearray()
        for v in self.prog.vars:
            upper_name = v.name.upper()
            upper_type = v.type_name.upper()
            if upper_type == 'INT' and v.init_value is not None:
                if isinstance(v.init_value, ExprLitInt) and v.init_value.value != 0:
                    if upper_name in self.int_vars:
                        idx = self.int_vars[upper_name]
                        val = wrap_i16(v.init_value.value)
                        buf += self._tlv(TAG_LOAD_INT, struct.pack('<Bh', idx, val))
            elif upper_type == 'BOOL' and v.init_value is not None:
                if isinstance(v.init_value, ExprLitBool):
                    if upper_name in self.bool_vars and v.init_value.value:
                        idx = self.bool_vars[upper_name]
                        buf += self._tlv(TAG_LOAD_TRUE, bytes([idx]))
        return bytes(buf)

    # ── Body section ──

    def _emit_body(self) -> bytes:
        """Emit body opcodes, resetting scratch per top-level statement."""
        buf = bytearray()
        for stmt in self.prog.body:
            self._reset_scratch()
            buf += self._emit_stmt(stmt)
        return bytes(buf)

    def _emit_stmt(self, stmt: Stmt) -> bytes:
        if isinstance(stmt, StmtAssign):
            return self._emit_assign(stmt)
        if isinstance(stmt, StmtFBCall):
            return self._emit_fb_call(stmt)
        if isinstance(stmt, StmtIf):
            return self._emit_if(stmt)
        if isinstance(stmt, StmtFor):
            return self._emit_for(stmt)
        raise ValueError(f"Unknown statement type: {type(stmt)}")

    # ── Assignment ──

    def _emit_assign(self, stmt: StmtAssign) -> bytes:
        target_upper = stmt.target.upper()

        if target_upper in self.bool_vars:
            dst = self.bool_vars[target_upper]
            return self._emit_bool_expr_to(stmt.expr, dst)
        elif target_upper in self.int_vars:
            dst = self.int_vars[target_upper]
            return self._emit_int_expr_to(stmt.expr, dst)
        else:
            raise ValueError(f"Unknown variable: {stmt.target}")

    # ── Bool expression emission ──

    def _is_bool_direct(self, expr: Expr) -> bool:
        """True if expr resolves to an existing bool pool slot (no codegen)."""
        if isinstance(expr, ExprVar):
            return expr.name.upper() in self.bool_vars
        if isinstance(expr, ExprFieldAccess):
            fb_upper = expr.obj.upper()
            field_upper = expr.field.upper()
            slots = self.fb_output_slots.get(fb_upper, {})
            entry = slots.get(field_upper)
            return entry is not None and entry[0] == 'bool'
        return False

    def _get_bool_direct(self, expr: Expr) -> int:
        """Get the bool pool index for a direct bool reference."""
        if isinstance(expr, ExprVar):
            return self.bool_vars[expr.name.upper()]
        if isinstance(expr, ExprFieldAccess):
            return self._fb_output_bool(expr.obj.upper(), expr.field.upper())
        raise ValueError("Not a direct bool reference")

    def _is_int_direct(self, expr: Expr) -> bool:
        """True if expr resolves to an existing int pool slot (no codegen)."""
        if isinstance(expr, ExprVar):
            return expr.name.upper() in self.int_vars
        if isinstance(expr, ExprFieldAccess):
            fb_upper = expr.obj.upper()
            field_upper = expr.field.upper()
            slots = self.fb_output_slots.get(fb_upper, {})
            entry = slots.get(field_upper)
            return entry is not None and entry[0] == 'int'
        return False

    def _get_int_direct(self, expr: Expr) -> int:
        """Get the int pool index for a direct int reference."""
        if isinstance(expr, ExprVar):
            return self.int_vars[expr.name.upper()]
        if isinstance(expr, ExprFieldAccess):
            return self._fb_output_int(expr.obj.upper(), expr.field.upper())
        raise ValueError("Not a direct int reference")

    def _emit_bool_expr_to(self, expr: Expr, dst: int) -> bytes:
        """Emit opcodes that compute a BOOL expr and store result in bools[dst]."""
        if isinstance(expr, ExprLitBool):
            if expr.value:
                return self._tlv(TAG_LOAD_TRUE, bytes([dst]))
            else:
                return self._tlv(TAG_LOAD_FALSE, bytes([dst]))

        if isinstance(expr, ExprVar):
            src = self.bool_vars[expr.name.upper()]
            if src == dst:
                return b''
            return self._tlv(TAG_COPY_BOOL, bytes([dst, src]))

        if isinstance(expr, ExprFieldAccess):
            src = self._fb_output_bool(expr.obj.upper(), expr.field.upper())
            if src == dst:
                return b''
            return self._tlv(TAG_COPY_BOOL, bytes([dst, src]))

        if isinstance(expr, ExprUnaryOp) and expr.op == 'NOT':
            # NOT: if operand is direct, just BOOL_NOT dst, src
            if self._is_bool_direct(expr.operand):
                src = self._get_bool_direct(expr.operand)
                return self._tlv(TAG_BOOL_NOT, bytes([dst, src]))
            # Otherwise, emit operand to dst first, then NOT in-place
            buf = self._emit_bool_expr_to(expr.operand, dst)
            buf += self._tlv(TAG_BOOL_NOT, bytes([dst, dst]))
            return buf

        if isinstance(expr, ExprBinOp):
            if expr.op in ('AND', 'OR', 'XOR'):
                return self._emit_bool_binop(expr, dst)
            if expr.op in ('=', '<>', '<', '>', '<=', '>='):
                return self._emit_comparison(expr, dst)

        raise ValueError(f"Cannot emit BOOL expression: {expr}")

    def _emit_bool_binop(self, expr: ExprBinOp, dst: int) -> bytes:
        """Emit AND/OR/XOR with minimal scratch usage.

        Strategy:
        - Both direct: OP dst, left, right (always safe, 0 scratch)
        - Left direct, right complex:
          - If left.idx != dst: emit right to dst, OP dst, left, dst (0 scratch)
          - If left.idx == dst: emit right to scratch, OP dst, dst, scratch (1 scratch)
        - Left complex, right direct: emit left to scratch, OP dst, scratch, right (1 scratch)
        - Both complex: emit left to scratch, emit right to dst, OP dst, scratch, dst (1 scratch)
        """
        tag_map = {'AND': TAG_BOOL_AND, 'OR': TAG_BOOL_OR, 'XOR': TAG_BOOL_XOR}
        tag = tag_map[expr.op]
        buf = bytearray()

        left_direct = self._is_bool_direct(expr.left)
        right_direct = self._is_bool_direct(expr.right)

        if left_direct and right_direct:
            left_idx = self._get_bool_direct(expr.left)
            right_idx = self._get_bool_direct(expr.right)
            buf += self._tlv(tag, bytes([dst, left_idx, right_idx]))
        elif left_direct:
            left_idx = self._get_bool_direct(expr.left)
            if left_idx == dst:
                # Aliasing: emit right to scratch to avoid overwriting left
                tmp = self._alloc_bool_scratch()
                buf += self._emit_bool_expr_to(expr.right, tmp)
                buf += self._tlv(tag, bytes([dst, dst, tmp]))
            else:
                # Safe: emit right to dst, then OP dst, left, dst
                buf += self._emit_bool_expr_to(expr.right, dst)
                buf += self._tlv(tag, bytes([dst, left_idx, dst]))
        elif right_direct:
            right_idx = self._get_bool_direct(expr.right)
            tmp = self._alloc_bool_scratch()
            buf += self._emit_bool_expr_to(expr.left, tmp)
            buf += self._tlv(tag, bytes([dst, tmp, right_idx]))
        else:
            tmp = self._alloc_bool_scratch()
            buf += self._emit_bool_expr_to(expr.left, tmp)
            buf += self._emit_bool_expr_to(expr.right, dst)
            buf += self._tlv(tag, bytes([dst, tmp, dst]))

        return bytes(buf)

    def _resolve_bool_source(self, expr: Expr, buf: bytearray) -> int:
        """Resolve a BOOL expression to a pool index. May emit opcodes to buf.
        Returns the bool pool index where the result is."""
        if self._is_bool_direct(expr):
            return self._get_bool_direct(expr)

        if isinstance(expr, ExprLitBool):
            tmp = self._alloc_bool_scratch()
            if expr.value:
                buf += self._tlv(TAG_LOAD_TRUE, bytes([tmp]))
            else:
                buf += self._tlv(TAG_LOAD_FALSE, bytes([tmp]))
            return tmp

        # Complex sub-expression: emit to scratch
        tmp = self._alloc_bool_scratch()
        buf += self._emit_bool_expr_to(expr, tmp)
        return tmp

    def _resolve_int_source(self, expr: Expr, buf: bytearray) -> int:
        """Resolve an INT expression to a pool index. May emit opcodes to buf."""
        if self._is_int_direct(expr):
            return self._get_int_direct(expr)

        if isinstance(expr, ExprLitInt):
            tmp = self._alloc_int_scratch()
            val = wrap_i16(expr.value)
            buf += self._tlv(TAG_LOAD_INT, struct.pack('<Bh', tmp, val))
            return tmp

        # Complex sub-expression: emit to scratch
        tmp = self._alloc_int_scratch()
        buf += self._emit_int_expr_to(expr, tmp)
        return tmp

    def _emit_comparison(self, expr: ExprBinOp, dst: int) -> bytes:
        """Emit a comparison (result is BOOL, operands are INT)."""
        tag_map = {'=': TAG_CMP_EQ, '<>': TAG_CMP_NE, '<': TAG_CMP_LT,
                   '>': TAG_CMP_GT, '<=': TAG_CMP_LE, '>=': TAG_CMP_GE}
        tag = tag_map[expr.op]

        buf = bytearray()
        a = self._resolve_int_source(expr.left, buf)
        b = self._resolve_int_source(expr.right, buf)
        buf += self._tlv(tag, bytes([dst, a, b]))
        return bytes(buf)

    def _emit_int_expr_to(self, expr: Expr, dst: int) -> bytes:
        """Emit opcodes that compute an INT expr and store in ints[dst]."""
        if isinstance(expr, ExprLitInt):
            val = wrap_i16(expr.value)
            return self._tlv(TAG_LOAD_INT, struct.pack('<Bh', dst, val))

        if isinstance(expr, ExprVar):
            src = self.int_vars[expr.name.upper()]
            if src == dst:
                return b''
            return self._tlv(TAG_COPY_INT, bytes([dst, src]))

        if isinstance(expr, ExprFieldAccess):
            src = self._fb_output_int(expr.obj.upper(), expr.field.upper())
            if src == dst:
                return b''
            return self._tlv(TAG_COPY_INT, bytes([dst, src]))

        if isinstance(expr, ExprUnaryOp) and expr.op == 'UNARY_MINUS':
            # -x = 0 - x
            buf = bytearray()
            zero_tmp = self._alloc_int_scratch()
            buf += self._tlv(TAG_LOAD_INT, struct.pack('<Bh', zero_tmp, 0))
            src = self._resolve_int_source(expr.operand, buf)
            buf += self._tlv(TAG_INT_SUB, bytes([dst, zero_tmp, src]))
            return bytes(buf)

        if isinstance(expr, ExprBinOp) and expr.op in ('+', '-', '*'):
            return self._emit_int_binop(expr, dst)

        raise ValueError(f"Cannot emit INT expression: {expr}")

    def _emit_int_binop(self, expr: ExprBinOp, dst: int) -> bytes:
        """Emit INT binary op with aliasing-safe scratch reuse."""
        tag_map = {'+': TAG_INT_ADD, '-': TAG_INT_SUB, '*': TAG_INT_MUL}
        tag = tag_map[expr.op]
        buf = bytearray()

        left_direct = self._is_int_direct(expr.left)
        right_direct = self._is_int_direct(expr.right)

        if left_direct and right_direct:
            a = self._get_int_direct(expr.left)
            b = self._get_int_direct(expr.right)
            buf += self._tlv(tag, bytes([dst, a, b]))
        elif left_direct:
            a = self._get_int_direct(expr.left)
            if a == dst:
                # Aliasing: emit right to scratch to avoid overwriting left
                tmp = self._alloc_int_scratch()
                buf += self._emit_int_expr_to(expr.right, tmp)
                buf += self._tlv(tag, bytes([dst, dst, tmp]))
            else:
                # Safe: emit right to dst, then OP dst, left, dst
                buf += self._emit_int_expr_to(expr.right, dst)
                buf += self._tlv(tag, bytes([dst, a, dst]))
        elif right_direct:
            b = self._get_int_direct(expr.right)
            tmp = self._alloc_int_scratch()
            buf += self._emit_int_expr_to(expr.left, tmp)
            buf += self._tlv(tag, bytes([dst, tmp, b]))
        else:
            tmp = self._alloc_int_scratch()
            buf += self._emit_int_expr_to(expr.left, tmp)
            buf += self._emit_int_expr_to(expr.right, dst)
            buf += self._tlv(tag, bytes([dst, tmp, dst]))

        return bytes(buf)

    # ── FB Call ──

    def _emit_fb_call(self, stmt: StmtFBCall) -> bytes:
        inst_upper = stmt.instance.upper()
        fb_type, fb_idx = self.fb_instances[inst_upper]
        args = {a.name.upper(): a.expr for a in stmt.args}

        buf = bytearray()

        if fb_type in ('TON', 'TOF', 'TP'):
            tag = {'TON': TAG_TON_CALL, 'TOF': TAG_TOF_CALL, 'TP': TAG_TP_CALL}[fb_type]
            in_expr = args.get('IN')
            in_b = self._resolve_bool_source(in_expr, buf)
            pt_expr = args.get('PT')
            pt_ticks = self._resolve_time_to_ticks(pt_expr)
            q_slot = self._fb_output_bool(inst_upper, 'Q')
            payload = bytes([fb_idx, in_b]) + struct.pack('<I', pt_ticks) + bytes([q_slot])
            buf += self._tlv(tag, payload)
            return bytes(buf)

        if fb_type == 'CTU':
            cu_b = self._resolve_bool_source(args.get('CU'), buf)
            r_b = self._resolve_bool_source(args.get('R'), buf)
            pv_i = self._resolve_int_source(args.get('PV'), buf)
            q_b = self._fb_output_bool(inst_upper, 'Q')
            cv_i = self._fb_output_int(inst_upper, 'CV')
            payload = bytes([fb_idx, cu_b, r_b, pv_i, q_b, cv_i])
            buf += self._tlv(TAG_CTU_CALL, payload)
            return bytes(buf)

        if fb_type == 'CTD':
            cd_b = self._resolve_bool_source(args.get('CD'), buf)
            ld_b = self._resolve_bool_source(args.get('LD'), buf)
            pv_i = self._resolve_int_source(args.get('PV'), buf)
            q_b = self._fb_output_bool(inst_upper, 'Q')
            cv_i = self._fb_output_int(inst_upper, 'CV')
            payload = bytes([fb_idx, cd_b, ld_b, pv_i, q_b, cv_i])
            buf += self._tlv(TAG_CTD_CALL, payload)
            return bytes(buf)

        if fb_type == 'CTUD':
            cu_b = self._resolve_bool_source(args.get('CU'), buf)
            cd_b = self._resolve_bool_source(args.get('CD'), buf)
            r_b = self._resolve_bool_source(args.get('R'), buf)
            ld_b = self._resolve_bool_source(args.get('LD'), buf)
            pv_i = self._resolve_int_source(args.get('PV'), buf)
            qu_b = self._fb_output_bool(inst_upper, 'QU')
            qd_b = self._fb_output_bool(inst_upper, 'QD')
            cv_i = self._fb_output_int(inst_upper, 'CV')
            payload = bytes([fb_idx, cu_b, cd_b, r_b, ld_b, pv_i, qu_b, qd_b, cv_i])
            buf += self._tlv(TAG_CTUD_CALL, payload)
            return bytes(buf)

        if fb_type == 'SR':
            s1_b = self._resolve_bool_source(args.get('S1'), buf)
            r_b = self._resolve_bool_source(args.get('R'), buf)
            q1_b = self._fb_output_bool(inst_upper, 'Q1')
            payload = bytes([fb_idx, s1_b, r_b, q1_b])
            buf += self._tlv(TAG_SR_CALL, payload)
            return bytes(buf)

        if fb_type == 'RS':
            s_b = self._resolve_bool_source(args.get('S'), buf)
            r1_b = self._resolve_bool_source(args.get('R1'), buf)
            q1_b = self._fb_output_bool(inst_upper, 'Q1')
            payload = bytes([fb_idx, s_b, r1_b, q1_b])
            buf += self._tlv(TAG_RS_CALL, payload)
            return bytes(buf)

        if fb_type == 'R_TRIG':
            clk_b = self._resolve_bool_source(args.get('CLK'), buf)
            q_b = self._fb_output_bool(inst_upper, 'Q')
            payload = bytes([fb_idx, clk_b, q_b])
            buf += self._tlv(TAG_R_TRIG_CALL, payload)
            return bytes(buf)

        if fb_type == 'F_TRIG':
            clk_b = self._resolve_bool_source(args.get('CLK'), buf)
            q_b = self._fb_output_bool(inst_upper, 'Q')
            payload = bytes([fb_idx, clk_b, q_b])
            buf += self._tlv(TAG_F_TRIG_CALL, payload)
            return bytes(buf)

        raise ValueError(f"Unknown FB type: {fb_type}")

    def _resolve_time_to_ticks(self, expr: Expr) -> int:
        """Convert a TIME expression to tick count."""
        if isinstance(expr, ExprLitTime):
            return expr.value_ms // self.tick_ms
        if isinstance(expr, ExprVar):
            upper = expr.name.upper()
            if upper in self.time_var_values:
                return self.time_var_values[upper] // self.tick_ms
        raise ValueError(f"Cannot resolve TIME expression to ticks: {expr}")

    # ── IF ──

    def _emit_if(self, stmt: StmtIf) -> bytes:
        buf = bytearray()

        # Pre-compute all conditions into scratch bool slots
        cond_slot = self._alloc_bool_scratch()
        buf += self._emit_bool_expr_to(stmt.condition, cond_slot)

        elsif_cond_slots = []
        for elsif in stmt.elsifs:
            s = self._alloc_bool_scratch()
            buf += self._emit_bool_expr_to(elsif.condition, s)
            elsif_cond_slots.append(s)

        # Now emit the IF_BLOCK opcode
        then_bytes = bytearray()
        for s in stmt.then_body:
            then_bytes += self._emit_stmt(s)

        elsif_bytes_list = []
        for elsif in stmt.elsifs:
            eb = bytearray()
            for s in elsif.body:
                eb += self._emit_stmt(s)
            elsif_bytes_list.append(bytes(eb))

        else_bytes = bytearray()
        for s in stmt.else_body:
            else_bytes += self._emit_stmt(s)

        # Build IF_BLOCK payload:
        # [cond_b:1][then_len:2][then_body][num_elsif:1]
        # {[elsif_cond:1][elsif_len:2][elsif_body]}*
        # [else_len:2][else_body]
        payload = bytearray()
        payload += bytes([cond_slot])
        payload += struct.pack('<H', len(then_bytes))
        payload += then_bytes
        payload += bytes([len(stmt.elsifs)])
        for i, elsif in enumerate(stmt.elsifs):
            payload += bytes([elsif_cond_slots[i]])
            payload += struct.pack('<H', len(elsif_bytes_list[i]))
            payload += elsif_bytes_list[i]
        payload += struct.pack('<H', len(else_bytes))
        payload += else_bytes

        buf += self._tlv(TAG_IF_BLOCK, bytes(payload))
        return bytes(buf)

    # ── FOR ──

    def _emit_for(self, stmt: StmtFor) -> bytes:
        var_upper = stmt.var.upper()
        idx_i = self.int_vars[var_upper]

        if not isinstance(stmt.start, ExprLitInt):
            raise ValueError("FOR start must be an INT literal")
        if not isinstance(stmt.end, ExprLitInt):
            raise ValueError("FOR end must be an INT literal")

        start_val = wrap_i16(stmt.start.value)
        end_val = wrap_i16(stmt.end.value)

        body_bytes = bytearray()
        for s in stmt.body:
            body_bytes += self._emit_stmt(s)

        # FOR_BLOCK payload: [idx_i:1][start:2 LE][end:2 LE][body_len:2 LE][body]
        payload = struct.pack('<BhhH', idx_i, start_val, end_val, len(body_bytes))
        payload += body_bytes

        return self._tlv(TAG_FOR_BLOCK, payload)

    # ── Helpers ──

    def _tlv(self, tag: int, payload: bytes) -> bytes:
        """Encode a single TLV opcode: [tag:1][len:2 LE][payload:len]."""
        return bytes([tag]) + struct.pack('<H', len(payload)) + payload

    def _alloc_bool_scratch(self) -> int:
        """Allocate a temporary BOOL scratch slot."""
        result = self._scratch_bool_base + self._bool_scratch_counter
        self._bool_scratch_counter += 1
        if result + 1 > self._max_bool_used:
            self._max_bool_used = result + 1
        return result

    def _alloc_int_scratch(self) -> int:
        """Allocate a temporary INT scratch slot."""
        result = self._scratch_int_base + self._int_scratch_counter
        self._int_scratch_counter += 1
        if result + 1 > self._max_int_used:
            self._max_int_used = result + 1
        return result

    def _reset_scratch(self):
        """Reset scratch counters (call at the start of each top-level stmt)."""
        self._bool_scratch_counter = 0
        self._int_scratch_counter = 0

    def _fb_output_bool(self, fb_name: str, field: str) -> int:
        """Get the bool pool index for an FB output."""
        slots = self.fb_output_slots.get(fb_name, {})
        entry = slots.get(field)
        if entry is None:
            raise ValueError(f"No scratch slot for {fb_name}.{field}")
        pool_type, idx = entry
        if pool_type != 'bool':
            raise ValueError(f"{fb_name}.{field} is not a BOOL output")
        return idx

    def _fb_output_int(self, fb_name: str, field: str) -> int:
        """Get the int pool index for an FB output."""
        slots = self.fb_output_slots.get(fb_name, {})
        entry = slots.get(field)
        if entry is None:
            raise ValueError(f"No scratch slot for {fb_name}.{field}")
        pool_type, idx = entry
        if pool_type != 'int':
            raise ValueError(f"{fb_name}.{field} is not an INT output")
        return idx


# ══════════════════════════════════════════════════════════════════════
# Section 7: CLI + Debugging
# ══════════════════════════════════════════════════════════════════════

def dump_ast(prog: ProgramAST):
    """Print AST in a human-readable format."""
    print(f"PROGRAM {prog.name}")
    print(f"  Variables ({len(prog.vars)}):")
    for v in prog.vars:
        init_str = f" := {v.init_value}" if v.init_value else ""
        print(f"    {v.name} : {v.type_name}{init_str}")
    print(f"  Body ({len(prog.body)} statements):")
    _dump_stmts(prog.body, 4)
    if prog.task:
        print(f"  TASK {prog.task.name}: interval={prog.task.interval_ms}ms, "
              f"priority={prog.task.priority}")
    if prog.binding:
        print(f"  PROGRAM {prog.binding.instance_name} WITH {prog.binding.task_name} "
              f": {prog.binding.program_name}")

def _dump_stmts(stmts, indent):
    pad = ' ' * indent
    for s in stmts:
        if isinstance(s, StmtAssign):
            print(f"{pad}{s.target} := {_dump_expr(s.expr)}")
        elif isinstance(s, StmtFBCall):
            args = ', '.join(f"{a.name}:={_dump_expr(a.expr)}" for a in s.args)
            print(f"{pad}{s.instance}({args})")
        elif isinstance(s, StmtIf):
            print(f"{pad}IF {_dump_expr(s.condition)} THEN")
            _dump_stmts(s.then_body, indent + 2)
            for e in s.elsifs:
                print(f"{pad}ELSIF {_dump_expr(e.condition)} THEN")
                _dump_stmts(e.body, indent + 2)
            if s.else_body:
                print(f"{pad}ELSE")
                _dump_stmts(s.else_body, indent + 2)
            print(f"{pad}END_IF")
        elif isinstance(s, StmtFor):
            print(f"{pad}FOR {s.var} := {_dump_expr(s.start)} TO {_dump_expr(s.end)} DO")
            _dump_stmts(s.body, indent + 2)
            print(f"{pad}END_FOR")

def _dump_expr(e) -> str:
    if isinstance(e, ExprLitBool):
        return 'TRUE' if e.value else 'FALSE'
    if isinstance(e, ExprLitInt):
        return str(e.value)
    if isinstance(e, ExprLitTime):
        return f"T#{e.value_ms}ms"
    if isinstance(e, ExprVar):
        return e.name
    if isinstance(e, ExprFieldAccess):
        return f"{e.obj}.{e.field}"
    if isinstance(e, ExprBinOp):
        return f"({_dump_expr(e.left)} {e.op} {_dump_expr(e.right)})"
    if isinstance(e, ExprUnaryOp):
        return f"({e.op} {_dump_expr(e.operand)})"
    return repr(e)


def dump_tlv_hex(data: bytes):
    """Print TLV binary as annotated hex dump."""
    print(f"Total size: {len(data)} bytes")
    print()

    # Header
    print("HEADER (20 bytes):")
    print(f"  magic:    {data[0]:02x} {data[1]:02x} ('{chr(data[0])}{chr(data[1])}')")
    print(f"  version:  {data[2]}")
    print(f"  flags:    {data[3]:02x}")
    print(f"  n_bools:  {data[4]}")
    print(f"  n_ints:   {data[5]}")
    print(f"  n_ton:    {data[6]}")
    print(f"  n_tof:    {data[7]}")
    print(f"  n_tp:     {data[8]}")
    print(f"  n_ctu:    {data[9]}")
    print(f"  n_ctd:    {data[10]}")
    print(f"  n_ctud:   {data[11]}")
    print(f"  n_sr:     {data[12]}")
    print(f"  n_rs:     {data[13]}")
    print(f"  n_rtrig:  {data[14]}")
    print(f"  n_ftrig:  {data[15]}")
    init_len = struct.unpack_from('<H', data, 16)[0]
    body_len = struct.unpack_from('<H', data, 18)[0]
    print(f"  init_len: {init_len}")
    print(f"  body_len: {body_len}")

    TAG_NAMES = {
        0x01: 'LOAD_TRUE', 0x02: 'LOAD_FALSE', 0x03: 'LOAD_INT',
        0x10: 'BOOL_NOT', 0x11: 'BOOL_AND', 0x12: 'BOOL_OR', 0x13: 'BOOL_XOR',
        0x14: 'COPY_BOOL',
        0x20: 'INT_ADD', 0x21: 'INT_SUB', 0x22: 'INT_MUL', 0x23: 'COPY_INT',
        0x30: 'CMP_EQ', 0x31: 'CMP_NE', 0x32: 'CMP_LT', 0x33: 'CMP_GT',
        0x34: 'CMP_LE', 0x35: 'CMP_GE',
        0x40: 'IF_BLOCK', 0x41: 'FOR_BLOCK',
        0x50: 'TON_CALL', 0x51: 'TOF_CALL', 0x52: 'TP_CALL',
        0x60: 'CTU_CALL', 0x61: 'CTD_CALL', 0x62: 'CTUD_CALL',
        0x70: 'SR_CALL', 0x71: 'RS_CALL', 0x72: 'R_TRIG_CALL', 0x73: 'F_TRIG_CALL',
    }

    def dump_opcodes(data, offset, end, indent="  "):
        while offset < end:
            tag = data[offset]
            plen = struct.unpack_from('<H', data, offset + 1)[0]
            name = TAG_NAMES.get(tag, f"UNKNOWN(0x{tag:02x})")
            payload = data[offset+3:offset+3+plen]
            hex_str = ' '.join(f'{b:02x}' for b in payload)
            print(f"{indent}@{offset-20:4d}: {name:15s} len={plen:3d}  [{hex_str}]")

            if tag == 0x40:  # IF_BLOCK - recurse
                _dump_if_block(data, offset + 3, plen, indent + "  ")

            offset += 3 + plen

    def _dump_if_block(data, start, plen, indent):
        off = start
        cond_b = data[off]; off += 1
        then_len = struct.unpack_from('<H', data, off)[0]; off += 2
        print(f"{indent}cond_b={cond_b}, then_len={then_len}")
        if then_len > 0:
            print(f"{indent}THEN:")
            dump_opcodes(data, off, off + then_len, indent + "  ")
        off += then_len
        num_elsif = data[off]; off += 1
        print(f"{indent}num_elsif={num_elsif}")
        for i in range(num_elsif):
            ec = data[off]; off += 1
            elen = struct.unpack_from('<H', data, off)[0]; off += 2
            print(f"{indent}ELSIF[{i}] cond_b={ec}, len={elen}:")
            if elen > 0:
                dump_opcodes(data, off, off + elen, indent + "  ")
            off += elen
        else_len = struct.unpack_from('<H', data, off)[0]; off += 2
        print(f"{indent}ELSE len={else_len}:")
        if else_len > 0:
            dump_opcodes(data, off, off + else_len, indent + "  ")

    if init_len > 0:
        print(f"\nINIT ({init_len} bytes):")
        dump_opcodes(data, 20, 20 + init_len)

    if body_len > 0:
        print(f"\nBODY ({body_len} bytes):")
        dump_opcodes(data, 20 + init_len, 20 + init_len + body_len)


def dump_types(prog: ProgramAST, emitter: TLVEmitter):
    """Print type environment and pool mapping."""
    print("=== Type Environment ===")
    print(f"Tick interval: {emitter.tick_ms}ms")
    print()
    print("BOOL pool:")
    for name, idx in sorted(emitter.bool_vars.items(), key=lambda x: x[1]):
        print(f"  [{idx:2d}] {name}")
    for fb_name, outputs in sorted(emitter.fb_output_slots.items()):
        for field, (ptype, idx) in sorted(outputs.items()):
            if ptype == 'bool':
                print(f"  [{idx:2d}] {fb_name}.{field} (scratch)")
    print(f"  Total: {emitter.next_bool}")
    print()
    print("INT pool:")
    for name, idx in sorted(emitter.int_vars.items(), key=lambda x: x[1]):
        print(f"  [{idx:2d}] {name}")
    for fb_name, outputs in sorted(emitter.fb_output_slots.items()):
        for field, (ptype, idx) in sorted(outputs.items()):
            if ptype == 'int':
                print(f"  [{idx:2d}] {fb_name}.{field} (scratch)")
    print(f"  Total: {emitter.next_int}")
    print()
    print("FB instances:")
    for name, (ftype, idx) in sorted(emitter.fb_instances.items()):
        print(f"  {ftype}[{idx}] = {name}")
    print()
    print("TIME variables:")
    for name, ms in sorted(emitter.time_var_values.items()):
        ticks = ms // emitter.tick_ms if emitter.tick_ms > 0 else 0
        print(f"  {name} = {ms}ms = {ticks} ticks")


def compile_file(source: str, filename: str = "<stdin>") -> Tuple[ProgramAST, bytes]:
    """Full compilation pipeline: source → (AST, TLV bytes)."""
    lexer = Lexer(source, filename)
    tokens = lexer.tokenize()

    parser = Parser(tokens, filename)
    prog = parser.parse()

    checker = TypeChecker(prog, filename)
    checker.check()

    emitter = TLVEmitter(prog, filename)
    tlv = emitter.emit()

    return prog, tlv, emitter


def main():
    ap = argparse.ArgumentParser(description='LowPLC-ST to TLV compiler')
    ap.add_argument('input', help='Input .st file')
    ap.add_argument('-o', '--output', help='Output .tlv file')
    ap.add_argument('--dump-ast', action='store_true', help='Print AST')
    ap.add_argument('--dump-types', action='store_true', help='Print type environment')
    ap.add_argument('--dump-tlv-hex', action='store_true', help='Print TLV hex dump')
    ap.add_argument('--dump-c-header', action='store_true',
                    help='Print TLV as C header (uint8_t array)')
    args = ap.parse_args()

    with open(args.input, 'r') as f:
        source = f.read()

    try:
        prog, tlv, emitter = compile_file(source, args.input)
    except (SyntaxError, TypeError) as e:
        print(f"Error: {e}", file=sys.stderr)
        sys.exit(1)

    if args.dump_ast:
        dump_ast(prog)
        print()

    if args.dump_types:
        dump_types(prog, emitter)
        print()

    if args.dump_tlv_hex:
        dump_tlv_hex(tlv)
        print()

    if args.dump_c_header:
        basename = args.input.rsplit('/', 1)[-1].rsplit('.', 1)[0]
        varname = basename.lower().replace('-', '_')
        print(f"static const uint8_t {varname}_tlv[] = {{")
        for i in range(0, len(tlv), 16):
            chunk = tlv[i:i+16]
            hex_str = ', '.join(f'0x{b:02x}' for b in chunk)
            print(f"    {hex_str},")
        print(f"}};")
        print(f"// Total: {len(tlv)} bytes")
        print()

    if args.output:
        with open(args.output, 'wb') as f:
            f.write(tlv)
        print(f"Wrote {len(tlv)} bytes to {args.output}")
    elif not any([args.dump_ast, args.dump_types, args.dump_tlv_hex, args.dump_c_header]):
        # Default: print hex dump
        dump_tlv_hex(tlv)


if __name__ == '__main__':
    main()
