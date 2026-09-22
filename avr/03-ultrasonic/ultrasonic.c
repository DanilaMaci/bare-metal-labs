#define F_CPU 16000000UL
#include <stdint.h>
#include <util/delay.h>

#define DDRB  (*(volatile uint8_t *)0x24)
#define PORTB (*(volatile uint8_t *)0x25)
#define DDRD  (*(volatile uint8_t *)0x2A)
#define PORTD (*(volatile uint8_t *)0x2B)
#define PINB  (*(volatile uint8_t *)0x23)

int main(void){
	DDRD  |= (1 << 7);  // PD7 output LED 
	DDRB  |= (1 << 0);  // PB0 output trigger -> 0
	DDRB  &= ~(1 << 1); // PB1 input (Echo)

	while (1) {	
		PORTB |= (1 << 0);
		_delay_us(10);
		PORTB &= ~(1 << 0);
		
		for (uint16_t i = 0; i < 600; i++){
			uint8_t echo = (PINB >> 1) & 1;
			if (echo == 1) {
				PORTD |= (1 << 7); 
			}
			else {
				PORTD &= ~(1 << 7);
			}
			_delay_us(100);
		}
	}
}
