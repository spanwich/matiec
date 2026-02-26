/*
 * stm32f767xx_minimal.h — Minimal register definitions for STM32F767ZI
 *
 * Contains ONLY the peripherals needed for DWT cycle-counting, UART output,
 * GPIO instrumentation, and clock configuration.  This replaces the full
 * CMSIS stm32f767xx.h header when it is not installed.
 *
 * Reference: STM32F76xxx Reference Manual (RM0410), STM32F767ZI datasheet.
 */
#ifndef STM32F767XX_MINIMAL_H
#define STM32F767XX_MINIMAL_H

#include <stdint.h>

/* ── Cortex-M7 Core Debug ─────────────────────────────────────────────── */

typedef struct {
    volatile uint32_t DHCSR;    /* 0x00  Debug Halting Control and Status */
    volatile uint32_t DCRSR;    /* 0x04  Debug Core Register Selector    */
    volatile uint32_t DCRDR;    /* 0x08  Debug Core Register Data        */
    volatile uint32_t DEMCR;    /* 0x0C  Debug Exception and Monitor Ctrl*/
} CoreDebug_Type;

#define CoreDebug_BASE      0xE000EDF0UL
#define CoreDebug           ((CoreDebug_Type *)CoreDebug_BASE)

#define CoreDebug_DEMCR_TRCENA_Pos  24U
#define CoreDebug_DEMCR_TRCENA_Msk  (1UL << CoreDebug_DEMCR_TRCENA_Pos)

/* ── Data Watchpoint and Trace (DWT) ──────────────────────────────────── */

typedef struct {
    volatile uint32_t CTRL;     /* 0x00  Control Register                */
    volatile uint32_t CYCCNT;   /* 0x04  Cycle Count Register            */
    volatile uint32_t CPICNT;   /* 0x08  CPI Count Register              */
    volatile uint32_t EXCCNT;   /* 0x0C  Exception Overhead Count        */
    volatile uint32_t SLEEPCNT; /* 0x10  Sleep Count Register            */
    volatile uint32_t LSUCNT;   /* 0x14  LSU Count Register              */
    volatile uint32_t FOLDCNT;  /* 0x18  Folded-instruction Count        */
    volatile uint32_t PCSR;     /* 0x1C  Program Counter Sample Register */
} DWT_Type;

#define DWT_BASE            0xE0001000UL
#define DWT                 ((DWT_Type *)DWT_BASE)

#define DWT_CTRL_CYCCNTENA_Pos  0U
#define DWT_CTRL_CYCCNTENA_Msk  (1UL << DWT_CTRL_CYCCNTENA_Pos)

/* ── SysTick ──────────────────────────────────────────────────────────── */

typedef struct {
    volatile uint32_t CTRL;     /* 0x00  SysTick Control and Status      */
    volatile uint32_t LOAD;     /* 0x04  SysTick Reload Value            */
    volatile uint32_t VAL;      /* 0x08  SysTick Current Value           */
    volatile uint32_t CALIB;    /* 0x0C  SysTick Calibration             */
} SysTick_Type;

#define SysTick_BASE        0xE000E010UL
#define SysTick             ((SysTick_Type *)SysTick_BASE)

/* ── FLASH ────────────────────────────────────────────────────────────── */

typedef struct {
    volatile uint32_t ACR;      /* 0x00  Access control register         */
    volatile uint32_t KEYR;     /* 0x04  Key register                    */
    volatile uint32_t OPTKEYR;  /* 0x08  Option key register             */
    volatile uint32_t SR;       /* 0x0C  Status register                 */
    volatile uint32_t CR;       /* 0x10  Control register                */
    volatile uint32_t OPTCR;    /* 0x14  Option control register         */
    volatile uint32_t OPTCR1;   /* 0x18  Option control register 1       */
} FLASH_TypeDef;

#define FLASH_BASE          0x40023C00UL
#define FLASH               ((FLASH_TypeDef *)FLASH_BASE)

