/*
 * dwt.h — DWT cycle counter for Cortex-M7
 */
#ifndef DWT_H
#define DWT_H

#include "stm32f767xx_minimal.h"

static inline void dwt_init(void) {
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

static inline uint32_t dwt_cycles(void) {
    return DWT->CYCCNT;
}

#endif /* DWT_H */
