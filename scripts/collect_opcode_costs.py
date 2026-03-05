#!/usr/bin/env python3
"""
collect_opcode_costs.py — Parse UART output from D5 per-opcode firmware
and write tools/cost_table.json with measured max values.

Usage:
    python3 scripts/collect_opcode_costs.py --serial /dev/ttyACM0
    python3 scripts/collect_opcode_costs.py --file results/d5_raw.txt

The firmware outputs:
    BEGIN_OPCODE_COSTS
    OPNAME,min,max
    ...
    END_OPCODE_COSTS
"""

import argparse
import json
import sys
import os

COST_TABLE_PATH = os.path.join(os.path.dirname(__file__), '..', 'tools', 'cost_table.json')


def parse_opcode_costs(lines):
    """Parse OPNAME,min,max lines between BEGIN/END markers."""
    costs = {}
    in_block = False
    for line in lines:
        line = line.strip()
        if line == 'BEGIN_OPCODE_COSTS':
            in_block = True
            continue
        if line == 'END_OPCODE_COSTS':
            break
        if not in_block:
            continue
        if line.startswith('#') or not line:
            continue
        parts = line.split(',')
        if len(parts) == 3:
            name, min_val, max_val = parts
            costs[name.strip()] = {
                'min': int(min_val.strip()),
                'max': int(max_val.strip()),
            }
    return costs


def read_from_serial(port, baudrate=115200):
    """Read UART output until END_OPCODE_COSTS is seen."""
    try:
        import serial
    except ImportError:
        print("ERROR: pyserial not installed. Run: pip install pyserial", file=sys.stderr)
        sys.exit(1)

    print(f"Listening on {port} @ {baudrate} baud...")
    ser = serial.Serial(port, baudrate, timeout=30)
    lines = []
    while True:
        raw = ser.readline()
        if not raw:
            continue
        line = raw.decode('ascii', errors='replace').strip()
        print(f"  {line}")
        lines.append(line)
        if line == 'END_OPCODE_COSTS':
            break
    ser.close()
    return lines


def read_from_file(path):
    """Read UART capture from a text file."""
    with open(path) as f:
        return f.readlines()


def update_cost_table(costs):
    """Write measured max values into tools/cost_table.json."""
    with open(COST_TABLE_PATH) as f:
        table = json.load(f)

    updated = 0
    for name, vals in costs.items():
        if name in table:
            table[name] = vals['max']
            updated += 1
        else:
            print(f"  WARNING: unknown opcode '{name}' in firmware output", file=sys.stderr)

    with open(COST_TABLE_PATH, 'w') as f:
        json.dump(table, f, indent=4)
        f.write('\n')

    return updated


def main():
    parser = argparse.ArgumentParser(description='Collect per-opcode DWT costs')
    group = parser.add_mutually_exclusive_group(required=True)
    group.add_argument('--serial', help='Serial port (e.g. /dev/ttyACM0)')
    group.add_argument('--file', help='Path to captured UART output file')
    parser.add_argument('--baudrate', type=int, default=115200)
    args = parser.parse_args()

    if args.serial:
        lines = read_from_serial(args.serial, args.baudrate)
    else:
        lines = read_from_file(args.file)

    costs = parse_opcode_costs(lines)
    if not costs:
        print("ERROR: No opcode costs found in input", file=sys.stderr)
        sys.exit(1)

    print(f"\nParsed {len(costs)} opcode costs:")
    print(f"  {'Operation':<15} {'Min':>6} {'Max':>6}")
    print(f"  {'-'*15} {'-'*6} {'-'*6}")
    for name, vals in sorted(costs.items()):
        print(f"  {name:<15} {vals['min']:>6} {vals['max']:>6}")

    updated = update_cost_table(costs)
    print(f"\nUpdated {updated} entries in {COST_TABLE_PATH}")


if __name__ == '__main__':
    main()
