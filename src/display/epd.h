#ifndef EPD_H
#define EPD_H

void epd_init(void);
void epd_clear(void);
/* buf_bw: 296*152/8 bytes black/white plane, buf_red: same for red plane */
void epd_display(const unsigned char *buf_bw, const unsigned char *buf_red);
void epd_sleep(void);

#endif
