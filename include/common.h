// SPDX-License-Identifier: MIT
#pragma once

#include <stdint.h>
#include <stddef.h>

/* QEMU virt machine memory map */
#define RAM_BASE        0x80000000UL
#define RAM_SIZE        (128UL * 1024UL * 1024UL)
#define CLINT_BASE      0x02000000UL
#define PLIC_BASE       0x0C000000UL
#define UART0_BASE      0x10000000UL

#define CLINT_MTIMECMP(hart) (CLINT_BASE + 0x4000 + 8 * (hart))
#define CLINT_MTIME        (CLINT_BASE + 0xBFF8)

#define PAGE_SIZE       4096UL

#define CSR_WRITE(csr, val) __asm__ volatile("csrw " #csr ", %0" :: "r"(val))
#define CSR_READ(csr) ({ uintptr_t __v; __asm__ volatile("csrr %0, " #csr : "=r"(__v)); __v; })
#define CSR_SET(csr, val) __asm__ volatile("csrs " #csr ", %0" :: "r"(val))
#define CSR_CLEAR(csr, val) __asm__ volatile("csrc " #csr ", %0" :: "r"(val))

#define MSTATUS_MIE     (1U << 3)
#define MSTATUS_MPIE    (1U << 7)
#define MSTATUS_MPP_M   (3U << 11)

#define MIE_MSIE        (1U << 3)
#define MIE_MTIE        (1U << 7)
#define MIE_MEIE        (1U << 11)

static inline void mmio_write32(uintptr_t addr, uint32_t value) {
    *(volatile uint32_t *)addr = value;
}

static inline uint32_t mmio_read32(uintptr_t addr) {
    return *(volatile uint32_t *)addr;
}

static inline void mmio_write8(uintptr_t addr, uint8_t value) {
    *(volatile uint8_t *)addr = value;
}

static inline uint8_t mmio_read8(uintptr_t addr) {
    return *(volatile uint8_t *)addr;
}

static inline void mmio_write64(uintptr_t addr, uint64_t value) {
    *(volatile uint64_t *)addr = value;
}

static inline uint64_t mmio_read64(uintptr_t addr) {
    return *(volatile uint64_t *)addr;
}

static inline void mem_zero(void *ptr, size_t len) {
    uint8_t *p = (uint8_t *)ptr;
    for (size_t i = 0; i < len; ++i) {
        p[i] = 0;
    }
}

static inline void mem_copy(void *dst, const void *src, size_t len) {
    uint8_t *d = (uint8_t *)dst;
    const uint8_t *s = (const uint8_t *)src;
    for (size_t i = 0; i < len; ++i) {
        d[i] = s[i];
    }
}
