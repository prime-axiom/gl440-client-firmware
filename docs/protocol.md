# GL440 Custom Radio Protocol

## Physical Layer

| Parameter | Value |
|---|---|
| Modulation | GFSK |
| Bitrate | 250 kbit/s |
| Deviation | ±127 kHz |
| Frequency | 2.440 GHz (ch 0), 1 MHz spacing |
| Sync word | `0xD391D391` (32-bit) |
| Whitening | PN9, enabled |
| CRC | CRC-16/CCITT, hardware-appended |

## Frame Format

```
┌──────────┬──────────┬───────┬─────┬─────┬──────┬─────────────┬──────────┐
│ Sync     │ Length   │ NetID │ Dst │ Src │ Type │ Payload     │ CRC16    │
│ 4 bytes  │ 1 byte   │ 1 B   │ 1 B │ 1 B │ 1 B  │ 0..58 bytes │ 2 bytes  │
└──────────┴──────────┴───────┴─────┴─────┴──────┴─────────────┴──────────┘
```

- **Sync**: not included in Length count; handled by CC2510 hardware sync-word detector.
- **Length**: byte count from NetID through CRC16 (inclusive).
- **NetID**: network identifier (allows multiple independent GL440 networks on the same channel).
- **Dst / Src**: 1-byte tag address. `0xFF` = broadcast.
- **CRC16**: over NetID..Payload, appended by hardware.

## Frame Types

| Type | Hex | Direction | Description |
|---|---|---|---|
| `BEACON` | `0x01` | AP → broadcast | Periodic announcement: channel, NetID, current AP time |
| `CHECKIN_REQ` | `0x02` | Tag → AP | Tag woke up: sends tag address, battery mV, current image hash |
| `CHECKIN_RESP` | `0x03` | AP → tag | "No update" or "update available: hash, total size, chunk count" |
| `IMG_CHUNK` | `0x04` | AP → tag | 56-byte data chunk with chunk index |
| `IMG_ACK` | `0x05` | Tag → AP | Bitmap of received chunks (up to 16 at a time) |
| `IMG_DONE` | `0x06` | AP → tag | Final hash — tag verifies, refreshes display, goes to sleep |

## Tag Sleep Cycle

```
[Wake] → listen for BEACON (~5 ms window)
       → if no BEACON: sleep(60 s)
       → send CHECKIN_REQ
       → receive CHECKIN_RESP
         → "no update": sleep(60 s)
         → "update": receive IMG_CHUNKs, send IMG_ACKs, wait for IMG_DONE
           → verify hash, refresh display (~5 s)
           → sleep(60 s)
```
