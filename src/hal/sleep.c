/* PM2 deep sleep via 32 kHz sleep timer — stub */
#include <cc2510fx.h>
#include "sleep.h"

void sleep_pm2(unsigned char seconds)
{
    /* Sleep timer tick = 1/32768 s. For N seconds: ST += N * 32768 */
    unsigned long ticks = (unsigned long)seconds * 32768UL;
    unsigned long now;

    /* Read current ST value (ST2:ST1:ST0, must read ST0 last) */
    now  = (unsigned long)ST2 << 16;
    now |= (unsigned long)ST1 << 8;
    now |= ST0;

    now += ticks;

    ST2 = (unsigned char)(now >> 16);
    ST1 = (unsigned char)(now >> 8);
    ST0 = (unsigned char)(now);       /* writing ST0 arms the compare */

    STIE = 1; /* enable sleep timer interrupt */
    STIF = 0;

    /* Enter PM2 */
    SLEEP = (SLEEP & ~0x03) | 0x02;
    __asm nop __endasm;
    __asm nop __endasm;
    __asm nop __endasm;
    PCON |= 0x01; /* IDLE bit — MCU halts until interrupt */
}
