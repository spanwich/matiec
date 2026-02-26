/*
 * uart.c — USART3 on PD8(TX)/PD9(RX) — ST-LINK virtual COM port
 *
 * APB1 clock = 54 MHz after SystemInit().
 */
#include "stm32f767xx_minimal.h"
#include "uart.h"

#define APB1_FREQ  54000000UL

void uart_init(uint32_t baud) {
    /* Enable GPIOD and USART3 clocks */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN_Msk;
    RCC->APB1ENR |= RCC_APB1ENR_USART3EN_Msk;
    (void)RCC->APB1ENR;

    /* PD8 = AF7 (USART3_TX), PD9 = AF7 (USART3_RX) */
    /* MODER: alternate function (10) for PD8 and PD9 */
    GPIOD->MODER &= ~((3U << (8 * 2)) | (3U << (9 * 2)));
    GPIOD->MODER |=  ((2U << (8 * 2)) | (2U << (9 * 2)));
    /* AFR[1] (AFRH): pins 8-15, AF7 for pin 8 and pin 9 */
    GPIOD->AFR[1] &= ~((0xFU << ((8 - 8) * 4)) | (0xFU << ((9 - 8) * 4)));
    GPIOD->AFR[1] |=  ((7U   << ((8 - 8) * 4)) | (7U   << ((9 - 8) * 4)));

    /* Configure USART3 */
    USART3->CR1 = 0;                         /* Disable USART first       */
    USART3->BRR = APB1_FREQ / baud;          /* 54MHz / 115200 = 468      */
    USART3->CR1 = USART_CR1_TE_Msk           /* Enable transmitter        */
                | USART_CR1_RE_Msk           /* Enable receiver           */
                | USART_CR1_UE_Msk;          /* Enable USART              */
}

void uart_putc(char c) {
    while (!(USART3->ISR & USART_ISR_TXE_Msk))
        ;
    USART3->TDR = (uint8_t)c;
}

void uart_puts(const char *s) {
    while (*s)
        uart_putc(*s++);
}

void uart_putu32(uint32_t v) {
    char buf[11]; /* max 10 digits + NUL */
    int i = 10;
    buf[i] = '\0';
    if (v == 0) {
        uart_putc('0');
        return;
    }
    while (v > 0) {
        buf[--i] = '0' + (char)(v % 10);
        v /= 10;
    }
    uart_puts(&buf[i]);
}

void uart_write(const uint8_t *buf, uint32_t len) {
    for (uint32_t i = 0; i < len; i++)
        uart_putc((char)buf[i]);
}
