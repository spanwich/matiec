/* test_lowplc_tlv.c — Validate benchmark TLVs and reject malformed inputs
 *
 * Tests:
 *   1. All 5 benchmark TLVs are accepted by the EverParse validator
 *   2. Malformed inputs are rejected:
 *      - Bad magic bytes
 *      - Pool count exceeding MAX
 *      - Unknown opcode tag
 *      - Truncated input
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "LowPLC_TLVWrapper.h"

/* Required by EverParse-generated wrapper */
void LowPLC_TLVEverParseError(const char *StructName, const char *FieldName,
                               const char *Reason) {
    (void)StructName; (void)FieldName; (void)Reason;
}

static int pass = 0, fail = 0;

static void check(const char *name, uint8_t *buf, uint32_t len, int expect_valid) {
    BOOLEAN result = LowPlcTlvCheckLowPlcprogram(buf, len);
    int ok = (result != 0) == expect_valid;
    if (ok) {
        pass++;
        printf("  PASS %-30s %s\n", name, expect_valid ? "ACCEPTED" : "REJECTED");
    } else {
        fail++;
        printf("  FAIL %-30s expected %s, got %s\n", name,
               expect_valid ? "ACCEPT" : "REJECT",
               result ? "ACCEPT" : "REJECT");
    }
}

static uint8_t *read_file(const char *path, uint32_t *out_len) {
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    fseek(f, 0, SEEK_SET);
    uint8_t *buf = (uint8_t *)malloc(sz);
    if (buf) {
        fread(buf, 1, sz, f);
        *out_len = (uint32_t)sz;
    }
    fclose(f);
    return buf;
}

/* Generate a TLV from a .st source file using the Python parser */
static uint8_t *generate_tlv(const char *st_path, uint32_t *out_len) {
    char cmd[512];
    snprintf(cmd, sizeof(cmd),
        "python3 -c \""
        "from tools.lowplcst_parser import compile_file; "
        "f = open('%s'); src = f.read(); f.close(); "
        "_, tlv, _ = compile_file(src, '%s'); "
        "import sys; sys.stdout.buffer.write(tlv)\"",
        st_path, st_path);

    FILE *p = popen(cmd, "r");
    if (!p) return NULL;

    uint8_t *buf = (uint8_t *)malloc(65536);
    size_t total = 0;
    size_t n;
    while ((n = fread(buf + total, 1, 65536 - total, p)) > 0)
        total += n;
    pclose(p);

    *out_len = (uint32_t)total;
    return buf;
}

int main(int argc, char *argv[]) {
    printf("=== EverParse LowPLC_TLV Validation Test ===\n\n");

    /* --- Test 1: All 5 benchmark TLVs accepted --- */
    const char *benchmarks[] = {
        "benchmarks/b1_esd.st",
        "benchmarks/b2_bms.st",
        "benchmarks/b3_conveyor.st",
        "benchmarks/b4_shutdown.st",
        "benchmarks/b5_stress.st",
    };

    printf("[Benchmark TLVs — must all be ACCEPTED]\n");
    for (int i = 0; i < 5; i++) {
        uint32_t len = 0;
        uint8_t *tlv = generate_tlv(benchmarks[i], &len);
        if (!tlv || len == 0) {
            printf("  SKIP %-30s (could not generate TLV)\n", benchmarks[i]);
            continue;
        }
        check(benchmarks[i], tlv, len, 1);
        free(tlv);
    }

    /* --- Test 2: Malformed inputs rejected --- */
    printf("\n[Malformed inputs — must all be REJECTED]\n");

    /* 2a: Bad magic */
    {
        uint8_t bad_magic[] = {
            0x00, 0x00,       /* wrong magic */
            0x01, 0x00,
            0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00,
            0x00, 0x00,       /* init_len = 0 */
            0x00, 0x00,       /* body_len = 0 */
        };
        check("bad_magic", bad_magic, sizeof(bad_magic), 0);
    }

    /* 2b: Pool count exceeding MAX (n_bools = 200 > 64) */
    {
        uint8_t bad_pool[] = {
            0x4C, 0x50,       /* "LP" */
            0x01, 0x00,
            0xC8, 0x00,       /* n_bools = 200 (> 64) */
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00,
            0x00, 0x00,
            0x00, 0x00,
        };
        check("pool_overflow", bad_pool, sizeof(bad_pool), 0);
    }

    /* 2c: Unknown opcode tag in body */
    {
        uint8_t bad_tag[] = {
            0x4C, 0x50,       /* "LP" */
            0x01, 0x00,
            0x01, 0x00,       /* n_bools=1, n_ints=0 */
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00,
            0x00, 0x00,       /* init_len = 0 */
            0x04, 0x00,       /* body_len = 4 */
            0xFF, 0x01, 0x00, 0x00,  /* unknown tag 0xFF */
        };
        check("unknown_tag", bad_tag, sizeof(bad_tag), 0);
    }

    /* 2d: Truncated input (header says body_len=10 but only 2 bytes follow) */
    {
        uint8_t truncated[] = {
            0x4C, 0x50,
            0x01, 0x00,
            0x01, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00,
            0x00, 0x00,       /* init_len = 0 */
            0x0A, 0x00,       /* body_len = 10 */
            0x01, 0x01,       /* only 2 bytes provided */
        };
        check("truncated", truncated, sizeof(truncated), 0);
    }

    printf("\n=== Results: %d/%d passed ===\n", pass, pass + fail);
    return fail > 0 ? 1 : 0;
}
