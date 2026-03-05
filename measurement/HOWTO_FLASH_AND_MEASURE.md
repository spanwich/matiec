# HOWTO: Flash and Measure matiec Benchmarks on STM32F767ZI

## Prerequisites

- STM32F767ZI Nucleo-144 board (ST-LINK V2-1 integrated)
- `arm-none-eabi-gcc` toolchain (tested with 13.2.1)
- OpenOCD (0.12+)
- Python 3.8+ with `pyserial` (`pip install pyserial`)
- Optional: `matplotlib` for histogram PNGs

## Build

```bash
cd measurement
make all
```

Verify all 5 ELFs are < 64KB:
```bash
make sizes
```

## Flash a Benchmark

```bash
# Flash B1 (ESD interlock)
openocd -f interface/stlink-v2-1.cfg -f target/stm32f7x.cfg \
  -c "program b1/benchmark_b1.elf verify reset exit"

# Flash B2 (BMS purge)
openocd -f interface/stlink-v2-1.cfg -f target/stm32f7x.cfg \
  -c "program b2/benchmark_b2.elf verify reset exit"

# Flash B3 (Conveyor counter)
openocd -f interface/stlink-v2-1.cfg -f target/stm32f7x.cfg \
  -c "program b3/benchmark_b3.elf verify reset exit"

# Flash B4 (Shutdown sequence)
openocd -f interface/stlink-v2-1.cfg -f target/stm32f7x.cfg \
  -c "program b4/benchmark_b4.elf verify reset exit"

# Flash B5 (Stress test)
openocd -f interface/stlink-v2-1.cfg -f target/stm32f7x.cfg \
  -c "program b5/benchmark_b5.elf verify reset exit"
```

## Capture Results

The firmware outputs results over USART3 (ST-LINK virtual COM port) at
115200 baud.  The serial device is typically `/dev/ttyACM0` on Linux.

### Live capture:

```bash
python3 ../scripts/collect_results.py --serial /dev/ttyACM0 --latex
```

### Save raw output then process:

```bash
# Capture raw UART output
stty -F /dev/ttyACM0 115200 raw -echo
cat /dev/ttyACM0 > results/b1_raw.txt
# Press Ctrl-C after seeing "END"

# Process
python3 ../scripts/collect_results.py --file results/b1_raw.txt --latex --histogram results/b1_hist.png
```

## Expected UART Output Format

```
# matiec WCET benchmark: B1_ESD
BEGIN,B1_ESD
TOTAL_MIN,<cycles>
TOTAL_MAX,<cycles>
TOTAL_SUM_HI,<upper 32 bits>
TOTAL_SUM_LO,<lower 32 bits>
TOTAL_COUNT,10000
EXEC_MIN,<cycles>
EXEC_MAX,<cycles>
EXEC_SUM_HI,<upper 32 bits>
EXEC_SUM_LO,<lower 32 bits>
EXEC_COUNT,10000
HIST_BINS,256
HIST_CYCLES_PER_BIN,10
HIST,<256 comma-separated bin counts>
END
```

## GPIO Instrumentation

For oscilloscope verification:
- **PB0**: HIGH during entire scan cycle (input setup + body execution)
- **PB1**: HIGH during POU body execution only

These allow independent verification of cycle-count measurements.

## Results CSV Template

```csv
Benchmark,EXEC_MIN,EXEC_MAX,EXEC_MEAN,MIN_ns,MAX_ns,MEAN_ns
B1_ESD,,,,,
B2_BMS,,,,,
B3_CONVEYOR,,,,,
B4_SHUTDOWN,,,,,
B5_STRESS,,,,,
```

## D5: Per-Opcode Cost Measurement

### Build

```bash
cd measurement
make d5_opcodes
```

### Flash

```bash
openocd -f interface/stlink-v2-1.cfg -f target/stm32f7x.cfg \
  -c "program d5_opcodes/benchmark_d5_opcodes.elf verify reset exit"
```

### Collect Cost Table

```bash
# Live capture — writes directly to tools/cost_table.json
python3 ../scripts/collect_opcode_costs.py --serial /dev/ttyACM0

# Or from saved output
stty -F /dev/ttyACM0 115200 raw -echo
cat /dev/ttyACM0 > results/d5_raw.txt
python3 ../scripts/collect_opcode_costs.py --file results/d5_raw.txt
```

### Expected Output Format

```
# D5 per-opcode cost measurement
BEGIN_OPCODE_COSTS
TON_CALL,<min>,<max>
TOF_CALL,<min>,<max>
...
CMP_GE,<min>,<max>
END_OPCODE_COSTS
```

The collection script writes max values to `tools/cost_table.json`, which
feeds the WCET calculator (LowPLC.WCET.fst / firmware/lowplc_runtime.c).

---

## Troubleshooting

- **No serial output**: Check that the ST-LINK firmware is up to date
  (`st-flash --version`). Try resetting the board after flashing.
- **Garbled output**: Verify baud rate is 115200. Check that no other
  program (e.g., minicom) has the serial port open.
- **OpenOCD connection failed**: Ensure the ST-LINK USB cable is connected
  to the CN1 (ST-LINK) USB port, not CN13 (user USB).
