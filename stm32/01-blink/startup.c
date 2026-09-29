#include <stdint.h>

// Defined by linker.ld, not by any C file. Only their addresses matter.
extern uint32_t _estack;
extern uint32_t _sidata;
extern uint32_t _sdata;
extern uint32_t _edata;
extern uint32_t _sbss;
extern uint32_t _ebss;

int  main(void);
void reset_handler(void);

__attribute__((section(".isr_vector"), used))
const uint32_t vector_table[] = {
    (uint32_t)&_estack,        // slot 0: initial stack pointer
    (uint32_t)reset_handler,   // slot 1: first code the CPU runs
};

void reset_handler(void) {
    // 1. copy .data (globals with initial values) from flash into RAM
    uint32_t *src = &_sidata;
    uint32_t *dst = &_sdata;
    while (dst < &_edata) {
        *dst++ = *src++;
    }

    // 2. zero .bss (globals that start at zero)
    dst = &_sbss;
    while (dst < &_ebss) {
        *dst++ = 0;
    }

    // 3. hand over to the program
    main();

    while (1) { }              // main should never return
}