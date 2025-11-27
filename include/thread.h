// SPDX-License-Identifier: MIT
#pragma once

#include <stdint.h>
#include "trap.h"

#define MAX_THREADS         4
#define THREAD_STACK_SIZE   2048

typedef enum {
    THREAD_UNUSED = 0,
    THREAD_READY,
    THREAD_RUNNING,
    THREAD_BLOCKED,
} thread_state_t;

typedef struct thread {
    uint32_t id;
    const char *name;
    thread_state_t state;
    trap_frame_t context;
    uint8_t stack[THREAD_STACK_SIZE];
} thread_t;

void thread_system_init(void);
thread_t *thread_create(const char *name, void (*entry)(void *), void *arg);
thread_t *thread_next_ready(thread_t *current);
thread_t *thread_current(void);
void thread_set_current(thread_t *thread);
void thread_mark_ready(thread_t *thread);
void thread_mark_blocked(thread_t *thread);
void thread_bootstrap(void);
void thread_save_context(thread_t *thread, const trap_frame_t *frame);
void thread_load_context(const thread_t *thread, trap_frame_t *frame);
void thread_launch(const trap_frame_t *frame);
uint32_t thread_id(const thread_t *thread);
const char *thread_name(const thread_t *thread);
