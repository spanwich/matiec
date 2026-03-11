#!/usr/bin/env python3
"""
test_e2e.py — End-to-end integration test for LowPLC-ST parser.

For each benchmark (B1-B5):
  1. Parses the .st file with lowplcst_parser.py → TLV binary
  2. Generates a C test harness that runs both:
     - The TLV interpreter (lowplc_interp.h)
     - The matiec-generated code (POUS.c)
  3. Compiles and runs the test, comparing ALL output variables at each scan
  4. Runs 100 scan cycles with varying inputs

Usage:
    python3 tools/test_e2e.py [--benchmarks b1,b2,...] [--scans N]
"""

import argparse
import os
import struct
import subprocess
import sys
import tempfile

# Add tools to path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from lowplcst_parser import compile_file, TLVEmitter

BASE_DIR = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))

# ── Benchmark definitions ──

BENCHMARKS = {
    'b1': {
        'source': 'benchmarks/b1_esd.st',
        'build_dir': 'build/b1',
        'struct': 'ESD_INTERLOCK_data__',
        'init': 'ESD_INTERLOCK_init__',
        'body': 'ESD_INTERLOCK_body__',
        'inputs': [
            ('BOOL', 'PRESSURE_HIGH'),
            ('BOOL', 'TEMP_HIGH'),
            ('BOOL', 'FLOW_LOW'),
            ('BOOL', 'MANUAL_RESET'),
        ],
        'outputs': [
            ('BOOL', 'ANY_TRIP'),
            ('BOOL', 'LATCHED'),
            ('BOOL', 'SHUTDOWN_CMD'),
            ('BOOL', 'ALARM_OUT'),
        ],
        'tick_ms': 10,
    },
    'b2': {
        'source': 'benchmarks/b2_bms.st',
        'build_dir': 'build/b2',
        'struct': 'BMS_PURGE_data__',
        'init': 'BMS_PURGE_init__',
        'body': 'BMS_PURGE_body__',
        'inputs': [
            ('BOOL', 'START_CMD'),
            ('BOOL', 'FLAME_SENSOR'),
            ('BOOL', 'PURGE_OK'),
        ],
        'outputs': [
            ('INT', 'STAGE'),
            ('BOOL', 'FAN_ON'),
            ('BOOL', 'FUEL_VALVE'),
            ('BOOL', 'IGNITER_ON'),
            ('BOOL', 'ALARM_LOCKOUT'),
        ],
        'tick_ms': 20,
    },
    'b3': {
        'source': 'benchmarks/b3_conveyor.st',
        'build_dir': 'build/b3',
        'struct': 'CONVEYOR_COUNT_data__',
        'init': 'CONVEYOR_COUNT_init__',
        'body': 'CONVEYOR_COUNT_body__',
        'inputs': [
            ('BOOL', 'PART_DETECT'),
            ('BOOL', 'BATCH_RESET'),
            ('BOOL', 'RUN_CMD'),
            ('INT', 'BATCH_SIZE'),
        ],
        'outputs': [
            ('BOOL', 'MOTOR_ON'),
            ('BOOL', 'BATCH_COMPLETE'),
            ('INT', 'TOTAL_COUNT'),
            ('INT', 'BATCH_TOTAL'),
        ],
        'tick_ms': 10,
    },
    'b4': {
        'source': 'benchmarks/b4_shutdown.st',
        'build_dir': 'build/b4',
        'struct': 'SHUTDOWN_SEQ_data__',
        'init': 'SHUTDOWN_SEQ_init__',
        'body': 'SHUTDOWN_SEQ_body__',
        'inputs': [
            ('BOOL', 'SHUTDOWN_REQ'),
        ],
        'outputs': [
            ('INT', 'STAGE'),
            ('INT', 'VALVE_POS'),
            ('BOOL', 'HEATER_ON'),
            ('BOOL', 'PUMP_ON'),
            ('BOOL', 'VENT_OPEN'),
            ('BOOL', 'SAFE_STATE'),
        ],
        'tick_ms': 50,
    },
    'b5': {
        'source': 'benchmarks/b5_stress.st',
        'build_dir': 'build/b5',
        'struct': 'STRESS_TEST_data__',
        'init': 'STRESS_TEST_init__',
        'body': 'STRESS_TEST_body__',
        'inputs': [
            ('BOOL', 'INP_A'),
            ('BOOL', 'INP_B'),
            ('BOOL', 'INP_C'),
            ('BOOL', 'INP_D'),
            ('INT', 'INP_E'),
            ('INT', 'INP_F'),
            ('INT', 'INP_G'),
        ],
        'outputs': [
            ('BOOL', 'BOOL_RES'),
            ('INT', 'INT_RES'),
            ('INT', 'SUM_ACC'),
            ('BOOL', 'CMP_EQ'),
            ('BOOL', 'CMP_NE'),
            ('BOOL', 'CMP_LT'),
            ('BOOL', 'CMP_GT'),
            ('BOOL', 'CMP_LE'),
            ('BOOL', 'CMP_GE'),
            ('BOOL', 'OUT_ALARM'),
            ('INT', 'OUT_COUNT'),
            ('BOOL', 'OUT_TIMER_Q'),
            ('INT', 'OUT_RESULT'),
        ],
        'tick_ms': 10,
    },
}


