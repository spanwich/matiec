#!/usr/bin/env python3
"""
test_parser.py — Unit tests for the LowPLC-ST parser.

Covers:
  - Lexer: tokenization, comments, TIME literals, case insensitivity
  - Parser: expression precedence, statements, FB calls, control flow
  - Type checker: type errors, FB type resolution
  - TLV emitter: header, opcodes, pool allocation, dead variable elimination
  - B1 byte-identical check against hand-encoded TLV
  - End-to-end: all 5 benchmarks parse without errors
"""

import os
import struct
import sys
import unittest

# Add tools to path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from lowplcst_parser import (
    Lexer, TT, Token, parse_time_literal,
    Parser, ProgramAST, ExprType,
    ExprLitBool, ExprLitInt, ExprVar, ExprBinOp, ExprUnaryOp,
    ExprFieldAccess, ExprLitTime,
    StmtAssign, StmtFBCall, StmtIf, StmtFor,
    TypeChecker, TLVEmitter, compile_file, wrap_i16,
)

BASE_DIR = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))


# ── Lexer Tests ─────────────────────────────────────────────────────

class TestLexer(unittest.TestCase):

    def _tokens(self, src):
        return Lexer(src).tokenize()

    def test_empty(self):
        tokens = self._tokens('')
        self.assertEqual(len(tokens), 1)
        self.assertEqual(tokens[0].type, TT.EOF)

    def test_int_literal(self):
        tokens = self._tokens('42')
        self.assertEqual(tokens[0].type, TT.INT_LIT)
        self.assertEqual(tokens[0].value, 42)

    def test_negative_int_literal(self):
        tokens = self._tokens('-7')
        self.assertEqual(tokens[0].type, TT.MINUS)
        self.assertEqual(tokens[1].type, TT.INT_LIT)
        self.assertEqual(tokens[1].value, 7)

    def test_bool_literals(self):
        tokens = self._tokens('TRUE FALSE')
        self.assertEqual(tokens[0].type, TT.BOOL_LIT)
        self.assertTrue(tokens[0].value)
        self.assertEqual(tokens[1].type, TT.BOOL_LIT)
        self.assertFalse(tokens[1].value)

    def test_case_insensitive_keywords(self):
        tokens = self._tokens('if If IF iF')
        for t in tokens[:4]:
            self.assertEqual(t.type, TT.IF)

    def test_identifiers(self):
        tokens = self._tokens('my_var x1 pressure_high')
        for t in tokens[:3]:
            self.assertEqual(t.type, TT.IDENT)
        self.assertEqual(tokens[0].value, 'my_var')
        self.assertEqual(tokens[2].value, 'pressure_high')

    def test_operators(self):
        tokens = self._tokens(':= : ; ( ) . + - * = <> < > <= >=')
        expected = [TT.ASSIGN, TT.COLON, TT.SEMI, TT.LPAREN, TT.RPAREN,
                    TT.DOT, TT.PLUS, TT.MINUS, TT.STAR, TT.EQ, TT.NE,
                    TT.LT, TT.GT, TT.LE, TT.GE]
        for i, tt in enumerate(expected):
            self.assertEqual(tokens[i].type, tt, f"Token {i}: expected {tt}")

    def test_nested_comments(self):
        tokens = self._tokens('a (* outer (* inner *) still_comment *) b')
        idents = [t for t in tokens if t.type == TT.IDENT]
        self.assertEqual(len(idents), 2)
        self.assertEqual(idents[0].value, 'a')
        self.assertEqual(idents[1].value, 'b')

    def test_fb_type_keywords(self):
        tokens = self._tokens('TON TOF TP CTU CTD CTUD SR RS R_TRIG F_TRIG')
        expected = [TT.TON, TT.TOF, TT.TP, TT.CTU, TT.CTD, TT.CTUD,
                    TT.SR, TT.RS, TT.R_TRIG, TT.F_TRIG]
        for i, tt in enumerate(expected):
            self.assertEqual(tokens[i].type, tt)

    def test_time_literal_token(self):
        tokens = self._tokens('T#3s')
        self.assertEqual(tokens[0].type, TT.TIME_LIT)
        self.assertEqual(tokens[0].value, 3000)  # 3000 ms

    def test_time_literal_complex(self):
        tokens = self._tokens('T#1m30s')
        self.assertEqual(tokens[0].type, TT.TIME_LIT)
        self.assertEqual(tokens[0].value, 90000)  # 90000 ms

    def test_line_tracking(self):
        tokens = self._tokens('a\nb\nc')
        self.assertEqual(tokens[0].line, 1)
        self.assertEqual(tokens[1].line, 2)
        self.assertEqual(tokens[2].line, 3)


