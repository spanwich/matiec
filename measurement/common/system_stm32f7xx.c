/*
 * system_stm32f7xx.c — Clock configuration for STM32F767ZI
 *
 * HSE(8 MHz) → PLL(M=8, N=432, P=2, Q=9) → SYSCLK = 216 MHz
 * AHB = 216 MHz (/1), APB1 = 54 MHz (/4), APB2 = 108 MHz (/2)
 * Flash latency: 7 wait states (for 216 MHz @ 3.3V)
 *
 * Also provides __CURRENT_TIME (required by matiec timer FBs) and
 * stub implementations for libc functions referenced by iec_std_lib.h.
 */
#include "stm32f767xx_minimal.h"
#include "system_stm32f7xx.h"
#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>

/* Required by matiec-generated code: extern TIME __CURRENT_TIME; */
#include "iec_types_all.h"
TIME __CURRENT_TIME = {0, 0};

void SystemInit(void) {
    /* ── Enable PWR clock and set voltage scale 1 ────────────────────── */
    RCC->APB1ENR |= RCC_APB1ENR_PWREN_Msk;
    (void)RCC->APB1ENR;
    PWR->CR1 |= (3UL << PWR_CR1_VOS_Pos);  /* Scale 1 (highest perf) */

    /* ── Enable HSE and wait for ready ───────────────────────────────── */
    RCC->CR |= RCC_CR_HSEON_Msk;
    while (!(RCC->CR & RCC_CR_HSERDY_Msk))
        ;

    /* ── Configure PLL: HSE/8 * 432 / 2 = 216 MHz ───────────────────── */
    RCC->PLLCFGR = (8U   << RCC_PLLCFGR_PLLM_Pos)   /* M = 8   */
                 | (432U << RCC_PLLCFGR_PLLN_Pos)    /* N = 432 */
                 | (0U   << RCC_PLLCFGR_PLLP_Pos)    /* P = 2 (0 means /2) */
                 | RCC_PLLCFGR_PLLSRC_HSE             /* Source = HSE */
                 | (9U   << RCC_PLLCFGR_PLLQ_Pos);   /* Q = 9   */

    /* ── Enable PLL and wait for lock ────────────────────────────────── */
    RCC->CR |= RCC_CR_PLLON_Msk;
    while (!(RCC->CR & RCC_CR_PLLRDY_Msk))
        ;

    /* ── Enable overdrive for 216 MHz ────────────────────────────────── */
    PWR->CR1 |= PWR_CR1_ODEN_Msk;
    while (!(PWR->CSR1 & PWR_CSR1_ODRDY_Msk))
        ;
    PWR->CR1 |= PWR_CR1_ODSWEN_Msk;
    while (!(PWR->CSR1 & PWR_CSR1_ODSWRDY_Msk))
        ;

    /* ── Flash latency: 7 WS + prefetch + ART accelerator ───────────── */
    FLASH->ACR = FLASH_ACR_LATENCY_7WS
               | FLASH_ACR_PRFTEN_Msk
               | FLASH_ACR_ARTEN_Msk;

    /* ── Bus prescalers: AHB/1, APB1/4, APB2/2 ──────────────────────── */
    RCC->CFGR = RCC_CFGR_HPRE_DIV1
              | RCC_CFGR_PPRE1_DIV4
              | RCC_CFGR_PPRE2_DIV2;

    /* ── Switch system clock to PLL ──────────────────────────────────── */
    RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_SW_Msk) | RCC_CFGR_SW_PLL;
    while ((RCC->CFGR & RCC_CFGR_SWS_Msk) != RCC_CFGR_SWS_PLL)
        ;
}

/* ── Minimal libc stubs (bare-metal, no libc) ────────────────────────── */

void *memset(void *s, int c, size_t n) {
    unsigned char *p = (unsigned char *)s;
    while (n--) *p++ = (unsigned char)c;
    return s;
}

void *memcpy(void *dest, const void *src, size_t n) {
    unsigned char *d = (unsigned char *)dest;
    const unsigned char *ss = (const unsigned char *)src;
    while (n--) *d++ = *ss++;
    return dest;
}

int memcmp(const void *s1, const void *s2, size_t n) {
    const unsigned char *a = (const unsigned char *)s1;
    const unsigned char *b = (const unsigned char *)s2;
    while (n--) {
        if (*a != *b) return *a - *b;
        a++; b++;
    }
    return 0;
}

/* ── Stubs for iec_std_lib.h libc redirections ───────────────────────── */

int iec_lib_snprintf(char *s, size_t maxlen, const char *fmt, ...) {
    (void)fmt;
    if (maxlen > 0) s[0] = '\0';
    return 0;
}

double iec_lib_acos(double x)  { (void)x; return 0.0; }
double iec_lib_asin(double x)  { (void)x; return 0.0; }
double iec_lib_atan(double x)  { (void)x; return 0.0; }
double iec_lib_cos(double x)   { (void)x; return 0.0; }
double iec_lib_exp(double x)   { (void)x; return 0.0; }
double iec_lib_fmod(double x, double y) { (void)x; (void)y; return 0.0; }
double iec_lib_log(double x)   { (void)x; return 0.0; }
double iec_lib_log10(double x) { (void)x; return 0.0; }
double iec_lib_pow(double x, double y)  { (void)x; (void)y; return 0.0; }
double iec_lib_sin(double x)   { (void)x; return 0.0; }
double iec_lib_sqrt(double x)  { (void)x; return 0.0; }
double iec_lib_tan(double x)   { (void)x; return 0.0; }
