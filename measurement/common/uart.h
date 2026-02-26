/*
 * uart.h — USART3 on PD8(TX)/PD9(RX) — ST-LINK virtual COM port
 */
#ifndef UART_H
#define UART_H

#include <stdint.h>

void uart_init(uint32_t baud);
void uart_putc(char c);
void uart_puts(const char *s);
void uart_putu32(uint32_t v);
void uart_write(const uint8_t *buf, uint32_t len);

#endif /* UART_H */