# ── TIME Literal Parsing ────────────────────────────────────────────

class TestTimeLiteral(unittest.TestCase):

    def test_seconds(self):
        self.assertEqual(parse_time_literal('T#3s'), 3000)

    def test_milliseconds(self):
        self.assertEqual(parse_time_literal('T#500ms'), 500)

    def test_minutes(self):
        self.assertEqual(parse_time_literal('T#2m'), 120000)

    def test_hours(self):
        self.assertEqual(parse_time_literal('T#1h'), 3600000)

    def test_days(self):
        self.assertEqual(parse_time_literal('T#1d'), 86400000)

    def test_composite(self):
        self.assertEqual(parse_time_literal('T#1m30s'), 90000)
        self.assertEqual(parse_time_literal('T#1h30m'), 5400000)

    def test_case_insensitive(self):
        self.assertEqual(parse_time_literal('t#10MS'), 10)
        self.assertEqual(parse_time_literal('T#10ms'), 10)

    def test_invalid(self):
        with self.assertRaises(ValueError):
            parse_time_literal('not_a_time')


# ── Parser Tests ────────────────────────────────────────────────────

class TestParser(unittest.TestCase):

    def _parse(self, src):
        tokens = Lexer(src).tokenize()
        return Parser(tokens).parse()

    MINIMAL = """\
PROGRAM test_prog
  VAR
    x : BOOL;
  END_VAR
  x := TRUE;
END_PROGRAM
CONFIGURATION c
  RESOURCE r ON PLC
    TASK t(INTERVAL := T#10ms, PRIORITY := 0);
    PROGRAM p WITH t : test_prog;
  END_RESOURCE
END_CONFIGURATION
"""

    def test_minimal_program(self):
        prog = self._parse(self.MINIMAL)
        self.assertEqual(prog.name, 'test_prog')
        self.assertEqual(len(prog.vars), 1)
        self.assertEqual(prog.vars[0].name, 'x')
        self.assertEqual(len(prog.body), 1)

    def test_assignment(self):
        prog = self._parse(self.MINIMAL)
        stmt = prog.body[0]
        self.assertIsInstance(stmt, StmtAssign)
        self.assertEqual(stmt.target, 'x')
        self.assertIsInstance(stmt.expr, ExprLitBool)
        self.assertTrue(stmt.expr.value)

    def test_int_var_with_init(self):
        src = """\
PROGRAM test_prog
  VAR
    count : INT := 42;
  END_VAR
  count := 0;
END_PROGRAM
CONFIGURATION c
  RESOURCE r ON PLC
    TASK t(INTERVAL := T#10ms, PRIORITY := 0);
    PROGRAM p WITH t : test_prog;
  END_RESOURCE
END_CONFIGURATION
"""
        prog = self._parse(src)
        self.assertEqual(prog.vars[0].name, 'count')
        self.assertIsInstance(prog.vars[0].init_value, ExprLitInt)
        self.assertEqual(prog.vars[0].init_value.value, 42)

    def test_if_elsif_else(self):
        src = """\
PROGRAM test_prog
  VAR
    x : BOOL;
    y : BOOL;
    z : INT := 0;
  END_VAR
  IF x THEN
    z := 1;
  ELSIF y THEN
    z := 2;
  ELSE
    z := 3;
  END_IF;
END_PROGRAM
CONFIGURATION c
  RESOURCE r ON PLC
    TASK t(INTERVAL := T#10ms, PRIORITY := 0);
    PROGRAM p WITH t : test_prog;
  END_RESOURCE
END_CONFIGURATION
"""
        prog = self._parse(src)
        stmt = prog.body[0]
        self.assertIsInstance(stmt, StmtIf)
        self.assertEqual(len(stmt.then_body), 1)
        self.assertEqual(len(stmt.elsifs), 1)
        self.assertEqual(len(stmt.else_body), 1)

    def test_for_loop(self):
        src = """\
PROGRAM test_prog
  VAR
    i : INT;
    s : INT := 0;
  END_VAR
  FOR i := 1 TO 10 DO
    s := s + 1;
  END_FOR;
END_PROGRAM
CONFIGURATION c
  RESOURCE r ON PLC
    TASK t(INTERVAL := T#10ms, PRIORITY := 0);
    PROGRAM p WITH t : test_prog;
  END_RESOURCE
END_CONFIGURATION
"""
        prog = self._parse(src)
        stmt = prog.body[0]
        self.assertIsInstance(stmt, StmtFor)
        self.assertEqual(stmt.var, 'i')

    def test_fb_call(self):
        src = """\
PROGRAM test_prog
  VAR
    clk : BOOL;
    edge : R_TRIG;
  END_VAR
  edge(CLK := clk);
END_PROGRAM
CONFIGURATION c
  RESOURCE r ON PLC
    TASK t(INTERVAL := T#10ms, PRIORITY := 0);
    PROGRAM p WITH t : test_prog;
  END_RESOURCE
END_CONFIGURATION
"""
        prog = self._parse(src)
        stmt = prog.body[0]
        self.assertIsInstance(stmt, StmtFBCall)
        self.assertEqual(stmt.instance, 'edge')
        self.assertEqual(len(stmt.args), 1)
        self.assertEqual(stmt.args[0].name, 'CLK')

    def test_field_access(self):
        src = """\
PROGRAM test_prog
  VAR
    clk : BOOL;
    out : BOOL;
    edge : R_TRIG;
  END_VAR
  edge(CLK := clk);
  out := edge.Q;
END_PROGRAM
CONFIGURATION c
  RESOURCE r ON PLC
    TASK t(INTERVAL := T#10ms, PRIORITY := 0);
    PROGRAM p WITH t : test_prog;
  END_RESOURCE
END_CONFIGURATION
"""
        prog = self._parse(src)
        stmt = prog.body[1]
        self.assertIsInstance(stmt.expr, ExprFieldAccess)
        self.assertEqual(stmt.expr.obj, 'edge')
        self.assertEqual(stmt.expr.field, 'Q')

    def test_expression_precedence(self):
        """OR has lower precedence than AND: a OR b AND c → a OR (b AND c)"""
        src = """\
PROGRAM test_prog
  VAR
    a : BOOL;
    b : BOOL;
    c : BOOL;
    r : BOOL;
  END_VAR
  r := a OR b AND c;
END_PROGRAM
CONFIGURATION c
  RESOURCE r ON PLC
    TASK t(INTERVAL := T#10ms, PRIORITY := 0);
    PROGRAM p WITH t : test_prog;
  END_RESOURCE
END_CONFIGURATION
"""
        prog = self._parse(src)
        expr = prog.body[0].expr
        # Top-level should be OR
        self.assertIsInstance(expr, ExprBinOp)
        self.assertEqual(expr.op, 'OR')
        # Left should be 'a'
        self.assertIsInstance(expr.left, ExprVar)
        # Right should be AND(b, c)
        self.assertIsInstance(expr.right, ExprBinOp)
        self.assertEqual(expr.right.op, 'AND')

    def test_tick_interval(self):
        src = """\
PROGRAM test_prog
  VAR x : BOOL; END_VAR
  x := TRUE;
END_PROGRAM
CONFIGURATION c
  RESOURCE r ON PLC
    TASK t(INTERVAL := T#20ms, PRIORITY := 0);
    PROGRAM p WITH t : test_prog;
  END_RESOURCE
END_CONFIGURATION
"""
        prog = self._parse(src)
        self.assertEqual(prog.task.interval_ms, 20)


