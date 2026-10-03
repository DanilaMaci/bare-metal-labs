# 08 - DC motor control with a TB6612FNG

Drives two DC motors forward, backward and spinning on the spot. Direction
comes from two GPIO pins per motor, speed from hardware PWM on Timer0.

## Wiring

| Nano pin | TB6612 pin | Purpose |
|---|---|---|
| D7 / PD7 | AIN1 | motor A direction |
| D4 / PD4 | AIN2 | motor A direction |
| D6 / PD6 | PWMA | motor A speed (OC0A) |
| D3 / PD3 | BIN1 | motor B direction |
| D2 / PD2 | BIN2 | motor B direction |
| D5 / PD5 | PWMB | motor B speed (OC0B) |
| 5V | VCC, **STBY** | logic supply |
| battery + | VM | motor supply |
| GND | GND | shared with the battery ground |

**STBY must be tied HIGH.** The chip sits in standby and does nothing at all
otherwise - no error, no movement, which makes it a confusing first failure.

D0 and D1 are deliberately left alone: they are the USB serial pins.

## Direction

The TB6612 truth table, with STBY high:

| IN1 | IN2 | result |
|---|---|---|
| H | L | forward at PWM duty |
| L | H | reverse at PWM duty |
| L | L | coast |
| H | H | brake |

`motor_set` takes a signed speed from -255 to +255 and picks the row: sign
chooses the direction pins, magnitude goes into the compare register.

## Speed

Timer0, Fast PWM, counting 0-255 and wrapping. Prescaler /64:

    16,000,000 / 64 / 256 = 976 Hz

Same frequency the Arduino core uses for `analogWrite`. The pin goes HIGH at
count 0 and LOW when the count reaches OCR0A/OCR0B, so the register value *is*
the duty cycle.

## Build

    make
    make flash

564 bytes of flash. The 2-second delay at the start of `main` is there to put
the chassis down after a reset.
EOF