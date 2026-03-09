# firmware/Makefile — LowPLC Renode bring-up build

PREFIX  := arm-none-eabi-
CC      := $(PREFIX)gcc
SIZE    := $(PREFIX)size

COMMON  := ../measurement/common
EVERPARSE := ../everparse
FSTAR_BUILD := ../fstar/build
INTERP  := ../fstar/test

CPU_FLAGS := -mcpu=cortex-m7 -mthumb -mfpu=fpv5-d16 -mfloat-abi=hard

MATIEC_LIB := ../lib/C

CFLAGS := $(CPU_FLAGS) -O0 -g3 -Wall -Wextra \
          -I$(COMMON) -I. -I$(EVERPARSE) -I$(FSTAR_BUILD) -I$(INTERP) \
          -I$(MATIEC_LIB) \
          -DSTM32F767xx -DLOWPLC_RENODE \
          -D_WIN32 \
          '-DKRML_HOST_PRINTF(...)=((void)0)' \
          '-DKRML_HOST_EXIT(x)=while(1){}' \
          -ffunction-sections -fdata-sections \
          -Wno-unused-parameter -Wno-unused-variable \
          -Wno-type-limits -Wno-sign-compare

LDFLAGS := $(CPU_FLAGS) -T$(COMMON)/stm32f767zi.ld \
           -Wl,--gc-sections \
           -nostdlib --specs=nosys.specs -lgcc

SRCS := main_bringup.c \
        lowplc_runtime.c \
        $(COMMON)/system_stm32f7xx.c \
        $(COMMON)/uart.c \
        $(EVERPARSE)/LowPLC_TLV.c \
        $(EVERPARSE)/LowPLC_TLVWrapper.c

ASM := $(COMMON)/startup_stm32f767xx.s

.PHONY: bringup clean

bringup: lowplc_bringup.elf

lowplc_bringup.elf: b1_tlv.h $(SRCS) $(ASM)
	$(CC) $(CFLAGS) $(SRCS) $(ASM) -o $@ $(LDFLAGS)
	$(SIZE) $@

b1_tlv.h: ../build/b1_esd.tlv ../tools/gen_tlv_header.py
	python3 ../tools/gen_tlv_header.py $< B1_TLV > $@

../build/b1_esd.tlv: ../benchmarks/b1_esd.st
	mkdir -p ../build
	python3 ../tools/lowplcst_parser.py $< -o $@

clean:
	rm -f lowplc_bringup.elf b1_tlv.h
