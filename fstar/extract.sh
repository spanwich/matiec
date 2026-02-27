#!/bin/bash
# extract.sh — Extract LowPLC F* modules to C via KaRaMeL
#
# Usage: cd fstar/src && bash ../extract.sh
set -e

FSTAR_EXE=/home/iamfo470/phd/everparse/opt/FStar/out/bin/fstar.exe
KRML_HOME=/home/iamfo470/phd/everparse/opt/karamel
Z3_PATH=/home/iamfo470/phd/everparse/opt/z3/z3-4.13.3
BUILD_DIR=../build

echo "=== Step 1: Verify F* modules ==="
$FSTAR_EXE --smt "$Z3_PATH" LowPLC.TON.fst
echo ""
$FSTAR_EXE --smt "$Z3_PATH" \
  --include "$KRML_HOME/krmllib" \
  --include "$KRML_HOME/krmllib/obj" \
  LowPLC.TON.Impl.fst
echo ""

echo "=== Step 2: KaRaMeL extraction ==="
$KRML_HOME/krml \
  -fstar "$FSTAR_EXE" \
  -fsopt "--smt" -fsopt "$Z3_PATH" \
  -tmpdir "$BUILD_DIR" \
  -skip-compilation -skip-linking \
  -minimal \
  -no-prefix LowPLC.TON.Impl \
  -bundle 'LowPLC.TON.Impl=LowPLC.TON,LowPLC.TON.Impl,LowPLC.AST' \
  -drop 'FStar.*,LowStar.*,Prims' \
  -warn-error -2 \
  LowPLC.TON.Impl.fst
echo ""

echo "=== Step 3: Cross-compile for Cortex-M7 ==="
arm-none-eabi-gcc \
  -mcpu=cortex-m7 -mthumb -mfpu=fpv5-d16 -mfloat-abi=hard \
  -O2 -g -Wall -Wextra -Wno-unused-parameter \
  -c "$BUILD_DIR/LowPLC_TON_Impl.c" \
  -I "$BUILD_DIR" \
  -o "$BUILD_DIR/LowPLC_TON_Impl.o"
echo ""

echo "=== Results ==="
arm-none-eabi-size "$BUILD_DIR/LowPLC_TON_Impl.o"
echo ""
echo "Extracted to $BUILD_DIR/LowPLC_TON_Impl.c"
echo "Cross-compiled to $BUILD_DIR/LowPLC_TON_Impl.o"
