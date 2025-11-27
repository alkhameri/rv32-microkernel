#include "thread.h"
#include "common.h"
#include "logger.h"
#include "sync.h"
#include "sched.h"

extern uint8_t __global_pointer$[];

static thread_t thread_table[MAX_THREADS];
static uint32_t next_thread_id = 1;
static thread_t *current_thread_ptr = NULL;
static semaphore_t demo_sem;

static void demo_producer(void *arg);
static void demo_consumer(void *arg);

void thread_system_init(void) {
    for (uint32_t i = 0; i < MAX_THREADS; ++i) {
        thread_table[i].id = 0;
        thread_table[i].name = NULL;
        thread_table[i].state = THREAD_UNUSED;
        mem_zero(&thread_table[i].context, sizeof(trap_frame_t));
        mem_zero(thread_table[i].stack, THREAD_STACK_SIZE);
    }
    next_thread_id = 1;
    current_thread_ptr = NULL;
}

static uintptr_t thread_stack_top(thread_t *thread) {
    uintptr_t top = (uintptr_t)(thread->stack + THREAD_STACK_SIZE);
    return top & ~0xFUL;
}

thread_t *thread_create(const char *name, void (*entry)(void *), void *arg) {
    for (uint32_t i = 0; i < MAX_THREADS; ++i) {
        if (thread_table[i].state == THREAD_UNUSED) {
            thread_t *thread = &thread_table[i];
            thread->id = next_thread_id++;
            thread->name = name;
            thread->state = THREAD_READY;
            mem_zero(&thread->context, sizeof(trap_frame_t));

            thread->context.x[REG_SP] = thread_stack_top(thread);
            thread->context.x[REG_RA] = 0;
            thread->context.x[REG_A0] = (uintptr_t)arg;
            thread->context.x[REG_GP] = (uintptr_t)__global_pointer$;
            thread->context.mepc = (uintptr_t)entry;
            thread->context.mstatus = MSTATUS_MPP_M | MSTATUS_MPIE;
            return thread;
        }
    }

    logger_log("THREAD", "No free TCB slots");
    return NULL;
}

thread_t *thread_next_ready(thread_t *current) {
    uint32_t start = 0;
    if (current) {
        start = (uint32_t)(current - thread_table) + 1;
    }

    for (uint32_t i = 0; i < MAX_THREADS; ++i) {
        uint32_t idx = (start + i) % MAX_THREADS;
        if (thread_table[idx].state == THREAD_READY) {
            return &thread_table[idx];
        }
    }

    if (current && current->state == THREAD_RUNNING) {
        return current;
    }

    return NULL;
}

thread_t *thread_current(void) {
    return current_thread_ptr;
}

void thread_set_current(thread_t *thread) {
    current_thread_ptr = thread;
}

void thread_mark_ready(thread_t *thread) {
    if (thread) {
        thread->state = THREAD_READY;
    }
}

void thread_mark_blocked(thread_t *thread) {
    if (thread) {
        thread->state = THREAD_BLOCKED;
    }
}

void thread_save_context(thread_t *thread, const trap_frame_t *frame) {
    if (!thread || !frame) {
        return;
    }
    mem_copy(&thread->context, frame, sizeof(trap_frame_t));
}

void thread_load_context(const thread_t *thread, trap_frame_t *frame) {
    if (!thread || !frame) {
        return;
    }
    mem_copy(frame, &thread->context, sizeof(trap_frame_t));
}

uint32_t thread_id(const thread_t *thread) {
    return thread ? thread->id : 0;
}

const char *thread_name(const thread_t *thread) {
    return thread && thread->name ? thread->name : "<none>";
}

static void demo_producer(void *arg) {
    semaphore_t *sem = (semaphore_t *)arg;
    uint32_t counter = 0;
    while (1) {
        logger_log_u32("WORK-A", "Produced token", counter++);
        semaphore_signal(sem);
        for (volatile uint32_t i = 0; i < 50000; ++i) {
            __asm__ volatile("nop");
        }
    }
}

static void demo_consumer(void *arg) {
    semaphore_t *sem = (semaphore_t *)arg;
    while (1) {
        semaphore_wait(sem);
        logger_log("WORK-B", "Semaphore acquired");
    }
}

void thread_bootstrap(void) {
    semaphore_init(&demo_sem, 0);
    thread_create("producer", demo_producer, &demo_sem);
    thread_create("consumer", demo_consumer, &demo_sem);
}
