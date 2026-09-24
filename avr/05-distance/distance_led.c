#define F_CPU 16000000UL
#include <stdint.h>
#include <util/delay.h>

#define UCSR0A (*(volatile uint8_t  *)0xC0)   // UART status
#define UCSR0B (*(volatile uint8_t  *)0xC1)   // UART enable
#define UBRR0L (*(volatile uint8_t  *)0xC4)   // UART speed
#define UDR0   (*(volatile uint8_t  *)0xC6)   // UART data out

#define DDRB   (*(volatile uint8_t  *)0x24)   // port B direction
#define PORTB  (*(volatile uint8_t  *)0x25)   // port B output
#define PINB   (*(volatile uint8_t  *)0x23)   // port B read
#define DDRD   (*(volatile uint8_t  *)0x2A)   // port D direction
#define PORTD  (*(volatile uint8_t  *)0x2B)   // port D output

#define TCCR1B (*(volatile uint8_t  *)0x81)   // Timer1 control
#define TCNT1  (*(volatile uint16_t *)0x84)   // Timer1 count (16-bit)

#define TXEN0 3 
#define UDRE0 5

void uart_putc(char c) {
    while (!(UCSR0A & (1 << UDRE0))) { }
    UDR0 = c;
}

void uart_print_number(uint16_t n) {
    uart_putc('0' + n / 100);
    uart_putc('0' + (n / 10) % 10);
    uart_putc('0' + n % 10);
    uart_putc('\r');
    uart_putc('\n');
}

int main(void) {
    UBRR0L = 103;              // 9600 baud
    UCSR0B |= (1 << TXEN0);    // TXEN0
    TCCR1B = (1 << 1);         // Timer1, prescaler /8
    DDRB |=  (1 << 0);         // PB0 (D8) Trig, output
    DDRB &= ~(1 << 1);         // PB1 (D9) Echo, input
    DDRD |=  (1 << 7);         // PD7 (D7) red LED, output
    DDRD |=  (1 << 6);         // PD6 (D6) yellow LED, output

    uint8_t near = 0;          // 0 = far, 1 = near (hysteresis state)

    while (1) {
        PORTB |=  (1 << 0);
        _delay_us(10);
        PORTB &= ~(1 << 0);

        uint8_t valid = 1;

        uint16_t guard = 0;
        while (!(PINB & (1 << 1))) {       // wait for Echo to rise
            if (++guard > 60000) {
                valid = 0;
                break;
            }
        }

        TCNT1 = 0;

        while (PINB & (1 << 1)) {          // wait for Echo to fall
            if (TCNT1 > 46400) {           // ~400 cm, out of range
                valid = 0;
                break;
            }
        }

        uint16_t cm = TCNT1 / 116;

        if (valid) {
            if (near == 0 && cm < 20) {
                near = 1;
            } else if (near == 1 && cm > 25) {
                near = 0;
            }

            if (near) {
                PORTD &= ~(1 << 6);
                PORTD |=  (1 << 7);
            } else {
                PORTD &= ~(1 << 7);
                PORTD |=  (1 << 6);
            }

            uart_print_number(cm);
        } else {
            uart_putc('X');
            uart_putc('\r');
            uart_putc('\n');
        }

        _delay_ms(60);
    }
}