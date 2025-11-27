#include "timer.h"
#include "common.h"
#include "logger.h"
#include "sched.h"

#define TIMER_INTERVAL_CYCLES 100000U /* ~10 ms @ 10 MHz */

static volatile uint64_t timer_tick_count = 0;

static uint64_t clint_read_mtime(void) {
    volatile uint32_t *mtime = (uint32_t *)CLINT_MTIME;
    uint32_t hi = mtime[1];
    uint32_t lo = mtime[0];
    uint32_t hi_check = mtime[1];
    while (hi != hi_check) {
        hi = hi_check;
        lo = mtime[0];
        hi_check = mtime[1];
    }
    return ((uint64_t)hi << 32) | lo;
}

static void clint_write_mtimecmp(uint64_t value) {
    volatile uint32_t *mtcmp = (uint32_t *)CLINT_MTIMECMP(0);
    mtcmp[1] = 0xFFFFFFFF;
    mtcmp[0] = (uint32_t)value;
    mtcmp[1] = (uint32_t)(value >> 32);
}

static void program_next_tick(void) {
    uint64_t next = clint_read_mtime() + TIMER_INTERVAL_CYCLES;
    clint_write_mtimecmp(next);
}

void timer_init(void) {
    timer_tick_count = 0;
    program_next_tick();
    CSR_SET(mie, MIE_MTIE);
}

uint64_t timer_ticks(void) {
    return timer_tick_count;
}

void timer_handle_tick(trap_frame_t *frame) {
    timer_tick_count++;
    logger_log_u32("TIMER", "Tick", (uint32_t)timer_tick_count);
    program_next_tick();
    sched_tick(frame);
}
