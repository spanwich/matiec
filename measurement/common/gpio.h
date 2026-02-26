/*
 * gpio.h — PB0/PB1 instrumentation pins for oscilloscope measurement
 *
 * PB0 = total scan cycle (high during entire body call)
 * PB1 = execution only  (high during POU body, excludes setup/teardown)
 */
#ifndef GPIO_H
#define GPIO_H

#include "stm32f767xx_minimal.h"

static inline void gpio_instr_init(void) {
    /* Enable GPIOB clock */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN_Msk;
    /* Brief delay for clock to stabilise */
    (void)RCC->AHB1ENR;

    /* PB0 and PB1: General purpose output, push-pull (MODER = 01) */
    GPIOB->MODER &= ~((3U << (0 * 2)) | (3U << (1 * 2)));
    GPIOB->MODER |=  ((1U << (0 * 2)) | (1U << (1 * 2)));
    /* Start low */
    GPIOB->BSRR = (1U << (0 + 16)) | (1U << (1 + 16));
}

/* Total scan instrumentation (PB0) */
#define SCAN_START()  do { GPIOB->BSRR = (1U << 0); } while (0)
#define SCAN_END()    do { GPIOB->BSRR = (1U << (0 + 16)); } while (0)

/* Execution-only instrumentation (PB1) */
#define EXEC_START()  do { GPIOB->BSRR = (1U << 1); } while (0)
#define EXEC_END()    do { GPIOB->BSRR = (1U << (1 + 16)); } while (0)

#endif /* GPIO_H */
