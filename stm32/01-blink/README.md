# STM32 01 — Bare-metal blink

Blinks LD2 (PA5) on a Nucleo-F446RE with a hand-written linker script, startup code and Makefile — no HAL, no CubeMX, no generated files. 136 B of flash.

## Memory map

| Region | Start | Size |
|---|---|---|
| Flash | `0x08000000` | 512 KB |
| RAM | `0x20000000` | 128 KB |

`linker.ld` places `.isr_vector` first in flash, then `.text`. `.data` is the awkward one: `int x = 5;` must be writable so it lives in RAM, but RAM is empty at power-on, so the value is stored in flash and copied across at boot. That's what `> RAM AT> FLASH` means.

## Boot

The Cortex-M4 reads the first two words of flash in hardware, before any code runs: word 0 becomes the stack pointer, word 1 is the address it jumps to. `reset_handler` then copies `.data` into RAM, zeros `.bss` (C guarantees uninitialised globals start at zero — someone has to write those zeros), and calls `main()`.

`KEEP(*(.isr_vector))` matters: nothing references the vector table, so `--gc-sections` would delete it and the chip would never boot.

## Clock gating

    RCC_AHB1ENR |= (1U << 0);        // GPIOAEN

No AVR equivalent. Every STM32 peripheral starts with its clock off and silently ignores writes until it's enabled. Most common cause of "the code looks right but nothing happens".

## Size, honestly

136 B here against 172 B for the AVR blink — but not because ARM is denser, it isn't. The AVR build spent most of its bytes on a full compiler-generated vector table and runtime. This one has a 2-entry vector table I wrote myself.

## Build and debug

    make && make flash

    # two terminals:
    openocd -f board/st_nucleo_f4.cfg
    make debug
    (gdb) load / break main / continue / next / print/x GPIOA_MODER

## Limitations

- Runs on the default 16 MHz HSI; the PLL isn't configured, so the chip is at a tenth of its 180 MHz capability.
- `delay()` is a busy loop, not a timer.
- Only 2 vector slots — any fault jumps to an address that was never filled in.

Nucleo-F446RE, Mini-USB cable, nothing external. Chip runs at 3.3 V, not 5 V.
