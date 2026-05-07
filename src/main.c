/*
 * gl440-client-firmware
 *
 * Phase 1 goal: display a BWR test pattern to verify the EPD driver.
 *
 * Test pattern:
 *   - Left third  : all black  (buf_bw = 0x00, buf_red = 0xff)
 *   - Middle third: all red    (buf_bw = 0xff, buf_red = 0x00)
 *   - Right third : all white  (buf_bw = 0xff, buf_red = 0xff)
 *
 * If all three bands appear correctly the EPD driver, SPI wiring, and
 * pin mapping are confirmed working.  After the refresh the tag sleeps.
 */

#include <cc2510fx.h>
#include <stdint.h>
#include "hal/clock.h"
#include "hal/uart.h"
#include "hal/sleep.h"
#include "display/epd.h"
#include "radio/radio.h"
#include "protocol/protocol.h"

/*
 * EPD_BUFFER_SIZE = 152/8 * 296 = 5624 bytes per plane.
 * We generate the pattern on the fly to avoid consuming all of XRAM.
 * Column byte index within a row: 0..18 (19 bytes = 152 pixels).
 * Columns 0..5 → black, 6..12 → red, 13..18 → white.
 */
#define COLS_PER_ROW   19   /* ceil(152/8) */
#define BLACK_COLS      6
#define RED_COLS        7   /* BLACK_COLS + RED_COLS = 13 */

static void display_test_pattern(void)
{
    uint16_t row, col;
    uint8_t bw_byte, red_byte;

    uint8_t bw_buf[COLS_PER_ROW];
    uint8_t red_buf[COLS_PER_ROW];

    /* Build one row worth of pattern bytes */
    for (col = 0; col < COLS_PER_ROW; col++) {
        if (col < BLACK_COLS) {
            bw_byte  = 0x00;    /* black: BW=0 */
            red_byte = 0xff;    /* not red */
        } else if (col < BLACK_COLS + RED_COLS) {
            bw_byte  = 0xff;    /* white base for red area */
            red_byte = 0x00;    /* red: RED=0 */
        } else {
            bw_byte  = 0xff;    /* white: BW=1 */
            red_byte = 0xff;    /* not red */
        }
        bw_buf[col]  = bw_byte;
        red_buf[col] = red_byte;
    }

    /* Send BW plane row by row */
    /* epd_display() expects a flat buffer; we stream it manually here
     * to avoid a 5624-byte XRAM allocation. */
    {
        extern void epd_stream_start_bw(void);
        extern void epd_stream_start_red(void);
        extern void epd_stream_byte(uint8_t b);
        extern void epd_stream_refresh(void);

        epd_stream_start_bw();
        for (row = 0; row < 296; row++)
            for (col = 0; col < COLS_PER_ROW; col++)
                epd_stream_byte(bw_buf[col]);

        epd_stream_start_red();
        for (row = 0; row < 296; row++)
            for (col = 0; col < COLS_PER_ROW; col++)
                epd_stream_byte(red_buf[col]);

        epd_stream_refresh();
    }
}

void main(void)
{
    clock_init();
    uart_init();

    uart_puts("gl440 phase-1 boot\r\n");

    epd_init();
    uart_puts("epd init done\r\n");

    display_test_pattern();
    uart_puts("test pattern sent\r\n");

    epd_sleep();
    uart_puts("epd sleeping\r\n");

    /* Deep sleep — wake via reset or power-cycle for now */
    while (1) {
        sleep_pm2(60);
    }
}
