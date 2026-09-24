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

#define TCCR1B (*(volatile uint8_t  *)0x81)   // Timer1 control
#define TCNT1  (*(volatile uint16_t *)0x84)   // Timer1 count (16-bit)

void uart_putc(char c) {
    while (!(UCSR0A & (1 << 5))) { }
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
    UBRR0L = 103;
    UCSR0B |= (1 << 3);
    TCCR1B = (1 << 1);
    DDRB |=  (1 << 0);
    DDRB &= ~(1 << 1);

    while (1) {
        PORTB |=  (1 << 0);
        _delay_us(10);
        PORTB &= ~(1 << 0);

        uint16_t guard = 0;
        while (!(PINB & (1 << 1))) {
            if (++guard > 60000) break;
        }

        TCNT1 = 0;

        guard = 0;
        while (PINB & (1 << 1)) {
            if (++guard > 60000) break;
        }

        uint16_t ticks = TCNT1;
        uart_print_number(ticks / 116);
        _delay_ms(60);
    }
}