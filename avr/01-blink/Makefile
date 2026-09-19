MCU    = atmega328p
F_CPU  = 16000000UL
PORT   = /dev/cu.usbserial-A50285BI
BAUD   = 57600

all: blink.hex

blink.elf: blink.c
	avr-gcc -mmcu=$(MCU) -DF_CPU=$(F_CPU) -Os -Wall -o blink.elf blink.c

blink.hex: blink.elf
	avr-objcopy -O ihex -R .eeprom blink.elf blink.hex
	avr-size blink.elf

flash: blink.hex
	avrdude -c arduino -p $(MCU) -P $(PORT) -b $(BAUD) -U flash:w:blink.hex:i

clean:
	rm -f blink.elf blink.hex
