# gl440-client-firmware

Custom CC2510 firmware for SES Imagotag VUSION 2.6 BWR GL440 e-ink price tags.

Replaces the proprietary Imagotag firmware with a lean open protocol stack that works with the companion [gl440-ap-firmware](https://github.com/prime-axiom/gl440-ap-firmware) access point.

## Overview

The GL440 tag uses a Texas Instruments **CC2510** (8051 core + 2.4 GHz radio, 32 KB flash). This firmware implements:

- Raw GFSK 2.4 GHz radio receive loop (no 802.15.4, no SimpliciTI)
- IL0373-compatible EPD display driver (296 x 152 px, BWR)
- Custom lightweight image transfer protocol
- Deep sleep between check-ins (~60 s interval, CR2450 battery life target: 6–12 months)

## Hardware

| Component | Part |
|---|---|
| MCU + Radio | TI CC2510 (8051, 32 KB flash, 4 KB XRAM) |
| Display | Pervasive Displays EL026TR3 (2.66", BWR, IL0373 driver) |
| NFC | NXP NT3H2111 (optional, not used in v1) |
| SPI Flash | Winbond W25X10CL (1 Mbit, optional cache) |

---

## Toolchain Setup

### macOS

```bash
brew install sdcc
sdcc --version   # SDCC : mcs51 4.x.x
```

### Linux (Debian/Ubuntu)

```bash
sudo apt-get update && sudo apt-get install -y sdcc
sdcc --version
```

---

## Build

```bash
make          # compiles all sources → firmware.hex
make clean    # remove build artefacts
```

Pre-built binaries are available as [GitHub Actions artifacts](../../actions) for every commit on `main`.

---

## Flashing

The CC2510 is programmed via its **CC-Debug** interface (a 2-wire serial protocol). The easiest way to do this is with an ESP32 running [atc1441/ESP_CC_Flasher](https://github.com/atc1441/ESP_CC_Flasher).

### What you need

| Item | Notes |
|---|---|
| Any ESP32 devboard | ESP32, ESP32-S3, etc. — not ESP8266 |
| 5 jumper wires | Female–female or female–male depending on your devboard |
| GL440 tag (opened) | See disassembly section below |
| USB cable | Data-capable, for both devboard and serial monitor |

### Step 1 — Flash ESP_CC_Flasher onto the ESP32

1. Clone the flasher repo:
   ```bash
   git clone https://github.com/atc1441/ESP_CC_Flasher
   cd ESP_CC_Flasher
   ```
2. Open in [PlatformIO](https://platformio.org/) (VS Code extension or CLI) and flash to your ESP32:
   ```bash
   pio run --target upload
   ```
3. The ESP32 will create a Wi-Fi access point named **`CC_Flasher`** (password: `12345678`).
4. Connect to it and open **http://cc.local** in your browser — you should see the flasher web UI.

### Step 2 — Open the GL440

The GL440 housing is held together by two plastic clips on the short sides. Use a flat spudger or guitar pick to release them — no screws.

Inside you will find the PCB with the CC2510 and a row of test pads along one edge.

### Step 3 — Locate the debug pads

The debug pads are on the **back of the PCB**, typically labelled or documented in the community teardown:

| Signal | CC2510 pin | Function |
|---|---|---|
| `DD` | P2_1 | Debug Data (bidirectional) |
| `DC` | P2_2 | Debug Clock |
| `RST` | RESET_N | Reset (active low) |
| `GND` | GND | Ground |
| `VCC` | VDD | 3.3 V supply |

> **Important:** The CC2510 runs at **3.3 V**. Do not connect 5 V — it will damage the chip.

### Step 4 — Wire ESP32 → GL440

Default pin mapping of ESP_CC_Flasher (can be changed in the web UI):

| GL440 pad | ESP32 GPIO |
|---|---|
| DD | GPIO 23 |
| DC | GPIO 19 |
| RST | GPIO 33 |
| GND | GND |
| VCC | 3.3 V |

Keep wires short (< 20 cm). No level shifter needed — both sides are 3.3 V.

You can power the GL440 from the ESP32's 3.3 V pin during flashing instead of inserting the CR2450 battery.

### Step 5 — Read and back up the original firmware

Before erasing, try to read the existing firmware:

1. Open **http://cc.local**
2. Click **Read** → wait for the progress bar
3. Download the `.hex` file and keep it safe

> **Note:** If the chip has read-out protection active, the read will return all zeros or fail. A subsequent chip erase will bypass the protection, but the original firmware will be lost permanently.

### Step 6 — Flash the custom firmware

1. In the flasher web UI click **Choose File** and select `firmware.hex`
   (built locally with `make`, or downloaded from [Actions artifacts](../../actions))
2. Click **Write**
3. Wait for "Programming done" — takes about 10–15 seconds
4. Click **Verify** to confirm the flash was successful

### Step 7 — Verify with the Phase 1 test pattern

After flashing, insert the CR2450 battery (or keep powering from 3.3 V):

- The display should refresh and show **three vertical bands**:
  - Left: **black**
  - Centre: **red**
  - Right: **white**
- UART debug output (115200 8N1 on P0_2) prints:
  ```
  gl440 phase-1 boot
  epd init done
  test pattern sent
  epd sleeping
  ```

If all three colour bands appear correctly: **Phase 1 complete** ✅

#### Troubleshooting

| Symptom | Likely cause |
|---|---|
| Display stays blank / white | Bad RST or VCC wiring during flash; retry |
| Only black and white, no red | Red plane bytes inverted — check buf_red logic |
| Ghosting / partial refresh | BUSY line not connected or floating |
| Flasher shows "Chip ID: 0x00" | DD/DC lines swapped, or VCC not connected |
| Flasher shows "Chip ID: 0x91" ✅ | CC2510 detected correctly, ready to flash |

---

## UART Debug Output

The firmware prints status messages on **UART0** at **115200 baud, 8N1**.

| CC2510 pin | Signal | Notes |
|---|---|---|
| P0_2 | TX | Connect to RX of your USB-serial adapter |
| GND | GND | Common ground |

Any USB–serial adapter (CP2102, CH340, FTDI) at 3.3 V logic levels works.

---

## Project Structure

```
src/
  main.c              # entry point, Phase 1 test pattern
  display/
    epd.c             # IL0373 EPD driver (init, clear, display, stream, sleep)
    epd.h
  radio/
    radio.c           # CC2510 raw GFSK — stub (Phase 3)
    radio.h
  protocol/
    protocol.c        # frame parser / state machine — stub (Phase 5)
    protocol.h
  hal/
    clock.c           # 26 MHz XOSC init, delay_ms
    clock.h
    uart.c            # UART0 debug output (115200)
    uart.h
    sleep.c           # PM2 deep sleep via WOR timer
    sleep.h
docs/
  protocol.md         # radio frame format specification
Makefile
```

---

## Protocol

Frames exchanged with the AP use a simple custom format over GFSK 250 kbit/s:

```
[Sync 4B][Length 1B][NetID 1B][Dst 1B][Src 1B][Type 1B][Payload N B][CRC16 2B]
```

Frame types: `BEACON`, `CHECKIN_REQ`, `CHECKIN_RESP`, `IMG_CHUNK`, `IMG_ACK`, `IMG_DONE`.

See [`docs/protocol.md`](docs/protocol.md) for the full specification.

---

## Roadmap

| Phase | Goal | Status |
|---|---|---|
| 0 | Toolchain: SDCC builds `firmware.hex` | ✅ done |
| 1 | Display bring-up: BWR test pattern on screen | 🔧 flash & verify |
| 2 | UART loopback CC2510 ↔ ESP32 (AP unit) | planned |
| 3 | Raw radio PoC: two tags exchange packets | planned |
| 4 | ESP32 AP HTTP/WebSocket skeleton | planned |
| 5 | Home Assistant integration end-to-end | planned |
| 6 | Scale to 50+ tags, battery life validation | planned |

---

## References

- [CC2510 Datasheet (SWRS055G)](https://www.ti.com/lit/ds/symlink/cc2510.pdf)
- [IL0373 Datasheet](https://www.mikroshop.ch/pdf/IL0373.pdf)
- [angrymew/firmware-cc2510](https://github.com/angrymew/firmware-cc2510) — display driver reference
- [andrei-tatar/imagotag-hack](https://github.com/andrei-tatar/imagotag-hack) — SDCC build skeleton reference
- [atc1441/ESP_CC_Flasher](https://github.com/atc1441/ESP_CC_Flasher) — CC2510 flasher tool
- [GL440 teardown (Jirka Balhar)](https://blog.jirkabalhar.cz/2023/12/hacking-sesimagotag-e-ink-price-tag/) — PCB photos, pad locations

## License

MIT
