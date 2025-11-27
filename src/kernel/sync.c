#include "sync.h"
#include "sched.h"

void sync_init(void) {
    /* Nothing global yet. */
}

void spinlock_init(spinlock_t *lock) {
    if (lock) {
        lock->value = 0;
    }
}

void spinlock_acquire(spinlock_t *lock) {
    if (!lock) {
        return;
    }
    while (__atomic_test_and_set(&lock->value, __ATOMIC_ACQUIRE)) {
        __asm__ volatile("nop");
    }
}

void spinlock_release(spinlock_t *lock) {
    if (!lock) {
        return;
    }
    __atomic_clear(&lock->value, __ATOMIC_RELEASE);
}

int spinlock_try_acquire(spinlock_t *lock) {
    if (!lock) {
        return 0;
    }
    uint32_t expected = 0;
    return __atomic_compare_exchange_n(&lock->value, &expected, 1, 0, __ATOMIC_ACQUIRE, __ATOMIC_RELAXED);
}

void semaphore_init(semaphore_t *sem, int32_t initial) {
    if (!sem) {
        return;
    }
    spinlock_init(&sem->lock);
    sem->count = initial;
}

void semaphore_signal(semaphore_t *sem) {
    if (!sem) {
        return;
    }
    spinlock_acquire(&sem->lock);
    sem->count++;
    spinlock_release(&sem->lock);
}

void semaphore_wait(semaphore_t *sem) {
    if (!sem) {
        return;
    }
    while (1) {
        spinlock_acquire(&sem->lock);
        if (sem->count > 0) {
            sem->count--;
            spinlock_release(&sem->lock);
            return;
        }
        spinlock_release(&sem->lock);
        sched_yield();
    }
}
