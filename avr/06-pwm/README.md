# 06 - Servo control with hardware PWM

Sweeps an SG90 servo between 45 and 180 degrees using Timer1 in Fast PWM mode.
No `analogWrite`, no Servo library - the timer generates the pulse train in
hardware, and the CPU only writes a new compare value when the angle changes.

## Wiring

| Servo wire | Goes to |
|---|---|
| orange (signal) | D10 / PB2 |
| red (power) | 5V |
| brown (ground) | GND |

A servo under load draws more current than the USB port comfortably supplies.
If it twitches or the board resets mid-sweep, power the servo separately and
tie the grounds together.

## Timing

Timer1 runs at 16 MHz / 8 = 2 MHz, so **one tick = 0.5 us**.

| | ticks | time |
|---|---|---|
| ICR1 (counter top) | 39,999 | 20 ms period = 50 Hz |
| OCR1B at 0 deg | 950 | 475 us |
| OCR1B at 180 deg | 5,050 | 2,525 us |

The servo responds to **pulse width**, not duty cycle. The usual 1-2 ms figure
didn't reach full travel on this unit, so the endpoints were found by hand.
950 and 5050 are measured, not assumed.

## The bug worth knowing about

`degrees * (SERVO_MAX - SERVO_MIN)` is `degrees * 4100`, which overflows 16
bits above about 15 degrees. Every angle past that wraps and bunches near one
end of the travel. The `(uint32_t)` cast in `set_angle` is the fix.

## Build

    make          # compile
    make flash    # upload

376 bytes of flash.
EOF