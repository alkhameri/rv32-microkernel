#include "interrupt.h"
#include "common.h"

void interrupt_init(void) {
    /* Mask external interrupts for now; timer uses CLINT */
    CSR_CLEAR(mie, MIE_MEIE | MIE_MSIE);
}
