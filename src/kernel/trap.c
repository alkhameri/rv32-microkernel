#include "trap.h"
#include "timer.h"
#include "logger.h"
#include "sched.h"
#include "common.h"

void trap_init(void) {
    /* Trap vector set in start.S; nothing more for now. */
}

void trap_handle(trap_frame_t *frame) {
    uint32_t mcause = frame->mcause;
    uint32_t code = mcause & 0x7FFFFFFF;
    uint32_t is_interrupt = mcause >> 31;

    if (is_interrupt && code == 7) {
        timer_handle_tick(frame);
        return;
    }

    if (!is_interrupt && code == 11) {
        sched_handle_syscall(frame);
        return;
    }

    logger_log_u32("TRAP", "Unhandled cause", mcause);
    logger_log_u32("TRAP", "mepc", frame->mepc);
    while (1) {
        __asm__ volatile("wfi");
    }
}
