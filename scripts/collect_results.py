#!/usr/bin/env python3
"""
collect_results.py — Parse matiec WCET benchmark UART output.

Reads structured CSV output from the STM32F767ZI measurement firmware,
computes statistics, and produces LaTeX table rows and histogram PNGs.

Usage:
  # From serial port (live capture):
  python3 collect_results.py --serial /dev/ttyACM0 --baud 115200

  # From saved file:
  python3 collect_results.py --file results/b1_capture.txt

  # Test with stdin:
  cat results/b1_capture.txt | python3 collect_results.py --file -
"""
import argparse
import sys
import os

SYSCLK_HZ = 216_000_000  # 216 MHz


def parse_results(lines):
    """Parse structured UART CSV output into a results dict."""
    results = {}
    hist = []
    in_block = False

    for line in lines:
        line = line.strip()
        if not line or line.startswith('#'):
            continue

        parts = line.split(',', 1)
        key = parts[0]

        if key == 'BEGIN':
            in_block = True
            results['name'] = parts[1] if len(parts) > 1 else 'UNKNOWN'
            continue
        if key == 'END':
            break
        if not in_block:
            continue

        if key == 'HIST':
            hist = [int(x) for x in parts[1].split(',')]
            results['histogram'] = hist
        elif len(parts) == 2:
            results[key] = int(parts[1])

    # Reconstruct 64-bit sums
    if 'TOTAL_SUM_HI' in results and 'TOTAL_SUM_LO' in results:
        results['TOTAL_SUM'] = (results['TOTAL_SUM_HI'] << 32) | results['TOTAL_SUM_LO']
    if 'EXEC_SUM_HI' in results and 'EXEC_SUM_LO' in results:
        results['EXEC_SUM'] = (results['EXEC_SUM_HI'] << 32) | results['EXEC_SUM_LO']

    return results


def cycles_to_ns(cycles):
    """Convert DWT cycles to nanoseconds at SYSCLK_HZ."""
    return cycles * 1_000_000_000 / SYSCLK_HZ


def cycles_to_us(cycles):
    """Convert DWT cycles to microseconds at SYSCLK_HZ."""
    return cycles * 1_000_000 / SYSCLK_HZ


def print_stats(results):
    """Print formatted measurement statistics."""
    name = results.get('name', 'UNKNOWN')
    print(f"\n{'='*60}")
    print(f"  Benchmark: {name}")
    print(f"{'='*60}")

    for prefix, label in [('TOTAL', 'Total Scan'), ('EXEC', 'POU Body')]:
        mn = results.get(f'{prefix}_MIN', 0)
        mx = results.get(f'{prefix}_MAX', 0)
        sm = results.get(f'{prefix}_SUM', 0)
        cnt = results.get(f'{prefix}_COUNT', 1)
        avg = sm / cnt if cnt > 0 else 0

        print(f"\n  {label}:")
        print(f"    Min:  {mn:>8d} cycles  ({cycles_to_ns(mn):>10.1f} ns)")
        print(f"    Max:  {mx:>8d} cycles  ({cycles_to_ns(mx):>10.1f} ns)")
        print(f"    Mean: {avg:>8.1f} cycles  ({cycles_to_ns(avg):>10.1f} ns)")
        print(f"    Count: {cnt}")

    print()


def print_latex_row(results):
    """Print a LaTeX table row for the results."""
    name = results.get('name', 'UNKNOWN')
    e_min = results.get('EXEC_MIN', 0)
    e_max = results.get('EXEC_MAX', 0)
    e_sum = results.get('EXEC_SUM', 0)
    e_cnt = results.get('EXEC_COUNT', 1)
    e_avg = e_sum / e_cnt if e_cnt > 0 else 0

    print(f"% LaTeX row for {name}")
    print(f"{name} & {e_min} & {e_max} & {e_avg:.0f} & "
          f"{cycles_to_us(e_min):.2f} & {cycles_to_us(e_max):.2f} & "
          f"{cycles_to_us(e_avg):.2f} \\\\")


def save_histogram_png(results, output_path):
    """Generate histogram PNG using matplotlib."""
    try:
        import matplotlib
        matplotlib.use('Agg')
        import matplotlib.pyplot as plt
    except ImportError:
        print("  [WARN] matplotlib not available, skipping histogram PNG",
              file=sys.stderr)
        return

    hist = results.get('histogram', [])
    if not hist:
        return

    cpb = results.get('HIST_CYCLES_PER_BIN', 10)
    name = results.get('name', 'UNKNOWN')

    bins = range(len(hist))
    labels = [i * cpb for i in bins]

    fig, ax = plt.subplots(figsize=(12, 4))
    ax.bar(labels, hist, width=cpb * 0.9, color='steelblue', edgecolor='none')
    ax.set_xlabel('Cycles')
    ax.set_ylabel('Count')
    ax.set_title(f'{name} — Execution Time Histogram ({results.get("EXEC_COUNT", 0)} iterations)')
    ax.set_xlim(0, len(hist) * cpb)
    fig.tight_layout()
    fig.savefig(output_path, dpi=150)
    plt.close(fig)
    print(f"  Histogram saved: {output_path}")


def read_serial(port, baud):
    """Read lines from serial port until END marker."""
    try:
        import serial
    except ImportError:
        print("ERROR: pyserial not installed. Run: pip install pyserial",
              file=sys.stderr)
        sys.exit(1)

    lines = []
    with serial.Serial(port, baud, timeout=30) as ser:
        print(f"Listening on {port} @ {baud} baud...")
        while True:
            raw = ser.readline()
            if not raw:
                continue
            line = raw.decode('ascii', errors='replace').strip()
            print(f"  < {line}")
            lines.append(line)
            if line.startswith('END'):
                break
    return lines


def main():
    parser = argparse.ArgumentParser(
        description='Parse matiec WCET benchmark UART output')
    group = parser.add_mutually_exclusive_group(required=True)
    group.add_argument('--serial', metavar='PORT',
                       help='Serial port (e.g., /dev/ttyACM0)')
    group.add_argument('--file', metavar='PATH',
                       help='Input file path (use - for stdin)')
    parser.add_argument('--baud', type=int, default=115200,
                        help='Serial baud rate (default: 115200)')
    parser.add_argument('--histogram', metavar='PATH',
                        help='Output histogram PNG path')
    parser.add_argument('--latex', action='store_true',
                        help='Print LaTeX table row')
    args = parser.parse_args()

    if args.serial:
        lines = read_serial(args.serial, args.baud)
    else:
        if args.file == '-':
            lines = sys.stdin.readlines()
        else:
            with open(args.file) as f:
                lines = f.readlines()

    results = parse_results(lines)

    if not results:
        print("ERROR: No valid benchmark data found", file=sys.stderr)
        sys.exit(1)

    print_stats(results)

    if args.latex:
        print_latex_row(results)

    if args.histogram:
        save_histogram_png(results, args.histogram)
    elif 'histogram' in results:
        # Default: save next to input file if using --file
        name = results.get('name', 'benchmark').lower()
        default_path = f'{name}_histogram.png'
        save_histogram_png(results, default_path)


if __name__ == '__main__':
    main()
