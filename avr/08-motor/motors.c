#define F_CPU 16000000UL
#include <stdint.h>
#include <util/delay.h>

/* ---- Port D: every motor control pin lives here (D2–D7) ----
   D0/D1 are left alone because they're the USB serial (UART) pins. */
#define DDRD   (*(volatile uint8_t *)0x2A)   // direction: 1 = output
#define PORTD  (*(volatile uint8_t *)0x2B)   // output level

/* ---- Timer0: hardware PWM for motor speed ---- */
#define TCCR0A (*(volatile uint8_t *)0x44)   // mode bits + connect pins to timer
#define TCCR0B (*(volatile uint8_t *)0x45)   // prescaler
#define OCR0A  (*(volatile uint8_t *)0x47)   // duty for PD6 (D6) -> PWMA
#define OCR0B  (*(volatile uint8_t *)0x48)   // duty for PD5 (D5) -> PWMB

/* ---- Bit names, from the datasheet's register diagrams ---- */
#define WGM00  0    // TCCR0A
#define WGM01  1    // TCCR0A
#define COM0B1 5    // TCCR0A
#define COM0A1 7    // TCCR0A
#define CS00   0    // TCCR0B
#define CS01   1    // TCCR0B

/* ---- Which Port D bit goes to which TB6612 pin ---- */
#define PIN_BIN2 2  // D2
#define PIN_BIN1 3  // D3
#define PIN_AIN2 4  // D4
#define PIN_PWMB 5  // D5  (OC0B, Timer0 output)
#define PIN_PWMA 6  // D6  (OC0A, Timer0 output)
#define PIN_AIN1 7  // D7


void setup(void) {
    /* All six control pins are outputs */
    DDRD |= (1 << PIN_AIN1) | (1 << PIN_AIN2)
          | (1 << PIN_BIN1) | (1 << PIN_BIN2)
          | (1 << PIN_PWMA) | (1 << PIN_PWMB);

    /* Start stopped: IN1 = IN2 = LOW means "coast" in the TB6612 truth table */
    PORTD &= ~((1 << PIN_AIN1) | (1 << PIN_AIN2)
             | (1 << PIN_BIN1) | (1 << PIN_BIN2));
    OCR0A = 0;
    OCR0B = 0;

    /* Timer0: Fast PWM, counts 0..255 then wraps (WGM01 + WGM00).
       COM0A1 / COM0B1 hand D6 and D5 over to the timer hardware:
       pin goes HIGH at 0, LOW when the count reaches OCR0x.
       So OCR0x = 0..255 sets the duty cycle = the motor speed. */
    TCCR0A = (1 << COM0A1) | (1 << COM0B1) | (1 << WGM01) | (1 << WGM00);

    /* Prescaler /64: 16,000,000 / 64 / 256 = 976 Hz PWM.
       Same frequency Arduino's analogWrite uses; fine for small DC motors. */
    TCCR0B = (1 << CS01) | (1 << CS00);
}

/* One TB6612 channel. speed: -255 (full reverse) .. 0 (stop) .. +255 (full forward)

   TB6612 truth table (STBY = HIGH):
     IN1  IN2   result
      H    L    forward  at PWM duty
      L    H    reverse  at PWM duty
      L    L    stop (coast)
      H    H    short brake                                               */
void motor_set(uint8_t in1, uint8_t in2, volatile uint8_t *ocr, int16_t speed) {
    if (speed >  255) speed =  255;
    if (speed < -255) speed = -255;

    if (speed > 0) {
        PORTD |=  (1 << in1);
        PORTD &= ~(1 << in2);
        *ocr = (uint8_t)speed;
    } else if (speed < 0) {
        PORTD &= ~(1 << in1);
        PORTD |=  (1 << in2);
        *ocr = (uint8_t)(-speed);
    } else {
        PORTD &= ~((1 << in1) | (1 << in2));
        *ocr = 0;
    }
}

/* Channel A = both left motors, channel B = both right motors */
void drive(int16_t left, int16_t right) {
    motor_set(PIN_AIN1, PIN_AIN2, &OCR0A, left);
    motor_set(PIN_BIN1, PIN_BIN2, &OCR0B, right);
}


int main(void) {
    setup();
    _delay_ms(2000);              // 2 s to put it down / pick it up after reset

    while (1) {
        drive( 150,  150);  _delay_ms(1500);   // forward, ~60% speed
        drive(   0,    0);  _delay_ms(1000);   // stop
        drive(-150, -150);  _delay_ms(1500);   // reverse
        drive(   0,    0);  _delay_ms(1000);
        drive(-150,  150);  _delay_ms(800);    // spin left on the spot
        drive(   0,    0);  _delay_ms(1000);
        drive( 150, -150);  _delay_ms(800);    // spin right on the spot
        drive(   0,    0);  _delay_ms(2000);
    }
}
