#ifndef PROTOCOL_H
#define PROTOCOL_H

/* Frame types */
#define FRAME_BEACON       0x01
#define FRAME_CHECKIN_REQ  0x02
#define FRAME_CHECKIN_RESP 0x03
#define FRAME_IMG_CHUNK    0x04
#define FRAME_IMG_ACK      0x05
#define FRAME_IMG_DONE     0x06

void protocol_tick(void);

#endif
