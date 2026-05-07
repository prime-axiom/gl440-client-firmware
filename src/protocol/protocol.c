/* Protocol state machine — stub for Phase 0 */

#include "protocol.h"
#include "../radio/radio.h"

void protocol_tick(void)
{
    unsigned char buf[64];
    unsigned char len;

    if (radio_rx(buf, &len)) {
        /* TODO: parse frame type and dispatch */
        (void)buf;
        (void)len;
    }
}