# ── wrap_i16 Tests ──────────────────────────────────────────────────

class TestWrapI16(unittest.TestCase):

    def test_positive_within_range(self):
        self.assertEqual(wrap_i16(100), 100)

    def test_negative_within_range(self):
        self.assertEqual(wrap_i16(-100), -100)

    def test_overflow_wraps(self):
        self.assertEqual(wrap_i16(32768), -32768)

    def test_underflow_wraps(self):
        self.assertEqual(wrap_i16(-32769), 32767)

    def test_boundaries(self):
        self.assertEqual(wrap_i16(32767), 32767)
        self.assertEqual(wrap_i16(-32768), -32768)


# ── TLV Emitter Tests ──────────────────────────────────────────────

class TestTLVEmitter(unittest.TestCase):

    def _compile(self, src):
        return compile_file(src)

    def test_header_magic(self):
        """TLV header starts with 'LP' magic and version 1."""
        src = """\
PROGRAM test_prog
  VAR x : BOOL; END_VAR
  x := TRUE;
END_PROGRAM
CONFIGURATION c
  RESOURCE r ON PLC
    TASK t(INTERVAL := T#10ms, PRIORITY := 0);
    PROGRAM p WITH t : test_prog;
  END_RESOURCE
END_CONFIGURATION
"""
        _, tlv, _ = self._compile(src)
        self.assertEqual(tlv[0:2], b'LP')
        self.assertEqual(tlv[2], 1)  # version

    def test_header_pool_sizes(self):
        """Header encodes correct pool sizes."""
        src = """\
PROGRAM test_prog
  VAR
    a : BOOL;
    b : INT := 5;
    t : TON;
  END_VAR
  a := TRUE;
  b := b + 1;
  t(IN := a, PT := T#1s);
END_PROGRAM
CONFIGURATION c
  RESOURCE r ON PLC
    TASK t(INTERVAL := T#10ms, PRIORITY := 0);
    PROGRAM p WITH t : test_prog;
  END_RESOURCE
END_CONFIGURATION
"""
        _, tlv, emitter = self._compile(src)
        # n_bools at offset 4, n_ints at offset 5, n_ton at offset 6
        n_bools = tlv[4]
        n_ints = tlv[5]
        n_ton = tlv[6]
        self.assertGreaterEqual(n_bools, 1)  # at least 'a'
        self.assertGreaterEqual(n_ints, 1)   # at least 'b'
        self.assertEqual(n_ton, 1)           # exactly one TON

    def test_dead_variable_elimination(self):
        """Variables declared but never used don't get pool slots."""
        src = """\
PROGRAM test_prog
  VAR
    used : BOOL;
    unused : BOOL;
  END_VAR
  used := TRUE;
END_PROGRAM
CONFIGURATION c
  RESOURCE r ON PLC
    TASK t(INTERVAL := T#10ms, PRIORITY := 0);
    PROGRAM p WITH t : test_prog;
  END_RESOURCE
END_CONFIGURATION
"""
        _, tlv, emitter = self._compile(src)
        # 'unused' should NOT appear in the pool
        self.assertIn('USED', emitter.bool_vars)
        self.assertNotIn('UNUSED', emitter.bool_vars)

    def test_init_section_for_int_initializer(self):
        """INT variable with := N generates LOAD_INT in INIT section."""
        src = """\
PROGRAM test_prog
  VAR
    count : INT := 42;
  END_VAR
  count := count + 1;
END_PROGRAM
CONFIGURATION c
  RESOURCE r ON PLC
    TASK t(INTERVAL := T#10ms, PRIORITY := 0);
    PROGRAM p WITH t : test_prog;
  END_RESOURCE
END_CONFIGURATION
"""
        _, tlv, emitter = self._compile(src)
        # INIT section starts at byte 20
        # Should contain LOAD_INT opcode (0x03)
        init_len = struct.unpack_from('<H', tlv, 16)[0]
        init_section = tlv[20:20+init_len]
        self.assertGreater(init_len, 0)
        self.assertIn(0x03, init_section)  # TAG_LOAD_INT

    def test_fb_output_slots_always_allocated(self):
        """All FB output slots are allocated even if not read via field access."""
        src = """\
PROGRAM test_prog
  VAR
    inp : BOOL;
    r : BOOL;
    counter : CTU;
  END_VAR
  r := FALSE;
  counter(CU := inp, R := r, PV := 10);
END_PROGRAM
CONFIGURATION c
  RESOURCE r ON PLC
    TASK t(INTERVAL := T#10ms, PRIORITY := 0);
    PROGRAM p WITH t : test_prog;
  END_RESOURCE
END_CONFIGURATION
"""
        _, tlv, emitter = self._compile(src)
        # CTU has Q (BOOL) and CV (INT) outputs — check via fb_output_slots
        self.assertIn('COUNTER', emitter.fb_output_slots)
        slots = emitter.fb_output_slots['COUNTER']
        self.assertIn('Q', slots)
        self.assertIn('CV', slots)
        self.assertEqual(slots['Q'][0], 'bool')
        self.assertEqual(slots['CV'][0], 'int')


