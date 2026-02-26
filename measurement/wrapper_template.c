/*
 * wrapper_template.c — Shared measurement loop for matiec benchmark suite
 *
 * Each benchmark's main.c #includes this file after defining:
 *   - BENCH_NAME    (string, e.g. "B1_ESD")
 *   - plc_init_wrapper()    — calls POU_init__()
 *   - plc_body_wrapper()    — calls POU_body__()
 *   - set_worst_case_inputs() — sets up worst-case inputs before each iteration
 *
 * Measurement protocol:
 *   1. 10,000 iterations with DWT cycle counting
 *   2. GPIO PB0 toggles around entire scan (init overhead + body)
 *   3. GPIO PB1 toggles around POU body only
 *   4. CSV output over UART for host-side collection
 */
#include "dwt.h"
#include "gpio.h"
#include "uart.h"
#include "measurement.h"
#include "system_stm32f7xx.h"

/* These must be provided by each benchmark's main.c */
extern void plc_init_wrapper(void);
extern void plc_body_wrapper(void);
extern void set_worst_case_inputs(void);

static measurement_t meas_total;
static measurement_t meas_exec;

static void emit_results(void) {
    uart_puts("BEGIN,");
    uart_puts(BENCH_NAME);
    uart_puts("\r\n");

    /* Total scan (PB0) */
    uart_puts("TOTAL_MIN,");
    uart_putu32(meas_total.min);
    uart_puts("\r\n");
    uart_puts("TOTAL_MAX,");
    uart_putu32(meas_total.max);
    uart_puts("\r\n");
    uart_puts("TOTAL_SUM_HI,");
    uart_putu32((uint32_t)(meas_total.sum >> 32));
    uart_puts("\r\n");
    uart_puts("TOTAL_SUM_LO,");
    uart_putu32((uint32_t)(meas_total.sum & 0xFFFFFFFFUL));
    uart_puts("\r\n");
    uart_puts("TOTAL_COUNT,");
    uart_putu32(meas_total.count);
    uart_puts("\r\n");

    /* Exec only (PB1) */
    uart_puts("EXEC_MIN,");
    uart_putu32(meas_exec.min);
    uart_puts("\r\n");
    uart_puts("EXEC_MAX,");
    uart_putu32(meas_exec.max);
    uart_puts("\r\n");
    uart_puts("EXEC_SUM_HI,");
    uart_putu32((uint32_t)(meas_exec.sum >> 32));
    uart_puts("\r\n");
    uart_puts("EXEC_SUM_LO,");
    uart_putu32((uint32_t)(meas_exec.sum & 0xFFFFFFFFUL));
    uart_puts("\r\n");
    uart_puts("EXEC_COUNT,");
    uart_putu32(meas_exec.count);
    uart_puts("\r\n");

    /* Histograms: EXEC only (exec is the primary metric) */
    uart_puts("HIST_BINS,");
    uart_putu32(MEAS_HIST_BINS);
    uart_puts("\r\n");
    uart_puts("HIST_CYCLES_PER_BIN,");
    uart_putu32(MEAS_CYCLES_PER_BIN);
    uart_puts("\r\n");
    uart_puts("HIST,");
    for (uint32_t i = 0; i < MEAS_HIST_BINS; i++) {
        if (i > 0) uart_putc(',');
        uart_putu32(meas_exec.histogram[i]);
    }
    uart_puts("\r\n");

    uart_puts("END\r\n");
}

int main(void) {
    uart_init(115200);
    dwt_init();
    gpio_instr_init();

    uart_puts("# matiec WCET benchmark: ");
    uart_puts(BENCH_NAME);
    uart_puts("\r\n");

    /* Initialise the POU once */
    plc_init_wrapper();

    /* Reset measurement accumulators */
    meas_reset(&meas_total);
    meas_reset(&meas_exec);

    for (uint32_t iter = 0; iter < MEAS_ITERATIONS; iter++) {
        /* Set up worst-case inputs before each scan */
        set_worst_case_inputs();

        /* ── Total scan measurement (PB0) ─────────────────────────── */
        SCAN_START();
        uint32_t t0 = dwt_cycles();

        /* ── POU body measurement (PB1) ───────────────────────────── */
        EXEC_START();
        uint32_t e0 = dwt_cycles();

        plc_body_wrapper();

        uint32_t e1 = dwt_cycles();
        EXEC_END();

        uint32_t t1 = dwt_cycles();
        SCAN_END();

        meas_record(&meas_total, t1 - t0);
        meas_record(&meas_exec,  e1 - e0);
    }

    emit_results();

    /* Halt */
    for (;;)
        __WFI();
}
