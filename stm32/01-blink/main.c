#include <stdint.h>

// STM32F446RE registers. Note these are 32-bit, unlike the AVR's 8-bit ones.
#define RCC_AHB1ENR (*(volatile uint32_t *)0x40023830)   // peripheral clock switches
#define GPIOA_MODER (*(volatile uint32_t *)0x40020000)   // pin direction/mode
#define GPIOA_ODR   (*(volatile uint32_t *)0x40020014)   // output data

// volatile stops the compiler deleting this loop for doing nothing
static void delay(volatile uint32_t n) {
    while (n--) { }
}

int main(void) {
    RCC_AHB1ENR |= (1U << 0);        // GPIOAEN: switch port A's clock on
    GPIOA_MODER &= ~(3U << 10);      // clear PA5's two mode bits
    GPIOA_MODER |=  (1U << 10);      // set them to 01 = output

    while (1) {
        GPIOA_ODR ^= (1U << 5);      // toggle PA5, the green LD2 on the board
        delay(1000000);
    }
}