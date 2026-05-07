/*
 * IL0373 EPD driver for GL440 (296x152 px, BWR)
 *
 * Pin mapping (from angrymew/firmware-cc2510):
 *   EPD_PWR  = P0_0
 *   CS       = P0_1
 *   MOSI     = P0_3  (USART0 alt. location 1)
 *   SCK      = P0_5  (USART0 alt. location 1)
 *   DC       = P1_2
 *   BUSY     = P1_3
 *   RESET    = P2_0
 *
 * Stub — real implementation in Phase 1.
 */

#include "epd.h"

void epd_init(void)  {}
void epd_clear(void) {}

void epd_display(const unsigned char *buf_bw, const unsigned char *buf_red)
{
    (void)buf_bw;
    (void)buf_red;
}

void epd_sleep(void) {}
