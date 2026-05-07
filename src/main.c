/*
 * gl440-client-firmware — main entry point
 *
 * Phase 0 skeleton: compiles clean, does nothing yet.
 * Replace stubs with real implementations phase by phase.
 */

#include <cc2510fx.h>
#include "hal/clock.h"
#include "hal/uart.h"
#include "hal/sleep.h"
#include "display/epd.h"
#include "radio/radio.h"
#include "protocol/protocol.h"

void main(void)
{
    clock_init();
    uart_init();
    radio_init();
    epd_init();

    uart_puts("gl440-client booted\r\n");

    while (1) {
        protocol_tick();
        sleep_pm2(60); /* wake every ~60 s */
    }
}