#define FLASH_ACR_LATENCY_Pos   0U
#define FLASH_ACR_LATENCY_Msk   (0xFUL << FLASH_ACR_LATENCY_Pos)
#define FLASH_ACR_LATENCY_7WS   (0x7UL << FLASH_ACR_LATENCY_Pos)
#define FLASH_ACR_PRFTEN_Pos    8U
#define FLASH_ACR_PRFTEN_Msk    (1UL << FLASH_ACR_PRFTEN_Pos)
#define FLASH_ACR_ARTEN_Pos     9U
#define FLASH_ACR_ARTEN_Msk     (1UL << FLASH_ACR_ARTEN_Pos)

/* ── Power Control (PWR) ──────────────────────────────────────────────── */

typedef struct {
    volatile uint32_t CR1;      /* 0x00 */
    volatile uint32_t CSR1;     /* 0x04 */
    volatile uint32_t CR2;      /* 0x08 */
    volatile uint32_t CSR2;     /* 0x0C */
} PWR_TypeDef;

#define PWR_BASE            0x40007000UL
#define PWR                 ((PWR_TypeDef *)PWR_BASE)

#define PWR_CR1_VOS_Pos     14U
#define PWR_CR1_VOS_Msk     (3UL << PWR_CR1_VOS_Pos)
#define PWR_CR1_ODEN_Pos    16U
#define PWR_CR1_ODEN_Msk    (1UL << PWR_CR1_ODEN_Pos)
#define PWR_CR1_ODSWEN_Pos  17U
#define PWR_CR1_ODSWEN_Msk  (1UL << PWR_CR1_ODSWEN_Pos)
#define PWR_CSR1_ODRDY_Pos  16U
#define PWR_CSR1_ODRDY_Msk  (1UL << PWR_CSR1_ODRDY_Pos)
#define PWR_CSR1_ODSWRDY_Pos 17U
#define PWR_CSR1_ODSWRDY_Msk (1UL << PWR_CSR1_ODSWRDY_Pos)

/* ── RCC ──────────────────────────────────────────────────────────────── */

typedef struct {
    volatile uint32_t CR;           /* 0x00 */
    volatile uint32_t PLLCFGR;      /* 0x04 */
    volatile uint32_t CFGR;         /* 0x08 */
    volatile uint32_t CIR;          /* 0x0C */
    volatile uint32_t AHB1RSTR;     /* 0x10 */
    volatile uint32_t AHB2RSTR;     /* 0x14 */
    volatile uint32_t AHB3RSTR;     /* 0x18 */
    uint32_t          RESERVED0;    /* 0x1C */
    volatile uint32_t APB1RSTR;     /* 0x20 */
    volatile uint32_t APB2RSTR;     /* 0x24 */
    uint32_t          RESERVED1[2]; /* 0x28-0x2C */
    volatile uint32_t AHB1ENR;      /* 0x30 */
    volatile uint32_t AHB2ENR;      /* 0x34 */
    volatile uint32_t AHB3ENR;      /* 0x38 */
    uint32_t          RESERVED2;    /* 0x3C */
    volatile uint32_t APB1ENR;      /* 0x40 */
    volatile uint32_t APB2ENR;      /* 0x44 */
    uint32_t          RESERVED3[2]; /* 0x48-0x4C */
    volatile uint32_t AHB1LPENR;    /* 0x50 */
    volatile uint32_t AHB2LPENR;    /* 0x54 */
    volatile uint32_t AHB3LPENR;    /* 0x58 */
    uint32_t          RESERVED4;    /* 0x5C */
    volatile uint32_t APB1LPENR;    /* 0x60 */
    volatile uint32_t APB2LPENR;    /* 0x64 */
    uint32_t          RESERVED5[2]; /* 0x68-0x6C */
    volatile uint32_t BDCR;         /* 0x70 */
    volatile uint32_t CSR;          /* 0x74 */
    uint32_t          RESERVED6[2]; /* 0x78-0x7C */
    volatile uint32_t SSCGR;       /* 0x80 */
    volatile uint32_t PLLI2SCFGR;  /* 0x84 */
    volatile uint32_t PLLSAICFGR;  /* 0x88 */
    volatile uint32_t DCKCFGR1;    /* 0x8C */
    volatile uint32_t DCKCFGR2;    /* 0x90 */
} RCC_TypeDef;

