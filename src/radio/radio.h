#ifndef RADIO_H
#define RADIO_H

#define RADIO_MAX_PAYLOAD 58

void          radio_init(void);
void          radio_tx(unsigned char dst, unsigned char type,
                       const unsigned char *payload, unsigned char len);
unsigned char radio_rx(unsigned char *buf, unsigned char *len); /* returns 1 if frame ready */

#endif
