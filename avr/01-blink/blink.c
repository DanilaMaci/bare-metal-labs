#define F_CPU 16000000UL
#include <stdint.h>
#include <util/delay.h>


#define DDRB (*(volatile uint8_t *)0x24)
#define PORTB (*(volatile uint8_t *)0x25)
#define DDRD (*(volatile uint8_t *)0x2A)
#define PORTD (*(volatile uint8_t *)0x2B)

int main(void) {
	// setup: runs once
	DDRB |= (1 << 5);
	DDRD |= (1 << 7);

	PORTD |= (1 << 7);

	while (1) {
		//loop:runs forever
		PORTB ^= (1 << 5);
		PORTD ^= (1 << 7);
		_delay_ms(500);
	}
}