def generate_tlv_header(tlv_bytes: bytes, name: str) -> str:
    """Generate a C header with the TLV binary as a uint8_t array."""
    lines = [f'static const uint8_t {name}_tlv[] = {{']
    for i in range(0, len(tlv_bytes), 16):
        chunk = tlv_bytes[i:i+16]
        hex_str = ', '.join(f'0x{b:02x}' for b in chunk)
        lines.append(f'    {hex_str},')
    lines.append('};')
    return '\n'.join(lines)


def generate_input_sequence(bench: dict, num_scans: int) -> list:
    """Generate a deterministic input sequence for testing."""
    inputs = bench['inputs']
    sequence = []
    for tick in range(1, num_scans + 1):
        values = {}
        for typ, name in inputs:
            if typ == 'BOOL':
                # Vary bools in interesting patterns
                h = hash((name, tick)) & 0xFFFFFFFF
                # Use different patterns for different inputs
                idx = [n for _, n in inputs if _ == 'BOOL'].index(name)
                if idx == 0:
                    values[name] = (tick % 7) < 3
                elif idx == 1:
                    values[name] = (tick % 11) < 5
                elif idx == 2:
                    values[name] = (tick % 13) < 6
                else:
                    values[name] = (tick % 17) < 8
            elif typ == 'INT':
                # Vary ints
                values[name] = ((tick * 7 + hash(name)) % 200) - 100
        sequence.append((tick, values))
    return sequence


