#include "kernel.h"
#include "logger.h"
#include "pmm.h"
#include "vm.h"
#include "interrupt.h"
#include "timer.h"
#include "trap.h"
#include "thread.h"
#include "sched.h"
#include "sync.h"

void kernel_main(void) {
    logger_init();
    logger_log("BOOT", "Kernel starting...");

    sync_init();
    pmm_init();
    vm_init();
    interrupt_init();
    thread_system_init();
    thread_bootstrap();
    sched_init();
    timer_init();
    trap_init();

    logger_log("BOOT", "Starting scheduler");
    sched_start();

    while (1) {
        __asm__ volatile("wfi");
    }
}
