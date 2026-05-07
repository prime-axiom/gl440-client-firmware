# gl440-client-firmware

Custom CC2510 firmware for SES Imagotag VUSION 2.6 BWR GL440 e-ink price tags.

Replaces the proprietary Imagotag firmware with a lean open protocol stack that works with the companion [gl440-ap-firmware](https://github.com/prime-axiom/gl440-ap-firmware) access point.

## Overview

The GL440 tag uses a Texas Instruments **CC2510** (8051 core + 2.4 GHz radio, 32 KB flash). This firmware implements:

- IL0373-compatible EPD display driver (296 x 152 px, BWR)
- Raw GFSK 2.4 GHz radio receive loop
- Custom lightweight image transfer protocol
- Deep sleep between check-ins (CR2450 battery life target: 6–12 months)

## Hardware

| Component | Part |
|---|---|
| MCU + Radio | TI CC2510 (8051, 32 KB flash, 4 KB XRAM) |
| Display | Pervasive Displays EL026TR3 (2.66", BWR, IL0373 driver) |
| NFC | NXP NT3H2111 |
| SPI Flash | Winbond W25X10CL (1 Mbit) |

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
make          # produces firmware.hex
make clean    # remove build artefacts
```

Pre-built binaries are available as [GitHub Actions artifacts](../../actions) for every commit on `main`.

---

## Flashing

The CC2510 is programmed via its **CC-Debug** interface using an ESP32 running [atc1441/ESP_CC_Flasher](https://github.com/atc1441/ESP_CC_Flasher).

### What you need

| Item | Notes |
|---|---|
| Any ESP32 devboard | ESP32, ESP32-S3, etc. — not ESP8266 |
| 5 jumper wires | Female–female or female–male depending on your devboard |
| GL440 tag (opened) | See disassembly section below |
| USB cable | Data-capable |

### Step 1 — Flash ESP_CC_Flasher onto the ESP32

1. Clone and flash:
   ```bash
   git clone https://github.com/atc1441/ESP_CC_Flasher
   cd ESP_CC_Flasher
   pio run --target upload
   ```
2. The ESP32 creates a Wi-Fi AP named **`CC_Flasher`** (password: `12345678`).
3. Connect to it and open **http://cc.local** — you should see the flasher web UI.

### Step 2 — Open the GL440

The housing is held by two plastic clips on the short sides. Use a flat spudger or guitar pick to release them — no screws.

### Step 3 — Locate the debug pads

The debug pads are on the back of the PCB:

| Signal | CC2510 pin | Function |
|---|---|---|
| `DD` | P2_1 | Debug Data (bidirectional) |
| `DC` | P2_2 | Debug Clock |
| `RST` | RESET_N | Reset (active low) |
| `GND` | GND | Ground |
| `VCC` | VDD | 3.3 V supply |

> **Important:** The CC2510 runs at **3.3 V**. Do not connect 5 V.

### Step 4 — Wire ESP32 → GL440

| GL440 pad | ESP32 GPIO |
|---|---|
| DD | GPIO 23 |
| DC | GPIO 19 |
| RST | GPIO 33 |
| GND | GND |
| VCC | 3.3 V |

Keep wires short (< 20 cm). You can power the GL440 from the ESP32's 3.3 V pin instead of the CR2450 battery during flashing.

### Step 5 — Back up the original firmware

Before erasing, click **Read** in the web UI and save the downloaded `.hex` file.

> If read-out protection is active, the read will fail. A chip erase will bypass it, but the original firmware will be lost permanently.

### Step 6 — Flash

1. Click **Choose File** and select `firmware.hex`
2. Click **Write** — takes about 10–15 seconds
3. Click **Verify** to confirm

### Troubleshooting

| Symptom | Likely cause |
|---|---|
| Flasher shows "Chip ID: 0x00" | DD/DC lines swapped, or VCC not connected |
| Flasher shows "Chip ID: 0x91" ✅ | CC2510 detected correctly |
| Display stays blank after flash | Bad RST or VCC wiring; retry flash |
| Ghosting / partial refresh | BUSY line not connected or floating |

---

## UART Debug Output

Status messages are printed on **UART0** at **115200 baud, 8N1**.

| CC2510 pin | Signal |
|---|---|
| P0_2 | TX → connect to RX of a 3.3 V USB–serial adapter |
| GND | GND |

---

## Project Structure

```
src/
  main.c              # entry point
  display/
    epd.c / epd.h     # IL0373 EPD driver
  radio/
    radio.c / radio.h # CC2510 GFSK radio
  protocol/
    protocol.c / .h   # image transfer protocol
  hal/
    clock.c / .h      # oscillator init, delay_ms
    uart.c / .h       # UART0 debug output
    sleep.c / .h      # PM2 deep sleep
docs/
  protocol.md         # radio frame format specification
Makefile
```

---

## References

- [CC2510 Datasheet (SWRS055G)](https://www.ti.com/lit/ds/symlink/cc2510.pdf)
- [IL0373 Datasheet](https://www.mikroshop.ch/pdf/IL0373.pdf)
- [atc1441/ESP_CC_Flasher](https://github.com/atc1441/ESP_CC_Flasher)
- [GL440 teardown (Jirka Balhar)](https://blog.jirkabalhar.cz/2023/12/hacking-sesimagotag-e-ink-price-tag/)

## License

MIT
