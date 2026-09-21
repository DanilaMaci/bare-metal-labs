#define F_CPU 16000000UL
#include <stdint.h>
#include <util/delay.h>

#define DDRB  (*(volatile uint8_t *)0x24)
#define PORTB (*(volatile uint8_t *)0x25)
#define DDRD  (*(volatile uint8_t *)0x2A)
#define PORTD (*(volatile uint8_t *)0x2B)
#define PIND  (*(volatile uint8_t *)0x29)


int main(void) {
    DDRD  |= (1 << 7);      // PD7 output: the LED
    DDRD  &= ~(1 << 2);     // PD2 input: the button (already 0, but be explicit)
    PORTD |= (1 << 2);      // pull-up on for PD2

    while (1) {
        if (PIND & (1 << 2)) {
            PORTD &= ~(1 << 7);   // reads 1 = released = LED off
        } else {
            PORTD |= (1 << 7);    // reads 0 = pressed = LED on
        }
    }
}
