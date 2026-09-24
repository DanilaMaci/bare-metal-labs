# 05 — Ultrasonic distance over UART

Measures distance with an HC-SR04 and prints it to a host terminal.

- `timer_test.c` — verifies Timer1 by measuring a known 10 ms delay (350 B)
- `distance.c` — distance in cm over UART, with timeout handling (378 B)
- `distance_led.c` — adds near/far LEDs with hysteresis (462 B)

## How it works

**Timer1 as a stopwatch.** Prescaler /8 gives 2,000,000 ticks per second, so
one tick is 0.5 µs. TCNT1 is zeroed when Echo rises and read when it falls;
the value is the pulse length in ticks.

**Calibrating the timer first.** `timer_test.c` measures a `_delay_ms(10)`
and should read 20,000 ticks. Verifying the stopwatch against a known
interval before trusting it on a real signal.

**Distance.** Sound travels ~343 m/s, so 29 µs per cm each way, 58 µs per cm
of distance. At 0.5 µs per tick that's 116 ticks per cm.

**Validity.** If Echo never rises, or stays high past 46,400 ticks (~400 cm),
the reading is marked invalid and printed as X rather than reported as a
number.

**Hysteresis** (`distance_led.c`). The near LED turns on below 20 cm but does
not release until 25 cm. Without the gap, readings jittering around a single
threshold make the LEDs flicker several times a second.

## Limitations

- Timer1 wraps at 65,535 ticks (32.8 ms). The sensor holds Echo high for
  about 38 ms when nothing is in range, which would wrap and report a false
  short distance. The timeout avoids this rather than fixing it.
- Blocking: the CPU spins in a wait loop for the whole measurement.
- 58 µs/cm assumes ~20 °C; the speed of sound changes ~0.6 m/s per °C, so
  readings drift a few percent with temperature.

Wiring: Trig → D8, Echo → D9, sensor VCC → 5 V rail, GND → GND rail.
LEDs on D7 (near) and D6 (far), each with its own 180 Ω resistor.