/* UART0 debug output (115200 8N1, P0_2=TX) — stub */
#include <cc2510fx.h>
#include "uart.h"

void uart_init(void)
{
    PERCFG &= ~0x01;       /* USART0 at alt. location 0: P0_2=TX, P0_3=RX */
    P0SEL  |=  0x0C;       /* P0_2, P0_3 as peripheral */
    U0CSR   =  0x80;       /* UART mode */
    U0GCR   =  11;         /* baud exponent for 115200 @ 26 MHz */
    U0BAUD  =  216;        /* baud mantissa */
    UTX0IF  =  1;
}

void uart_putc(unsigned char c)
{
    U0DBUF = c;
    while (!UTX0IF);
    UTX0IF = 0;
}

void uart_puts(const char *s)
{
    while (*s) uart_putc((unsigned char)*s++);
}
