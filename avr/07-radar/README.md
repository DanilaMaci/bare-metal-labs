# 07 - Ultrasonic radar

Sweeps the servo in 5-degree steps, pings an HC-SR04 at each position, and
sends `angle,distance` over UART. `radar.py` draws the result as a polar plot
with a fading tail.

Combines labs 03 (ultrasonic), 04 (UART) and 06 (servo PWM).

## Wiring

| Pin | Goes to |
|---|---|
| D8 / PB0 | HC-SR04 Trig |
| D9 / PB1 | HC-SR04 Echo |
| D10 / PB2 | servo signal |
| 5V, GND | sensor and servo power |

## The interesting part

Timer1 does two jobs at once. It generates the servo's 50 Hz PWM in hardware,
*and* it is read directly as a stopwatch for the echo pulse. That means the
counter can never be zeroed - it has a servo to drive - so `measure_cm` reads
TCNT1 at the start and end of the echo and subtracts:

    if (end >= start) ticks = end - start;
    else              ticks = (TIMER_TOP - start) + end;

The second branch handles the counter wrapping at 40,000 partway through a
measurement, which happens every 20 ms.

## Distance maths

Sound travels 1 cm and back in 58 us. One tick is 0.5 us.

    58 us / 0.5 us = 116 ticks per cm

Both wait loops have a guard counter and return 999 on timeout, so a missing
echo stalls one reading instead of hanging the program forever.

## Output format

    045,023
    050,022
    055,187

Three digits each, comma separated, CRLF terminated. Fixed width means the
Python side never has to handle a partial number.

## Running the display

    python3 radar.py                              # fake data, no hardware
    python3 radar.py /dev/cu.usbserial-A50285BI   # live

## Build

    make
    make flash

648 bytes of flash.
EOF