#include "pmm.h"
#include "common.h"

extern uint8_t __heap_start[];
extern uint8_t __heap_end[];

static uintptr_t free_ptr;
static uintptr_t free_end;

void pmm_init(void) {
    free_ptr = ((uintptr_t)__heap_start + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);
    free_end = (uintptr_t)__heap_end;
}

void *pmm_alloc_page(void) {
    if (free_ptr + PAGE_SIZE > free_end) {
        return NULL;
    }
    void *page = (void *)free_ptr;
    free_ptr += PAGE_SIZE;
    mem_zero(page, PAGE_SIZE);
    return page;
}

void pmm_free_page(void *page) {
    (void)page;
    /* Simple bump allocator; no free list yet. */
}
