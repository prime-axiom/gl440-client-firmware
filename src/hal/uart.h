#ifndef UART_H
#define UART_H

void uart_init(void);
void uart_putc(unsigned char c);
void uart_puts(const char *s);

#endif
