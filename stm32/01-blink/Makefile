TARGET  = blink

CC      = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
SIZE    = arm-none-eabi-size

CFLAGS  = -mcpu=cortex-m4 -mthumb -Wall -Os -g3
CFLAGS += -ffunction-sections -fdata-sections
CFLAGS += -nostdlib -nostartfiles

LDFLAGS = -T linker.ld -Wl,--gc-sections -Wl,-Map=$(TARGET).map

SRCS    = startup.c main.c

all: $(TARGET).bin

$(TARGET).elf: $(SRCS) linker.ld
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(SRCS)
	$(SIZE) $@

$(TARGET).bin: $(TARGET).elf
	$(OBJCOPY) -O binary $< $@

flash: $(TARGET).elf
	openocd -f board/st_nucleo_f4.cfg -c "program $(TARGET).elf verify reset exit"

debug: $(TARGET).elf
	arm-none-eabi-gdb $(TARGET).elf -ex "target extended-remote localhost:3333"

clean:
	rm -f $(TARGET).elf $(TARGET).bin $(TARGET).map