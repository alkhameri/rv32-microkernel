// SPDX-License-Identifier: MIT
#pragma once

#include <stddef.h>

void pmm_init(void);
void *pmm_alloc_page(void);
void pmm_free_page(void *page);