def generate_test_c(bench_id: str, bench: dict, emitter: TLVEmitter,
                    tlv_header_name: str, num_scans: int) -> str:
    """Generate the C test harness source code."""
    inputs = bench['inputs']
    outputs = bench['outputs']
    struct_name = bench['struct']
    init_name = bench['init']
    body_name = bench['body']
    tick_ms = bench['tick_ms']

    # Build pool index maps from emitter
    bool_vars = emitter.bool_vars
    int_vars = emitter.int_vars

    lines = []
    lines.append('/* Auto-generated end-to-end test */')
    lines.append('#include <stdio.h>')
    lines.append('#include <stdlib.h>')
    lines.append('#include <string.h>')
    lines.append('#include <stdbool.h>')
    lines.append('#include <stdint.h>')
    lines.append('')
    lines.append('/* matiec headers */')
    lines.append('#include "iec_std_lib.h"')
    lines.append('#include "iec_std_FB.h"')
    lines.append('#include "POUS.h"')
    lines.append('')
    lines.append('TIME __CURRENT_TIME = {0, 0};')
    lines.append('')
    lines.append('#include "POUS.c"')
    lines.append('')
    lines.append('/* TLV interpreter */')
    lines.append(f'#include "{tlv_header_name}"')
    lines.append('#include "lowplc_interp.h"')
    lines.append('')
    lines.append('static int total = 0, passed = 0, failed = 0;')
    lines.append('')
    lines.append('#define CHECK(cond, fmt, ...) do { \\')
    lines.append('    total++; \\')
    lines.append('    if (cond) { passed++; } \\')
    lines.append('    else { \\')
    lines.append('        failed++; \\')
    lines.append('        printf("  FAIL [scan %d]: " fmt "\\n", tick, ##__VA_ARGS__); \\')
    lines.append('    } \\')
    lines.append('} while(0)')
    lines.append('')
    lines.append('int main(void) {')
    lines.append(f'    printf("=== E2E Test: {bench_id.upper()} ({num_scans} scans) ===\\n");')
    lines.append('')
    lines.append('    /* Parse TLV header */')
    lines.append('    tlv_header_t hdr;')
    lines.append(f'    int rc = parse_header({bench_id}_tlv, sizeof({bench_id}_tlv), &hdr);')
    lines.append('    if (rc != 0) { printf("Header parse failed: %d\\n", rc); return 1; }')
    lines.append('')
    lines.append('    /* Init both engines */')
    lines.append('    var_pool_t pool;')
    lines.append('    init_pool(&pool, &hdr);')
    lines.append(f'    pool.scan_cycle_period_us = {tick_ms * 1000};')
    lines.append(f'    exec_init({bench_id}_tlv, sizeof({bench_id}_tlv), &hdr, &pool, 1024);')
    lines.append('')
    lines.append(f'    {struct_name} matiec;')
    lines.append(f'    memset(&matiec, 0, sizeof(matiec));')
    lines.append(f'    {init_name}(&matiec, 0);')
    lines.append('')

    # Generate scan loop
    lines.append(f'    for (int scan = 0; scan < {num_scans}; scan++) {{')
    lines.append('        int tick = scan + 1;')
    lines.append('')

    # Set inputs with deterministic patterns
    for i, (typ, name) in enumerate(inputs):
        name_upper = name.upper()
        if typ == 'BOOL':
            bool_patterns = [
                '(tick % 7) < 3',
                '(tick % 11) < 5',
                '(tick % 13) < 6',
                '(tick % 17) < 8',
            ]
            bool_idx = [n for t, n in inputs if t == 'BOOL'].index(name)
            pattern = bool_patterns[bool_idx % len(bool_patterns)]
            val_expr = f'({pattern})'

            if name_upper in bool_vars:
                pool_idx = bool_vars[name_upper]
                lines.append(f'        pool.bools[{pool_idx}] = {val_expr};')
            lines.append(f'        __SET_VAR(matiec.,{name_upper},,{val_expr});')

        elif typ == 'INT':
            val_expr = f'(int16_t)(((tick * 7 + {hash(name) & 0xFFFF}) % 200) - 100)'
            if name_upper in int_vars:
                pool_idx = int_vars[name_upper]
                lines.append(f'        pool.ints[{pool_idx}] = {val_expr};')
            lines.append(f'        __SET_VAR(matiec.,{name_upper},,{val_expr});')

    # Set tick/time
    lines.append(f'        pool.tick = tick;')
    lines.append(f'        uint32_t total_ms = (uint32_t)tick * {tick_ms};')
    lines.append('        __CURRENT_TIME.tv_sec  = total_ms / 1000;')
    lines.append('        __CURRENT_TIME.tv_nsec = (total_ms % 1000) * 1000000;')
    lines.append('')

    # Execute both
    lines.append(f'        exec_scan({bench_id}_tlv, sizeof({bench_id}_tlv), &hdr, &pool, 65536);')
    lines.append(f'        {body_name}(&matiec);')
    lines.append('')

    # Compare outputs
    for typ, name in outputs:
        name_upper = name.upper()
        if typ == 'BOOL':
            if name_upper in bool_vars:
                pool_idx = bool_vars[name_upper]
                lines.append(f'        CHECK(pool.bools[{pool_idx}] == __GET_VAR(matiec.{name_upper},),')
                lines.append(f'              "{name}: TLV=%d matiec=%d", pool.bools[{pool_idx}], __GET_VAR(matiec.{name_upper},));')
        elif typ == 'INT':
            if name_upper in int_vars:
                pool_idx = int_vars[name_upper]
                lines.append(f'        CHECK(pool.ints[{pool_idx}] == __GET_VAR(matiec.{name_upper},),')
                lines.append(f'              "{name}: TLV=%d matiec=%d", pool.ints[{pool_idx}], __GET_VAR(matiec.{name_upper},));')

    lines.append('    }')
    lines.append('')
    lines.append('    printf("Results: %d/%d passed", passed, total);')
    lines.append('    if (failed > 0) printf(" (%d FAILED)", failed);')
    lines.append('    printf("\\n");')
    lines.append('    return failed > 0 ? 1 : 0;')
    lines.append('}')
    return '\n'.join(lines)


