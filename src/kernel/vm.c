#include "vm.h"
#include "pmm.h"
#include "common.h"
#include "logger.h"

#define PTE_V  (1U << 0)
#define PTE_R  (1U << 1)
#define PTE_W  (1U << 2)
#define PTE_X  (1U << 3)
#define PTE_U  (1U << 4)
#define PTE_G  (1U << 5)
#define PTE_A  (1U << 6)
#define PTE_D  (1U << 7)

static uint32_t *kernel_page_table = NULL;

static uint32_t *alloc_table(void) {
    return (uint32_t *)pmm_alloc_page();
}

static uint32_t *walk_table(uint32_t *root, uintptr_t va, int level, int create) {
    uint32_t idx = (va >> (12 + level * 10)) & 0x3FF;
    uint32_t entry = root[idx];
    if (!(entry & PTE_V)) {
        if (!create) {
            return NULL;
        }
        uint32_t *child = alloc_table();
        if (!child) {
            return NULL;
        }
        mem_zero(child, PAGE_SIZE);
        root[idx] = (((uintptr_t)child >> 12) << 10) | PTE_V;
        entry = root[idx];
    }
    return (uint32_t *)(((entry >> 10) << 12));
}

static void vm_map_page(uintptr_t va, uintptr_t pa, uint32_t flags) {
    uint32_t *l1 = walk_table(kernel_page_table, va, 1, 1);
    if (!l1) {
        return;
    }
    uint32_t *l0 = walk_table(l1, va, 0, 1);
    if (!l0) {
        return;
    }
    uint32_t idx0 = (va >> 12) & 0x3FF;
    l0[idx0] = ((pa >> 12) << 10) | flags | PTE_V | PTE_A | PTE_D;
}

void vm_map_identity(uintptr_t start, uintptr_t end) {
    if (!kernel_page_table) {
        return;
    }
    uintptr_t va = start & ~(PAGE_SIZE - 1);
    for (; va < end; va += PAGE_SIZE) {
        vm_map_page(va, va, PTE_R | PTE_W | PTE_X | PTE_G);
    }
}

void vm_init(void) {
    kernel_page_table = alloc_table();
    if (!kernel_page_table) {
        logger_log("VM", "Failed to allocate root page table");
        return;
    }
    mem_zero(kernel_page_table, PAGE_SIZE);

    vm_map_identity(RAM_BASE, RAM_BASE + (16UL * 1024UL * 1024UL));

    uintptr_t satp_val = (1UL << 31) | (((uintptr_t)kernel_page_table) >> 12);
    CSR_WRITE(satp, satp_val);
    __asm__ volatile("sfence.vma zero, zero");
}
