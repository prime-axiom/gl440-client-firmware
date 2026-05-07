# gl440-client-firmware

Custom CC2510 firmware for SES Imagotag VUSION 2.6 BWR GL440 e-ink price tags.

Replaces the proprietary Imagotag firmware with a lean open protocol stack that works with the companion [gl440-ap-firmware](https://github.com/prime-axiom/gl440-ap-firmware) access point.

## Overview

The GL440 tag uses a Texas Instruments **CC2510** (8051 core + 2.4 GHz radio, 32 KB flash). This firmware implements:

- Raw GFSK 2.4 GHz radio receive loop (no 802.15.4, no SimpliciTI)
- IL0373-compatible EPD display driver (296x152 px, BWR)
- Custom lightweight image transfer protocol
- Deep sleep between check-ins (~60 s interval, CR2450 battery life target: 6-12 months)

## Hardware

| Component | Part |
|---|---|
| MCU + Radio | TI CC2510 (8051, 32 KB flash, 4 KB XRAM) |
| Display | Pervasive Displays EL026TR3 (2.66", BWR, IL0373 driver) |
| NFC | NXP NT3H2111 (optional, not used in v1) |
| SPI Flash | Winbond W25X10CL (1 Mbit, optional cache) |

## Toolchain

- **Compiler**: [SDCC](https://sdcc.sourceforge.net/) >= 4.4 (`sdcc -mmcs51`)
- **Flasher**: [atc1441/ESP_CC_Flasher](https://github.com/atc1441/ESP_CC_Flasher) (ESP32 + 3 wires to CC-Debug pads)

### macOS Setup

```bash
brew install sdcc
sdcc --version  # SDCC : mcs51 4.x.x
```

## Build

```bash
make            # produces firmware.hex
make clean
```

## Flashing

Flash via [ESP_CC_Flasher](https://github.com/atc1441/ESP_CC_Flasher).

Debug pad locations on the GL440 PCB:

| Signal | CC2510 Pin | GL440 Test Pad |
|---|---|---|
| DD (data) | P2_1 | TP_DD |
| DC (clock) | P2_2 | TP_DC |
| RESET_N | RST | TP_RST |
| GND | GND | TP_GND |
| VCC (3.3V) | VDD | TP_VCC |

> **Warning**: Flashing erases the original Imagotag firmware permanently. Back it up first if you want to preserve it (not always possible if read-out protection is active — chip erase will bypass it).

## Project Structure

```
src/
  main.c          # entry point, main loop
  display/
    epd.c         # IL0373 EPD driver
    epd.h
  radio/
    radio.c       # CC2510 raw GFSK init + TX/RX
    radio.h
  protocol/
    protocol.c    # frame parser / state machine
    protocol.h
  hal/
    clock.c       # WDT-based tick
    uart.c        # UART0 debug output
    sleep.c       # PM2 sleep + wakeup timer
Makefile
```

## Protocol

Frames exchanged with the AP use a simple custom format over GFSK 250 kbit/s:

```
[Sync 4B][Length 1B][NetID 1B][Dst 1B][Src 1B][Type 1B][Payload N B][CRC16 2B]
```

Frame types: `BEACON`, `CHECKIN_REQ`, `CHECKIN_RESP`, `IMG_CHUNK`, `IMG_ACK`, `IMG_DONE`.

See [`docs/protocol.md`](docs/protocol.md) for full specification.

## References

- [CC2510 Datasheet (SWRS055G)](https://www.ti.com/lit/ds/symlink/cc2510.pdf)
- [IL0373 Datasheet](https://www.mikroshop.ch/pdf/IL0373.pdf)
- [angrymew/firmware-cc2510](https://github.com/angrymew/firmware-cc2510) — display driver reference
- [andrei-tatar/imagotag-hack](https://github.com/andrei-tatar/imagotag-hack) — SDCC build skeleton reference
- [atc1441/ESP_CC_Flasher](https://github.com/atc1441/ESP_CC_Flasher) — CC2510 flasher tool

## License

MIT
