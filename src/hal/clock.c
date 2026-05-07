/* WDT-based clock tick — stub */
#include <cc2510fx.h>
#include "clock.h"

void clock_init(void)
{
    /* 26 MHz XOSC, divide by 1 */
    CLKCON = 0x00;
    while (!(SLEEP & 0x40)); /* wait for XOSC stable */
}

void delay_ms(unsigned int ms)
{
    /* busy-wait placeholder; replace with timer-based implementation */
    unsigned int i, j;
    for (i = 0; i < ms; i++)
        for (j = 0; j < 1200; j++);
}