#define RCC_BASE            0x40023800UL
#define RCC                 ((RCC_TypeDef *)RCC_BASE)

/* RCC_CR bits */
#define RCC_CR_HSEON_Pos        16U
#define RCC_CR_HSEON_Msk        (1UL << RCC_CR_HSEON_Pos)
#define RCC_CR_HSERDY_Pos       17U
#define RCC_CR_HSERDY_Msk       (1UL << RCC_CR_HSERDY_Pos)
#define RCC_CR_PLLON_Pos        24U
#define RCC_CR_PLLON_Msk        (1UL << RCC_CR_PLLON_Pos)
#define RCC_CR_PLLRDY_Pos       25U
#define RCC_CR_PLLRDY_Msk       (1UL << RCC_CR_PLLRDY_Pos)

/* RCC_PLLCFGR bits */
#define RCC_PLLCFGR_PLLM_Pos   0U
#define RCC_PLLCFGR_PLLM_Msk   (0x3FUL << RCC_PLLCFGR_PLLM_Pos)
#define RCC_PLLCFGR_PLLN_Pos   6U
#define RCC_PLLCFGR_PLLN_Msk   (0x1FFUL << RCC_PLLCFGR_PLLN_Pos)
#define RCC_PLLCFGR_PLLP_Pos   16U
#define RCC_PLLCFGR_PLLP_Msk   (3UL << RCC_PLLCFGR_PLLP_Pos)
#define RCC_PLLCFGR_PLLSRC_Pos 22U
#define RCC_PLLCFGR_PLLSRC_Msk (1UL << RCC_PLLCFGR_PLLSRC_Pos)
#define RCC_PLLCFGR_PLLSRC_HSE (1UL << RCC_PLLCFGR_PLLSRC_Pos)
#define RCC_PLLCFGR_PLLQ_Pos   24U
#define RCC_PLLCFGR_PLLQ_Msk   (0xFUL << RCC_PLLCFGR_PLLQ_Pos)

/* RCC_CFGR bits */
#define RCC_CFGR_SW_Pos         0U
#define RCC_CFGR_SW_Msk         (3UL << RCC_CFGR_SW_Pos)
#define RCC_CFGR_SW_PLL         (2UL << RCC_CFGR_SW_Pos)
#define RCC_CFGR_SWS_Pos        2U
#define RCC_CFGR_SWS_Msk        (3UL << RCC_CFGR_SWS_Pos)
#define RCC_CFGR_SWS_PLL        (2UL << RCC_CFGR_SWS_Pos)
#define RCC_CFGR_HPRE_Pos       4U
#define RCC_CFGR_HPRE_Msk       (0xFUL << RCC_CFGR_HPRE_Pos)
#define RCC_CFGR_HPRE_DIV1      (0x0UL << RCC_CFGR_HPRE_Pos)
#define RCC_CFGR_PPRE1_Pos      10U
#define RCC_CFGR_PPRE1_Msk      (7UL << RCC_CFGR_PPRE1_Pos)
#define RCC_CFGR_PPRE1_DIV4     (5UL << RCC_CFGR_PPRE1_Pos)
#define RCC_CFGR_PPRE2_Pos      13U
#define RCC_CFGR_PPRE2_Msk      (7UL << RCC_CFGR_PPRE2_Pos)
#define RCC_CFGR_PPRE2_DIV2     (4UL << RCC_CFGR_PPRE2_Pos)

/* RCC_AHB1ENR bits */
#define RCC_AHB1ENR_GPIOBEN_Pos 1U
#define RCC_AHB1ENR_GPIOBEN_Msk (1UL << RCC_AHB1ENR_GPIOBEN_Pos)
#define RCC_AHB1ENR_GPIODEN_Pos 3U
#define RCC_AHB1ENR_GPIODEN_Msk (1UL << RCC_AHB1ENR_GPIODEN_Pos)

