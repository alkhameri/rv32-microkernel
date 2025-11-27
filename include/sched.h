// SPDX-License-Identifier: MIT
#pragma once

#include <stdint.h>
#include "trap.h"

void sched_init(void);
void sched_start(void);
void sched_tick(trap_frame_t *frame);
void sched_handle_syscall(trap_frame_t *frame);
void sched_yield(void);
uint32_t sched_current_thread_id(void);
