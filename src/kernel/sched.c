#include "sched.h"
#include "thread.h"
#include "logger.h"

#define SYSCALL_YIELD 0U

static int scheduler_active = 0;

static void perform_switch(trap_frame_t *frame, thread_t *next) {
    if (!next) {
        return;
    }

    thread_t *current = thread_current();

    if (current && frame) {
        thread_save_context(current, frame);
        if (current->state == THREAD_RUNNING) {
            current->state = THREAD_READY;
        }
    }

    if (frame) {
        thread_load_context(next, frame);
    }

    next->state = THREAD_RUNNING;
    thread_set_current(next);
    logger_log_u32("SCHED", "Switching to thread", thread_id(next));
}

void sched_init(void) {
    thread_set_current(NULL);
    scheduler_active = 0;
}

void sched_start(void) {
    thread_t *next = thread_next_ready(NULL);
    if (!next) {
        logger_log("SCHED", "No runnable threads");
        return;
    }
    scheduler_active = 1;
    thread_set_current(next);
    next->state = THREAD_RUNNING;
    logger_log_u32("SCHED", "Switching to thread", thread_id(next));
    thread_launch(&next->context);
}

void sched_tick(trap_frame_t *frame) {
    if (!scheduler_active) {
        return;
    }

    thread_t *current = thread_current();
    thread_t *candidate = thread_next_ready(current);
    if (!candidate || candidate == current) {
        return;
    }

    perform_switch(frame, candidate);
}

void sched_handle_syscall(trap_frame_t *frame) {
    if (!scheduler_active) {
        frame->mepc += 4;
        return;
    }

    uint32_t syscall_id = frame->x[REG_A7];
    frame->mepc += 4;

    switch (syscall_id) {
    case SYSCALL_YIELD:
        sched_tick(frame);
        break;
    default:
        logger_log_u32("SYSCALL", "Unknown ID", syscall_id);
        break;
    }
}

void sched_yield(void) {
    __asm__ volatile("li a7, %0\n\tecall" :: "i"(SYSCALL_YIELD) : "memory");
}

uint32_t sched_current_thread_id(void) {
    thread_t *current = thread_current();
    if (!current) {
        return 0xFFFFFFFFu;
    }
    return thread_id(current);
}
