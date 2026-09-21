# 02 — Button

Reads a push button on PD2 (D2) and drives the LED on PD7 (D7).

- `button_follow.c` — LED on while the button is held (150 B)
- `button_toggle.c` — each press flips the LED (166 B)

## How it works

**Pull-up.** PD2 is an input with its internal pull-up enabled (PORTD bit 2
set while DDRD bit 2 is clear). Without it, the pin floats and reads noise
when the button is released.

**Active-low.** The button connects PD2 to GND, so pressed reads 0 and
released reads 1.

**Edge detection.** The toggle compares the current reading with the previous
one and acts only on a 1 → 0 transition, so one press toggles once rather than
on every loop pass.

**Debounce.** The contacts bounce for a few ms on each press. A 20 ms delay
after a detected press ignores the bounces.

## Limitations

The delay blocks the CPU: nothing else runs for 20 ms after each press.
A timer-based debounce would fix this.

Wiring: button between D2 and GND. LED as in lab 01.
