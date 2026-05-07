/*
 * PM2 deep sleep via CC2510 sleep timer (WOR = Wake-On-Radio timer)
 *
 * The CC2510 sleep timer is a 24-bit counter running at 32 kHz.
 * Exposed via WORTIME0 (low) and WORTIME1 (high 8 bits only — 16-bit view).
 * Wake-up is configured via WOREVT0/WOREVT1 (event 0 timeout, 16-bit).
 * WORCTL[2:0] selects the WOR resolution: 000 = 1 period (~31 µs each).
 *
 * For N seconds at 32768 Hz: timeout_ticks = N * 32768 / WOR_resolution
 * With WORCTL=0x00 (1-period resolution): set WOREVT = N * 32768
 * Max WOREVT = 0xFFFF = ~2 s per event; repeat via interrupt for longer sleeps.
 *
 * Simpler approach used here: set WOREVT for the desired interval directly
 * (capped at ~2 s per WOR event; chain multiple events for longer sleeps).
 *
 * For Phase 0 we keep it simple: busy-wait placeholder so the build passes.
 * Real PM2 implementation comes in Phase 3.
 */

#include <cc2510fx.h>
#include "sleep.h"

void sleep_pm2(unsigned char seconds)
{
    /* Phase 0 placeholder: busy-wait loop instead of real PM2 sleep.
     * Replace with WOR-based sleep in Phase 3. */
    unsigned char i;
    unsigned int  j;
    for (i = 0; i < seconds; i++)
        for (j = 0; j < 60000U; j++);
}
