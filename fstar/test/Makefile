# Makefile for LowPLC FB equivalence tests
#
# NOTE: Must use -O0 -fsanitize=address,undefined because the matiec
# standard library (iec_std_FB.h) has undefined behaviour that causes
# incorrect results at other optimisation levels.  See test_fbs.c header.

CC      = gcc
CFLAGS  = -O0 -fsanitize=address,undefined -Wall -Wextra -Wno-unused-parameter
MATIEC  = ../../lib/C
BUILD   = ../build

LOWPLC_SRCS = $(wildcard $(BUILD)/LowPLC_*_Impl.c)

.PHONY: all clean test

all: test_fbs test_ton

test_fbs: test_fbs.c $(LOWPLC_SRCS)
	$(CC) $(CFLAGS) -I$(MATIEC) -I$(BUILD) -o $@ $^

test_ton: test_ton.c
	$(CC) $(CFLAGS) -I$(MATIEC) -I$(BUILD) -o $@ $<

test: test_fbs test_ton
	@echo "=== Running test_ton ==="
	./test_ton
	@echo ""
	@echo "=== Running test_fbs ==="
	./test_fbs

clean:
	rm -f test_fbs test_ton *.o debug_ctu
