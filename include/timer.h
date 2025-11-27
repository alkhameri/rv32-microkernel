// SPDX-License-Identifier: MIT
#pragma once

#include <stdint.h>
#include "trap.h"

void timer_init(void);
void timer_handle_tick(trap_frame_t *frame);
uint64_t timer_ticks(void);
