#include "logger.h"
#include "common.h"
#include "timer.h"
#include "sync.h"

#define UART_THR (UART0_BASE + 0x00)
#define UART_LSR (UART0_BASE + 0x05)
#define UART_LSR_TX_IDLE (1U << 5)

static spinlock_t log_lock;

static int logger_try_lock(void) {
    uintptr_t status = CSR_READ(mstatus);
    if (status & MSTATUS_MIE) {
        spinlock_acquire(&log_lock);
        return 1;
    }
    return spinlock_try_acquire(&log_lock);
}

static void logger_unlock(int held) {
    if (held) {
        spinlock_release(&log_lock);
    }
}

static void uart_putc(char c) {
    while ((mmio_read8(UART_LSR) & UART_LSR_TX_IDLE) == 0) {
        __asm__ volatile("nop");
    }
    mmio_write8(UART_THR, (uint8_t)c);
}

static void uart_write_str(const char *s) {
    while (*s) {
        if (*s == '\n') {
            uart_putc('\r');
        }
        uart_putc(*s++);
    }
}

static void uart_write_u64(uint64_t value) {
    char buf[24];
    size_t idx = 0;
    if (value == 0) {
        uart_putc('0');
        return;
    }
    while (value && idx < sizeof(buf)) {
        buf[idx++] = '0' + (value % 10);
        value /= 10;
    }
    while (idx > 0) {
        uart_putc(buf[--idx]);
    }
}

static void uart_write_u32(uint32_t value) {
    uart_write_u64(value);
}

static void logger_prefix(const char *tag) {
    uart_putc('[');
    uart_write_u64(timer_ticks());
    uart_putc(']');
    uart_putc('[');
    uart_write_str(tag);
    uart_putc(']');
    uart_putc(' ');
}

void logger_init(void) {
    spinlock_init(&log_lock);
}

void logger_log(const char *tag, const char *msg) {
    int locked = logger_try_lock();
    logger_prefix(tag);
    uart_write_str(msg);
    uart_putc('\n');
    logger_unlock(locked);
}

void logger_log_u32(const char *tag, const char *prefix, uint32_t value) {
    int locked = logger_try_lock();
    logger_prefix(tag);
    uart_write_str(prefix);
    uart_putc(' ');
    uart_write_u32(value);
    uart_putc('\n');
    logger_unlock(locked);
}
