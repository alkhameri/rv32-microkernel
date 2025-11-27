// SPDX-License-Identifier: MIT
#pragma once

#include <stdint.h>

void logger_init(void);
void logger_log(const char *tag, const char *msg);
void logger_log_u32(const char *tag, const char *prefix, uint32_t value);
