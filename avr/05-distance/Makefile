TARGET = timer_test
MCU    = atmega328p
F_CPU  = 16000000UL
PORT   = /dev/cu.usbserial-A50285BI
BAUD   = 57600

all: $(TARGET).hex

$(TARGET).elf: $(TARGET).c
	avr-gcc -mmcu=$(MCU) -DF_CPU=$(F_CPU) -Os -Wall -o $(TARGET).elf $(TARGET).c

$(TARGET).hex: $(TARGET).elf
	avr-objcopy -O ihex -R .eeprom $(TARGET).elf $(TARGET).hex
	avr-size $(TARGET).elf

flash: $(TARGET).hex
	avrdude -c arduino -p $(MCU) -P $(PORT) -b $(BAUD) -U flash:w:$(TARGET).hex:i

clean:
	rm -f $(TARGET).elf $(TARGET).hex
