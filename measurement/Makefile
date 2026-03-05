# Makefile — matiec WCET benchmark suite for STM32F767ZI
#
# Usage:
#   make all        — build all 5 benchmark ELFs
#   make b1         — build just B1
#   make sizes      — print size summary
#   make clean      — remove all build artifacts

# ── Toolchain ──────────────────────────────────────────────────────────
PREFIX  := arm-none-eabi-
CC      := $(PREFIX)gcc
AS      := $(PREFIX)gcc
LD      := $(PREFIX)gcc
OBJCOPY := $(PREFIX)objcopy
SIZE    := $(PREFIX)size

# ── CPU flags ──────────────────────────────────────────────────────────
CPU_FLAGS := -mcpu=cortex-m7 -mthumb -mfpu=fpv5-d16 -mfloat-abi=hard

# ── Paths ──────────────────────────────────────────────────────────────
MATIEC_LIB := ../lib/C
BUILD_DIR  := ../build
FSTAR_BUILD := ../fstar/build
COMMON     := common

# ── Compiler / linker flags ────────────────────────────────────────────
CFLAGS := $(CPU_FLAGS) -O2 -g -Wall -Wextra \
          -I$(MATIEC_LIB) -I$(COMMON) \
          -DSTM32F767xx \
          -ffunction-sections -fdata-sections \
          -Wno-unused-parameter -Wno-unused-variable \
          -Wno-type-limits

ASFLAGS := $(CPU_FLAGS) -g

LDFLAGS := $(CPU_FLAGS) -T$(COMMON)/stm32f767zi.ld \
           -Wl,--gc-sections \
           -nostdlib -lnosys -lgcc \
           --specs=nosys.specs

# ── Benchmarks ─────────────────────────────────────────────────────────
BENCHMARKS := b1 b2 b3 b4 b5
ELFS := $(foreach b,$(BENCHMARKS),$(b)/benchmark_$(b).elf)

# ── Common objects ─────────────────────────────────────────────────────
COMMON_SRCS := $(COMMON)/startup_stm32f767xx.s \
               $(COMMON)/system_stm32f7xx.c \
               $(COMMON)/uart.c

.PHONY: all clean sizes $(BENCHMARKS) d5_opcodes

all: $(BENCHMARKS)
	@echo "=== All benchmarks built ==="
	@$(MAKE) --no-print-directory sizes

# ── Per-benchmark rules ────────────────────────────────────────────────
# Each benchmark compiles its main.c (which #includes POUS.c and
# wrapper_template.c) plus the common infrastructure.

define BENCH_RULE
$(1): $(1)/benchmark_$(1).elf

$(1)/benchmark_$(1).elf: $(1)/main.c wrapper_template.c \
                         $(COMMON_SRCS) \
                         $(COMMON)/stm32f767xx_minimal.h \
                         $(COMMON)/dwt.h $(COMMON)/gpio.h \
                         $(COMMON)/uart.h $(COMMON)/measurement.h \
                         $(COMMON)/system_stm32f7xx.h \
                         $(COMMON)/stm32f767zi.ld
	@echo "[BUILD] $$@"
	$(CC) $(CFLAGS) -I$(BUILD_DIR)/$(1) $(ASFLAGS) \
		$(COMMON)/startup_stm32f767xx.s \
		$(COMMON)/system_stm32f7xx.c \
		$(COMMON)/uart.c \
		$(1)/main.c \
		-o $$@ $(LDFLAGS)
	$(SIZE) $$@
endef

$(foreach b,$(BENCHMARKS),$(eval $(call BENCH_RULE,$(b))))

# ── D5 per-opcode measurement ─────────────────────────────────────────
# Measures KaRaMeL-extracted FB step functions + simple opcode costs.
# Uses fstar/build/ headers and sources (unity build).

d5_opcodes: d5_opcodes/benchmark_d5_opcodes.elf

d5_opcodes/benchmark_d5_opcodes.elf: d5_opcodes/main.c \
                                     $(COMMON_SRCS) \
                                     $(COMMON)/stm32f767xx_minimal.h \
                                     $(COMMON)/dwt.h $(COMMON)/uart.h \
                                     $(COMMON)/measurement.h \
                                     $(COMMON)/system_stm32f7xx.h \
                                     $(COMMON)/stm32f767zi.ld
	@echo "[BUILD] $@"
	$(CC) $(CFLAGS) -I$(FSTAR_BUILD) $(ASFLAGS) \
		$(COMMON)/startup_stm32f767xx.s \
		$(COMMON)/system_stm32f7xx.c \
		$(COMMON)/uart.c \
		d5_opcodes/main.c \
		-o $@ $(LDFLAGS)
	$(SIZE) $@

# ── Utility targets ────────────────────────────────────────────────────
sizes:
	@echo ""
	@echo "=== Benchmark ELF sizes ==="
	@for b in $(BENCHMARKS); do \
		if [ -f $$b/benchmark_$$b.elf ]; then \
			$(SIZE) $$b/benchmark_$$b.elf; \
		fi; \
	done

clean:
	rm -f $(foreach b,$(BENCHMARKS),$(b)/benchmark_$(b).elf)
	rm -f $(foreach b,$(BENCHMARKS),$(b)/*.o)
	rm -f d5_opcodes/benchmark_d5_opcodes.elf
