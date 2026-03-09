#include "stm32f767xx_minimal.h"
#include "system_stm32f7xx.h"
#include "uart.h"
#include "lowplc_runtime.h"
#include "b1_tlv.h"

static void run_bringup(void) {
    uart_puts("\r\n=== LowPLC Bring-Up ===\r\n");

    /* Load and validate B1 TLV */
    uint32_t wcet = 0;
    lowplc_err_t result = lowplc_load_program(
        B1_TLV, B1_TLV_LEN,
        0xFFFFFFFFu,   /* no WCET limit for bring-up */
        &wcet
    );

    if (result != LOWPLC_OK) {
        uart_puts("FAIL: TLV validation failed code=");
        uart_putc('0' + (char)(-(int)result));
        uart_puts("\r\n");
        return;
    }
    uart_puts("PASS: TLV validated\r\n");

    /* Scenario 1: all inputs FALSE — no shutdown expected */
    lowplc_set_tick(0);
    lowplc_exec_scan();
    uart_puts("S1 SHUTDOWN_CMD=");
    uart_puts(lowplc_get_bool(6) ? "TRUE" : "FALSE");
    uart_puts(" ALARM_OUT=");
    uart_puts(lowplc_get_bool(7) ? "TRUE" : "FALSE");
    uart_puts("\r\n");

    /* Scenario 2: PRESSURE_HIGH=TRUE, TON not yet expired */
    lowplc_set_bool(0, true);
    lowplc_set_tick(1);
    lowplc_exec_scan();
    uart_puts("S2 SHUTDOWN_CMD=");
    uart_puts(lowplc_get_bool(6) ? "TRUE" : "FALSE");
    uart_puts(" ALARM_OUT=");
    uart_puts(lowplc_get_bool(7) ? "TRUE" : "FALSE");
    uart_puts("\r\n");

    /* Scenario 3: tick past TON PT (300 ticks) — alarm confirmed */
    lowplc_set_tick(400);
    lowplc_exec_scan();
    uart_puts("S3 SHUTDOWN_CMD=");
    uart_puts(lowplc_get_bool(6) ? "TRUE" : "FALSE");
    uart_puts(" ALARM_OUT=");
    uart_puts(lowplc_get_bool(7) ? "TRUE" : "FALSE");
    uart_puts("\r\n");

    uart_puts("=== Bring-Up Complete ===\r\n");
}

int main(void) {
#ifndef LOWPLC_RENODE
    SystemInit();
#endif
    uart_init(115200);
    run_bringup();
    while (1) { __WFI(); }
}