# ── B1 Byte-Identical Test ──────────────────────────────────────────

class TestB1ByteIdentical(unittest.TestCase):

    def test_b1_matches_hand_encoding(self):
        """Parser output for B1 must be byte-identical to hand-encoded TLV."""
        b1_path = os.path.join(BASE_DIR, 'benchmarks', 'b1_esd.st')
        if not os.path.exists(b1_path):
            self.skipTest('b1_esd.st not found')

        with open(b1_path) as f:
            source = f.read()
        _, parser_tlv, _ = compile_file(source, b1_path)

        # Read hand-encoded TLV from the C header
        hand_tlv_path = os.path.join(BASE_DIR, 'fstar', 'test', 'b1_esd.tlv.h')
        if not os.path.exists(hand_tlv_path):
            self.skipTest('b1_esd.tlv.h not found')

        # Extract bytes only from within the array definition
        import re
        with open(hand_tlv_path) as f:
            content = f.read()
        # Match the array body between { and };
        m = re.search(r'b1_esd_tlv\[\]\s*=\s*\{(.*?)\};', content, re.DOTALL)
        self.assertIsNotNone(m, "Could not find b1_esd_tlv array in header")
        array_body = m.group(1)
        # Strip C comments (/* ... */) before extracting hex to avoid
        # matching hex-like patterns in comment text (e.g. "0x012C")
        array_body = re.sub(r'/\*.*?\*/', '', array_body, flags=re.DOTALL)
        hex_values = re.findall(r'0x([0-9a-fA-F]{2})', array_body)
        hand_tlv = bytes(int(h, 16) for h in hex_values)

        self.assertEqual(len(parser_tlv), len(hand_tlv),
                         f"Length mismatch: parser={len(parser_tlv)}, hand={len(hand_tlv)}")
        self.assertEqual(parser_tlv, hand_tlv,
                         "Parser TLV does not match hand-encoded TLV byte-for-byte")


