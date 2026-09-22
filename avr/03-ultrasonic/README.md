# 03 — Ultrasonic (HC-SR04)

Triggers an HC-SR04 and mirrors its Echo pin onto the LED on PD7.
Far objects give longer echo pulses, so the LED looks brighter. 182 B

## How it works

**Trigger.** A 10 µs high pulse on Trig (PB0, D8) starts a measurement.

**Echo.** The sensor holds Echo (PB1, D9) high for the sound's round trip.
Distance in cm ≈ pulse length in µs ÷ 58.

**Mirroring.** For 60 ms after each trigger, the loop copies PB1 to PD7
every 100 µs. 60 ms is the minimum gap between triggers.

## Limitations

Nothing is measured: the LED shows the echo, but the program never knows the
distance. Measuring the pulse needs a hardware timer (Timer1).

Wiring: VCC → 5V rail, GND → GND rail, Trig → D8, Echo → D9. LED as in lab 01.
