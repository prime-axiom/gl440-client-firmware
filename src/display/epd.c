/*
 * IL0373 EPD driver for GL440 (296 x 152 px, BWR)
 *
 * Pin mapping (verified against angrymew/firmware-cc2510):
 *   EPD_PWR  = P0_0  — panel power enable (active low)
 *   EPD_CS   = P0_1  — SPI chip select    (active low)
 *   MOSI     = P0_3  — USART0 alt. location 1
 *   SCK      = P0_5  — USART0 alt. location 1
 *   EPD_DC   = P1_2  — data/command select (low = command, high = data)
 *   EPD_BUSY = P1_3  — busy signal         (low = busy)
 *   EPD_RST  = P2_0  — hardware reset      (active low)
 *
 * IL0373 command subset used:
 *   0x00  Panel Setting
 *   0x04  Power On
 *   0x06  Booster Soft-Start
 *   0x10  Data Start Transmission 1 (BW plane)
 *   0x12  Display Refresh
 *   0x13  Data Start Transmission 2 (Red plane)
 *   0x50  VCOM and Data Interval Setting
 *   0x61  Resolution Setting
 *   0x02  Power Off
 *   0x07  Deep Sleep
 */

#include <cc2510fx.h>
#include "epd.h"
#include "../hal/clock.h"

/* --- Pin aliases --------------------------------------------------------- */
#define EPD_PWR   P0_0
#define EPD_CS    P0_1
#define EPD_DC    P1_2
#define EPD_BUSY  P1_3
#define EPD_RST   P2_0

#define PWR_ON    (EPD_PWR = 0)
#define PWR_OFF   (EPD_PWR = 1)
#define RST_LOW   (EPD_RST = 0)
#define RST_HIGH  (EPD_RST = 1)
#define CS_LOW    (EPD_CS  = 0)
#define CS_HIGH   (EPD_CS  = 1)
#define DC_CMD    (EPD_DC  = 0)
#define DC_DATA   (EPD_DC  = 1)

/* --- Internal helpers ---------------------------------------------------- */

static void spi_write(uint8_t byte)
{
    CS_LOW;
    U0DBUF = byte;
    while (U0CSR & 0x01);   /* wait for TX complete flag to clear */
    CS_HIGH;
}

static void send_cmd(uint8_t cmd)
{
    DC_CMD;
    spi_write(cmd);
    DC_DATA;
}

static void send_data(uint8_t data)
{
    spi_write(data);
}

static void wait_busy(void)
{
    while (EPD_BUSY == 0);  /* BUSY is active-low: wait until high */
    delay_ms(200);
}

/* --- Public API ---------------------------------------------------------- */

void epd_init(void)
{
    /* USART0 SPI master, alternative location 1 (MOSI=P0_3, SCK=P0_5) */
    PERCFG &= ~0x01;
    U0CSR   = 0x00;                 /* SPI mode, master */
    U0GCR   = (1 << 5) | 17;       /* SCK idle low, MSB first, baud E=17 */
    U0BAUD  = 0;                    /* baud M=0 → ~4 MHz @ 26 MHz XOSC */
    U0CSR  |= 0x40;                 /* enable SPI (bit 6) */

    /* GPIO directions (bit masks instead of BV() macro) */
    P0SEL  |= 0x28;    /* P0_3=MOSI, P0_5=SCK → peripheral (bits 3,5) */
    P0DIR  |= 0x2b;    /* P0_0=PWR, P0_1=CS, P0_3=MOSI, P0_5=SCK → output */
    P1DIR  |= 0x04;    /* P1_2=DC → output */
    P1DIR  &= ~0x08;   /* P1_3=BUSY → input */
    P2DIR  |= 0x01;    /* P2_0=RST → output */

    /* Hardware reset sequence */
    PWR_ON;
    delay_ms(10);
    RST_LOW;
    delay_ms(10);
    RST_HIGH;
    delay_ms(10);

    /* Booster soft-start */
    send_cmd(0x06);
    send_data(0x17);
    send_data(0x17);
    send_data(0x17);

    /* Power on */
    send_cmd(0x04);
    wait_busy();

    /* Panel setting: LUT from OTP, KW/R mode, scan up, shift right, booster on */
    send_cmd(0x00);
    send_data(0x0f);
    send_data(0x0d);

    /* Resolution: 152 x 296 */
    send_cmd(0x61);
    send_data(EPD_HRES);
    send_data((uint8_t)(EPD_VRES >> 8));
    send_data((uint8_t)(EPD_VRES));

    /* VCOM interval: WBmode=VBDF, DDX=1, CDI=0x17 */
    send_cmd(0x50);
    send_data(0x77);

    epd_clear();
}

void epd_clear(void)
{
    uint16_t i;

    send_cmd(0x10);
    for (i = 0; i < EPD_BUFFER_SIZE; i++)
        send_data(0xff);    /* white */

    send_cmd(0x13);
    for (i = 0; i < EPD_BUFFER_SIZE; i++)
        send_data(0xff);    /* no red */

    send_cmd(0x12);         /* display refresh */
    delay_ms(100);
    wait_busy();
}

void epd_display(const uint8_t *buf_bw, const uint8_t *buf_red)
{
    uint16_t i;

    wait_busy();

    send_cmd(0x10);
    for (i = 0; i < EPD_BUFFER_SIZE; i++)
        send_data(buf_bw[i]);

    send_cmd(0x13);
    for (i = 0; i < EPD_BUFFER_SIZE; i++)
        send_data(buf_red[i]);

    send_cmd(0x12);
    delay_ms(100);
    wait_busy();
}

/* --- Streaming API (no full-frame XRAM buffer needed) -------------------- */

void epd_stream_start_bw(void)
{
    wait_busy();
    send_cmd(0x10);
}

void epd_stream_start_red(void)
{
    send_cmd(0x13);
}

void epd_stream_byte(uint8_t b)
{
    send_data(b);
}

void epd_stream_refresh(void)
{
    send_cmd(0x12);
    delay_ms(100);
    wait_busy();
}

/* --- Deep sleep ---------------------------------------------------------- */

void epd_sleep(void)
{
    send_cmd(0x02);     /* power off */
    wait_busy();
    send_cmd(0x07);     /* deep sleep */
    send_data(0xa5);    /* check code */
}
