#define F_CPU 16000000UL
#include <stdint.h>
#include <util/delay.h>

#define UCSR0A (*(volatile uint8_t *)0xC0)
#define UCSR0B (*(volatile uint8_t *)0xC1)
#define UBRR0L (*(volatile uint8_t *)0xC4)
#define UDR0   (*(volatile uint8_t *)0xC6)

#define TCCR1B (*(volatile uint8_t  *)0x81) // control bits, chooses prescaler
#define TCNT1  (*(volatile uint16_t *)0x84) // the count itself choosing 16 bits

void uart_putc(char c) {
    while (!(UCSR0A & (1 << 5))) { }   // wait until the UART is ready
    UDR0 = c;                          // send the character
}


void uart_print_number(uint16_t n) {
    uint8_t d1 = n / 100;
    uint8_t d2 = (n / 10) % 10;
    uint8_t d3 = n % 10;
    uart_putc('0' + d1);
    uart_putc('0' + d2);
    uart_putc('0' + d3);
    uart_putc('\r');
    uart_putc('\n');
}

int main(void) {
    UBRR0L = 103;              // 9600 baud at 16 MHz
    UCSR0B |= (1 << 3);        // TXEN0: transmitter on
    TCCR1B = (1 << 1);            // setup: start Timer1, prescaler ÷8

    while (1) {
        TCNT1 = 0;
        _delay_ms(10);
        uint16_t ticks = TCNT1;
        uart_print_number(ticks / 100);
        _delay_ms(500);
    }
}