def run_benchmark_test(bench_id: str, num_scans: int, verbose: bool = False) -> bool:
    """Run the end-to-end test for one benchmark. Returns True on success."""
    bench = BENCHMARKS[bench_id]
    source_path = os.path.join(BASE_DIR, bench['source'])
    build_dir = os.path.join(BASE_DIR, bench['build_dir'])
    matiec_lib = os.path.join(BASE_DIR, 'lib', 'C')
    interp_dir = os.path.join(BASE_DIR, 'fstar', 'test')
    build_low_dir = os.path.join(BASE_DIR, 'fstar', 'build_low')
    krml_inc = os.path.join(os.path.expanduser('~'), 'phd', 'everparse', 'opt', 'karamel', 'include')
    krml_min = os.path.join(os.path.expanduser('~'), 'phd', 'everparse', 'opt', 'karamel', 'krmllib', 'dist', 'minimal')

    # Step 1: Parse and emit TLV
    with open(source_path) as f:
        source = f.read()
    prog, tlv, emitter = compile_file(source, source_path)

    if verbose:
        print(f"  Parsed {bench_id}: {len(tlv)} bytes TLV")

    # Step 2: Generate files in a temp directory
    with tempfile.TemporaryDirectory() as tmpdir:
        # Write TLV header
        tlv_h = generate_tlv_header(tlv, bench_id)
        tlv_h_path = os.path.join(tmpdir, f'{bench_id}.tlv.h')
        with open(tlv_h_path, 'w') as f:
            f.write(tlv_h)

        # Generate C test
        test_c = generate_test_c(bench_id, bench, emitter, f'{bench_id}.tlv.h', num_scans)
        test_c_path = os.path.join(tmpdir, f'test_{bench_id}.c')
        with open(test_c_path, 'w') as f:
            f.write(test_c)

        # Step 3: Compile
        exe_path = os.path.join(tmpdir, f'test_{bench_id}')
        extracted_c = os.path.join(build_low_dir, 'LowPLC_Interp_Low.c')
        compile_cmd = [
            'gcc', '-O0', '-fsanitize=address,undefined',
            '-Wall', '-Wextra', '-Wno-unused-parameter',
            f'-I{matiec_lib}', f'-I{build_dir}', f'-I{interp_dir}', f'-I{tmpdir}',
            f'-I{build_low_dir}', f'-I{krml_inc}', f'-I{krml_min}',
            '-o', exe_path, test_c_path, extracted_c,
        ]

        if verbose:
            print(f"  Compiling: {' '.join(compile_cmd)}")

        result = subprocess.run(compile_cmd, capture_output=True, text=True)
        if result.returncode != 0:
            print(f"  COMPILE ERROR for {bench_id}:")
            print(result.stderr)
            return False

        # Step 4: Run
        result = subprocess.run([exe_path], capture_output=True, text=True, timeout=30)
        print(f"  {result.stdout.strip()}")
        if result.returncode != 0:
            if result.stderr:
                print(f"  STDERR: {result.stderr.strip()}")
            return False
        return True


def main():
    ap = argparse.ArgumentParser(description='LowPLC E2E integration test')
    ap.add_argument('--benchmarks', default='b1,b2,b3,b4,b5',
                    help='Comma-separated benchmark IDs')
    ap.add_argument('--scans', type=int, default=100,
                    help='Number of scan cycles per benchmark')
    ap.add_argument('-v', '--verbose', action='store_true')
    args = ap.parse_args()

    benchmarks = [b.strip() for b in args.benchmarks.split(',')]
    total_ok = 0
    total_fail = 0

    for bid in benchmarks:
        if bid not in BENCHMARKS:
            print(f"Unknown benchmark: {bid}")
            total_fail += 1
            continue

        print(f"\n[{bid.upper()}] Running {args.scans}-scan E2E test...")
        try:
            ok = run_benchmark_test(bid, args.scans, args.verbose)
            if ok:
                total_ok += 1
            else:
                total_fail += 1
        except Exception as e:
            print(f"  ERROR: {e}")
            total_fail += 1

    print(f"\n{'='*50}")
    print(f"E2E Results: {total_ok}/{total_ok + total_fail} benchmarks passed")
    if total_fail > 0:
        print(f"  {total_fail} FAILED")
    sys.exit(1 if total_fail > 0 else 0)


if __name__ == '__main__':
    main()
