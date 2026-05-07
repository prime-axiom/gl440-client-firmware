#ifndef EPD_H
#define EPD_H

#include <stdint.h>

#define EPD_HRES        152
#define EPD_VRES        296
#define EPD_BUFFER_SIZE (EPD_HRES / 8 * EPD_VRES)  /* 5624 bytes per plane */

void epd_init(void);
void epd_clear(void);

/*
 * Display a BWR image.
 * buf_bw : EPD_BUFFER_SIZE bytes, black/white plane (0 = black, 1 = white)
 * buf_red: EPD_BUFFER_SIZE bytes, red plane      (0 = red,   1 = no red)
 * Both buffers must reside in XRAM or __code space.
 */
void epd_display(const uint8_t *buf_bw, const uint8_t *buf_red);
void epd_sleep(void);

/* Streaming API — send image data without a full-frame buffer in XRAM.
 * Usage: epd_stream_start_bw() → N x epd_stream_byte() →
 *        epd_stream_start_red() → N x epd_stream_byte() →
 *        epd_stream_refresh()
 */
void epd_stream_start_bw(void);
void epd_stream_start_red(void);
void epd_stream_byte(uint8_t b);
void epd_stream_refresh(void);

#endif
