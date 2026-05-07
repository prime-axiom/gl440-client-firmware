CC      = sdcc
TARGET  = firmware

# Source files
SRCS    = src/main.c \
          src/display/epd.c \
          src/radio/radio.c \
          src/protocol/protocol.c \
          src/hal/clock.c \
          src/hal/uart.c \
          src/hal/sleep.c

RELS    = $(SRCS:.c=.rel)

CFLAGS  = --model-small --opt-code-speed
LDFLAGS = --out-fmt-ihx \
          --code-loc 0x000  --code-size 0x7FFF \
          --xram-loc 0xF000 --xram-size 0x0F00 \
          --iram-size 0x100

.PHONY: all clean

all: $(TARGET).hex

$(TARGET).hex: $(TARGET).ihx
	packihx $< > $@

$(TARGET).ihx: $(RELS)
	$(CC) $(LDFLAGS) $(CFLAGS) -o $@ $^

%.rel: %.c
	$(CC) -c $(CFLAGS) -o $@ $<

clean:
	find . -name "*.rel" -o -name "*.asm" -o -name "*.lst" \
	       -o -name "*.sym" -o -name "*.map" -o -name "*.mem" \
	       -o -name "*.ihx" -o -name "*.hex" | xargs rm -f
