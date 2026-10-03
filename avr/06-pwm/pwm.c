#define F_CPU 16000000UL
#include <stdint.h>
#include <util/delay.h>

#define TCCR1A (*(volatile uint8_t  *)0x80)
#define TCCR1B (*(volatile uint8_t  *)0x81)
#define ICR1   (*(volatile uint16_t *)0x86)
#define OCR1B  (*(volatile uint16_t *)0x8A)
#define DDRB   (*(volatile uint8_t *) 0x24)

#define SERVIO_MIN 950   // ticks at 0
#define SERVIO_MAX 5050   // ticks at 180

void pwm_init(void) {
    DDRB |= (1 << 2); // PB2 (D10 on the board) is output
    TCCR1A = (1 << 5) | (1 << 1); // COM1B1: drive PB2; WGM11
    TCCR1B = (1 << 4) | (1 << 3) | (1 << 1);     // WGM13, WGM12: Fast PWM with top = ICR1; CS10: prescaler /8
    ICR1   = 39999;                              // period
}

void set_angle (uint8_t degrees) { // 0 to 180
    OCR1B = SERVIO_MIN + ((uint32_t)degrees * (SERVIO_MAX - SERVIO_MIN))/180;   // ~0.6 ms to 2.4 ms
}

int main(void) {
    pwm_init();
    while (1) { 
        for (uint8_t a = 45; a < 180; a += 1) {
            set_angle(a);
            _delay_ms(15);
        }
        for (uint8_t a = 180; a > 45; a -= 1) {
            set_angle(a);
            _delay_ms(15);
        }
    }
}