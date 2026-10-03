#define F_CPU 16000000UL
#include <stdint.h>
#include <util/delay.h>

/* ---- UART registers ---- */
#define UCSR0A (*(volatile uint8_t  *)0xC0)   // status: bit UDRE0 = ready for next byte
#define UCSR0B (*(volatile uint8_t  *)0xC1)   // enable: bit TXEN0 = transmitter on
#define UBRR0L (*(volatile uint8_t  *)0xC4)   // baud rate divisor
#define UDR0   (*(volatile uint8_t  *)0xC6)   // write a byte here to send it

/* ---- Port B: D8 Trig, D9 Echo, D10 servo ---- */
#define DDRB   (*(volatile uint8_t  *)0x24)   // direction: 1 = output
#define PORTB  (*(volatile uint8_t  *)0x25)   // output level
#define PINB   (*(volatile uint8_t  *)0x23)   // read the actual pin

/* ---- Timer1: PWM for the servo, and the stopwatch for the echo ---- */
#define TCCR1A (*(volatile uint8_t  *)0x80)   // mode bits + connect pin to timer
#define TCCR1B (*(volatile uint8_t  *)0x81)   // mode bits + prescaler
#define TCNT1  (*(volatile uint16_t *)0x84)   // the live count
#define ICR1   (*(volatile uint16_t *)0x86)   // PWM period (counter top)
#define OCR1B  (*(volatile uint16_t *)0x8A)   // PWM switch point (pulse width)

/* ---- Bit names, from the datasheet's register diagrams ---- */
#define TXEN0  3
#define UDRE0  5

/* ---- Servo calibration: measured, not assumed ---- */
#define SERVO_MIN  950    // ticks at 0°   (475 µs)
#define SERVO_MAX  5050   // ticks at 180° (2525 µs)

#define TIMER_TOP  40000  // ICR1 + 1: the counter wraps here every 20 ms


/* Send one character. Waits until the transmitter is free, because
   writing to UDR0 early would overwrite a byte still going out. */
void uart_putc(char c) {
    while (!(UCSR0A & (1 << UDRE0))) { }
    UDR0 = c;
}

/* Three digits, no line break. UART sends bytes, not numbers, so each
   digit is converted to its ASCII code by adding '0' (48). */
void uart_print_3(uint16_t n) {
    uart_putc('0' + n / 100);
    uart_putc('0' + (n / 10) % 10);
    uart_putc('0' + n % 10);
}

/* Same, then end the line. '\r' returns the cursor, '\n' moves down. */
void uart_print_line(uint16_t n) {
    uart_print_3(n);
    uart_putc('\r');
    uart_putc('\n');
}


void setup(void) {
    /* UART: 16,000,000 / (16 × 9600) − 1 = 103, giving 0.16% error */
    UBRR0L = 103;
    UCSR0B |= (1 << TXEN0);

    /* Pins */
    DDRB |=  (1 << 2);    // PB2 (D10) servo signal, output
    DDRB |=  (1 << 0);    // PB0 (D8)  Trig, output
    DDRB &= ~(1 << 1);    // PB1 (D9)  Echo, input

    /* Timer1: Fast PWM, top = ICR1, prescaler /8 so one tick = 0.5 µs.
       COM1B1 hands PB2 over to the timer hardware. */
    TCCR1A = (1 << 5) | (1 << 1);
    TCCR1B = (1 << 4) | (1 << 3) | (1 << 1);
    ICR1   = TIMER_TOP - 1;        // 40,000 ticks × 0.5 µs = 20 ms → 50 Hz
}

/* The servo reads pulse WIDTH, not duty cycle. The cast to uint32_t matters:
   degrees × 4100 overflows 16 bits above ~15°, which would bunch every angle
   near one end. */
void set_angle(uint8_t degrees) {
    OCR1B = SERVO_MIN + ((uint32_t)degrees * (SERVO_MAX - SERVO_MIN)) / 180;
}

/* Ping, time the echo, convert to cm. Returns 999 if no valid echo.

   Timer1 can't be zeroed here — it's generating the servo's PWM — so the
   start and end are read and subtracted instead. */
uint16_t measure_cm(void) {
    PORTB |=  (1 << 0);          // Trig high
    _delay_us(10);               // the sensor's "go" signal
    PORTB &= ~(1 << 0);          // Trig low; the sensor now chirps

    uint16_t guard = 0;
    while (!(PINB & (1 << 1))) { // wait for Echo to rise
        if (++guard > 60000) return 999;
    }

    uint16_t start = TCNT1;

    guard = 0;
    while (PINB & (1 << 1)) {    // wait for Echo to fall
        if (++guard > 60000) return 999;
    }

    uint16_t end = TCNT1;

    /* The counter resets every 20 ms, so end can be smaller than start. */
    uint16_t ticks;
    if (end >= start) {
        ticks = end - start;
    } else {
        ticks = (TIMER_TOP - start) + end;
    }

    /* Sound covers 1 cm of distance in 58 µs (29 µs each way),
       and one tick is 0.5 µs, so 116 ticks = 1 cm. */
    return ticks / 116;
}


int main(void) {
    setup();

    while (1) {
        for (uint8_t a = 45; a <= 180; a += 5) {
            set_angle(a);
            _delay_ms(150);              // let the servo travel and settle

            uint16_t cm = measure_cm();

            uart_print_3(a);             // "045"
            uart_putc(',');
            uart_print_line(cm);         // "023\r\n"

            _delay_ms(60);               // sensor's minimum gap between pings
        }
        for (uint8_t a = 180; a >= 45; a -= 5) {
            set_angle(a);
            _delay_ms(150);              // let the servo travel and settle

            uint16_t cm = measure_cm();

            uart_print_3(a);             // "045"
            uart_putc(',');
            uart_print_line(cm);         // "023\r\n"

            _delay_ms(60);               // sensor's minimum gap between pings
        }
    }
}