# ── All Benchmarks Parse ────────────────────────────────────────────

class TestAllBenchmarks(unittest.TestCase):

    BENCHMARKS = ['b1_esd.st', 'b2_bms.st', 'b3_conveyor.st',
                  'b4_shutdown.st', 'b5_stress.st']

    def test_all_benchmarks_compile(self):
        """All 5 benchmark .st files parse and emit TLV without errors."""
        bench_dir = os.path.join(BASE_DIR, 'benchmarks')
        for name in self.BENCHMARKS:
            path = os.path.join(bench_dir, name)
            if not os.path.exists(path):
                self.skipTest(f'{name} not found')
            with open(path) as f:
                source = f.read()
            with self.subTest(benchmark=name):
                prog, tlv, emitter = compile_file(source, path)
                self.assertIsNotNone(prog)
                self.assertGreater(len(tlv), 20)  # At least header

    def test_tlv_header_valid(self):
        """All benchmark TLVs have valid headers."""
        bench_dir = os.path.join(BASE_DIR, 'benchmarks')
        for name in self.BENCHMARKS:
            path = os.path.join(bench_dir, name)
            if not os.path.exists(path):
                continue
            with open(path) as f:
                source = f.read()
            with self.subTest(benchmark=name):
                _, tlv, _ = compile_file(source, path)
                # Check magic
                self.assertEqual(tlv[0:2], b'LP')
                self.assertEqual(tlv[2], 1)
                # Check init_len + body_len match total
                init_len = struct.unpack_from('<H', tlv, 16)[0]
                body_len = struct.unpack_from('<H', tlv, 18)[0]
                self.assertEqual(len(tlv), 20 + init_len + body_len)


# ── Type Checker Error Tests ────────────────────────────────────────

class TestTypeErrors(unittest.TestCase):

    def test_undeclared_variable(self):
        """Assignment to undeclared variable should raise."""
        src = """\
PROGRAM test_prog
  VAR x : BOOL; END_VAR
  y := TRUE;
END_PROGRAM
CONFIGURATION c
  RESOURCE r ON PLC
    TASK t(INTERVAL := T#10ms, PRIORITY := 0);
    PROGRAM p WITH t : test_prog;
  END_RESOURCE
END_CONFIGURATION
"""
        with self.assertRaises(Exception):
            compile_file(src)

    def test_type_mismatch_bool_to_int(self):
        """Assigning BOOL expression to INT variable should raise."""
        src = """\
PROGRAM test_prog
  VAR
    x : INT;
    y : BOOL;
  END_VAR
  x := y;
END_PROGRAM
CONFIGURATION c
  RESOURCE r ON PLC
    TASK t(INTERVAL := T#10ms, PRIORITY := 0);
    PROGRAM p WITH t : test_prog;
  END_RESOURCE
END_CONFIGURATION
"""
        with self.assertRaises(Exception):
            compile_file(src)


if __name__ == '__main__':
    unittest.main()