/* RCC_APB1ENR bits */
#define RCC_APB1ENR_USART3EN_Pos 18U
#define RCC_APB1ENR_USART3EN_Msk (1UL << RCC_APB1ENR_USART3EN_Pos)
#define RCC_APB1ENR_PWREN_Pos    28U
#define RCC_APB1ENR_PWREN_Msk    (1UL << RCC_APB1ENR_PWREN_Pos)

/* ── GPIO ─────────────────────────────────────────────────────────────── */

typedef struct {
    volatile uint32_t MODER;    /* 0x00  Mode register                   */
    volatile uint32_t OTYPER;   /* 0x04  Output type register            */
    volatile uint32_t OSPEEDR;  /* 0x08  Output speed register           */
    volatile uint32_t PUPDR;    /* 0x0C  Pull-up/pull-down register      */
    volatile uint32_t IDR;      /* 0x10  Input data register             */
    volatile uint32_t ODR;      /* 0x14  Output data register            */
    volatile uint32_t BSRR;     /* 0x18  Bit set/reset register          */
    volatile uint32_t LCKR;     /* 0x1C  Lock register                   */
    volatile uint32_t AFR[2];   /* 0x20  Alternate function low/high     */
} GPIO_TypeDef;

#define GPIOB_BASE          0x40020400UL
#define GPIOD_BASE          0x40020C00UL
#define GPIOB               ((GPIO_TypeDef *)GPIOB_BASE)
#define GPIOD               ((GPIO_TypeDef *)GPIOD_BASE)

/* ── USART ────────────────────────────────────────────────────────────── */

typedef struct {
    volatile uint32_t CR1;      /* 0x00  Control register 1              */
    volatile uint32_t CR2;      /* 0x04  Control register 2              */
    volatile uint32_t CR3;      /* 0x08  Control register 3              */
    volatile uint32_t BRR;      /* 0x0C  Baud rate register              */
    volatile uint32_t GTPR;     /* 0x10  Guard time and prescaler        */
    volatile uint32_t RTOR;     /* 0x14  Receiver Time Out register      */
    volatile uint32_t RQR;      /* 0x18  Request register                */
    volatile uint32_t ISR;      /* 0x1C  Interrupt and status register   */
    volatile uint32_t ICR;      /* 0x20  Interrupt flag clear register   */
    volatile uint32_t RDR;      /* 0x24  Receive data register           */
    volatile uint32_t TDR;      /* 0x28  Transmit data register          */
} USART_TypeDef;

#define USART3_BASE         0x40004800UL
#define USART3              ((USART_TypeDef *)USART3_BASE)

#define USART_CR1_UE_Pos    0U
#define USART_CR1_UE_Msk    (1UL << USART_CR1_UE_Pos)
#define USART_CR1_TE_Pos    3U
#define USART_CR1_TE_Msk    (1UL << USART_CR1_TE_Pos)
#define USART_CR1_RE_Pos    2U
#define USART_CR1_RE_Msk    (1UL << USART_CR1_RE_Pos)

#define USART_ISR_TXE_Pos   7U
#define USART_ISR_TXE_Msk   (1UL << USART_ISR_TXE_Pos)
#define USART_ISR_TC_Pos    6U
#define USART_ISR_TC_Msk    (1UL << USART_ISR_TC_Pos)

/* ── Inline helpers ───────────────────────────────────────────────────── */

static inline void __NOP(void)  { __asm volatile ("nop"); }
static inline void __DSB(void)  { __asm volatile ("dsb 0xF" ::: "memory"); }
static inline void __ISB(void)  { __asm volatile ("isb 0xF" ::: "memory"); }
static inline void __WFI(void)  { __asm volatile ("wfi"); }

/* NVIC_SystemReset — request system reset via SCB AIRCR */
#define SCB_AIRCR           (*(volatile uint32_t *)0xE000ED0CUL)
#define SCB_AIRCR_VECTKEY   (0x05FAUL << 16U)
#define SCB_AIRCR_SYSRESETREQ (1UL << 2U)

static inline void NVIC_SystemReset(void) {
    __DSB();
    SCB_AIRCR = SCB_AIRCR_VECTKEY | SCB_AIRCR_SYSRESETREQ;
    __DSB();
    for (;;) __NOP();
}

#endif /* STM32F767XX_MINIMAL_H */
