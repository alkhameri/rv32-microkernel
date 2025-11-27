// SPDX-License-Identifier: MIT
#pragma once

#include <stdint.h>

typedef struct {
    volatile uint32_t value;
} spinlock_t;

typedef struct {
    int32_t count;
    spinlock_t lock;
} semaphore_t;

void sync_init(void);
void spinlock_init(spinlock_t *lock);
void spinlock_acquire(spinlock_t *lock);
void spinlock_release(spinlock_t *lock);
int spinlock_try_acquire(spinlock_t *lock);

void semaphore_init(semaphore_t *sem, int32_t initial);
void semaphore_signal(semaphore_t *sem);
void semaphore_wait(semaphore_t *sem);
