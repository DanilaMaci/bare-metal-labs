# Embedded Labs

Learning embedded systems from first principles: writing directly to hardware
registers, no vendor libraries. Toolchain: `avr-gcc`, `avrdude`, plain Makefiles.

| # | Lab | Chip | Concepts | Flash |
|---|-----|------|----------|-------|
| 01 | [Blink](avr/01-blink/) | ATmega328P | GPIO registers, bit masking, `volatile` | 172 B |
| 02 | [Button](avr/02-button/) | ATmega328P | Inputs, pull-ups, edge detection, debounce | 150–166 B |
| 03 | [Ultrasonic](avr/03-ultrasonic/) | ATmega328P | HC-SR04 trigger and echo protocol | 182 B |
| 04 | [UART](avr/04-uart/) | ATmega328P | Serial framing, baud rate, ASCII digits | 326 B |
| 05 | [Distance](avr/05-distance/) | ATmega328P | Timer1 input timing, timeouts, hysteresis | 378 B |
