/*
 * startup_stm32f767xx.s — Minimal startup for STM32F767ZI (Cortex-M7)
 *
 * Stack at top of SRAM (0x20080000 = 0x20000000 + 512K).
 * Vector table at 0x08000000 (FLASH base).
 *
 * Reset handler: copy .data, zero .bss, call SystemInit, call main.
 */

    .syntax unified
    .cpu    cortex-m7
    .fpu    fpv5-d16
    .thumb

/* ── Stack pointer initial value ──────────────────────────────────────── */
    .equ    _estack, 0x20080000

/* ── Vector table ─────────────────────────────────────────────────────── */
    .section .isr_vector, "a", %progbits
    .type   g_pfnVectors, %object

g_pfnVectors:
    .word   _estack             /* 0x00: Initial stack pointer           */
    .word   Reset_Handler       /* 0x04: Reset                           */
    .word   NMI_Handler         /* 0x08: NMI                             */
    .word   HardFault_Handler   /* 0x0C: Hard fault                      */
    .word   MemManage_Handler   /* 0x10: Memory management fault         */
    .word   BusFault_Handler    /* 0x14: Bus fault                       */
    .word   UsageFault_Handler  /* 0x18: Usage fault                     */
    .word   0                   /* 0x1C: Reserved                        */
    .word   0                   /* 0x20: Reserved                        */
    .word   0                   /* 0x24: Reserved                        */
    .word   0                   /* 0x28: Reserved                        */
    .word   SVC_Handler         /* 0x2C: SVCall                          */
    .word   DebugMon_Handler    /* 0x30: Debug monitor                   */
    .word   0                   /* 0x34: Reserved                        */
    .word   PendSV_Handler      /* 0x38: PendSV                          */
    .word   SysTick_Handler     /* 0x3C: SysTick                         */
    .size   g_pfnVectors, . - g_pfnVectors

/* ── Reset handler ────────────────────────────────────────────────────── */
    .section .text.Reset_Handler
    .type   Reset_Handler, %function
    .global Reset_Handler

Reset_Handler:
    /* Copy .data from FLASH to SRAM */
    ldr     r0, =_sdata
    ldr     r1, =_edata
    ldr     r2, =_sidata
.Lcopy_data:
    cmp     r0, r1
    bge     .Lzero_bss
    ldr     r3, [r2], #4
    str     r3, [r0], #4
    b       .Lcopy_data

    /* Zero .bss */
.Lzero_bss:
    ldr     r0, =_sbss
    ldr     r1, =_ebss
    movs    r2, #0
.Lzero_loop:
    cmp     r0, r1
    bge     .Lcall_init
    str     r2, [r0], #4
    b       .Lzero_loop

.Lcall_init:
    bl      SystemInit
    bl      main
    b       .

    .size   Reset_Handler, . - Reset_Handler

/* ── Default fault handlers (infinite loop) ───────────────────────────── */
    .section .text.Default_Handler, "ax", %progbits
    .type   Default_Handler, %function
    .weak   Default_Handler
Default_Handler:
    b       .
    .size   Default_Handler, . - Default_Handler

    .weak   NMI_Handler
    .thumb_set NMI_Handler, Default_Handler
    .weak   HardFault_Handler
    .thumb_set HardFault_Handler, Default_Handler
    .weak   MemManage_Handler
    .thumb_set MemManage_Handler, Default_Handler
    .weak   BusFault_Handler
    .thumb_set BusFault_Handler, Default_Handler
    .weak   UsageFault_Handler
    .thumb_set UsageFault_Handler, Default_Handler
    .weak   SVC_Handler
    .thumb_set SVC_Handler, Default_Handler
    .weak   DebugMon_Handler
    .thumb_set DebugMon_Handler, Default_Handler
    .weak   PendSV_Handler
    .thumb_set PendSV_Handler, Default_Handler
    .weak   SysTick_Handler
    .thumb_set SysTick_Handler, Default_Handler

    .end
