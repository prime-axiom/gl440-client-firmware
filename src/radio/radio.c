/*
 * CC2510 raw GFSK radio driver
 *
 * Config target:
 *   Modulation : GFSK 250 kbit/s, ±127 kHz deviation
 *   Frequency  : 2.440 GHz (channel 0), 1 MHz spacing
 *   Sync word  : 0xD391D391
 *   Whitening  : enabled (PN9)
 *   Address    : hardware address filter on byte 0 of payload
 *   CRC        : hardware CRC-16/CCITT appended by TX, checked by RX
 *
 * Stub — real register init in Phase 3.
 * Register values to derive from SWRS055G + DN107 (DMA+Radio config).
 */

#include "radio.h"

void radio_init(void) {}

void radio_tx(unsigned char dst, unsigned char type,
              const unsigned char *payload, unsigned char len)
{
    (void)dst; (void)type; (void)payload; (void)len;
}

unsigned char radio_rx(unsigned char *buf, unsigned char *len)
{
    (void)buf; (void)len;
    return 0;
}
