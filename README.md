# RV32 Educational Microkernel

This project is a minimal but functional RISC-V RV32 microkernel that boots in machine mode on QEMU’s `virt` platform. It demonstrates startup, trap handling, timer interrupts, a preemptive round-robin scheduler, kernel threads with dedicated stacks, Sv32 virtual memory, a bump-based physical memory allocator, synchronization primitives, and a timestamped UART logger. A tiny syscall interface (`ecall`-based yield) and semaphore-driven demo threads showcase cooperative and preemptive scheduling.

## Features

- **Machine-mode boot path**: hand-written entry/trap assembly (`src/boot/start.S`) installs the trap vector, zeroes `.bss`, and jumps into C.
- **Trap + interrupt stack**: full register save/restore with `trap_frame_t`, timer interrupt dispatch, and syscall handling.
- **CLINT timer driver**: 10 ms tick programming plus tick accounting and log timestamps.
- **Preemptive scheduler**: round-robin context switching driven by timer IRQs or voluntary `sched_yield()` syscalls.
- **Kernel threads**: dedicated stacks, automatic GP setup, and sample producer/consumer threads.
- **Memory management**: bump-pointer physical allocator and Sv32 identity map enabling `satp`.
- **Sync primitives**: ticket-style spinlock and blocking semaphore (implemented via scheduler yield).
- **Logger**: timestamped UART output guarded by a spinlock.

Stretch goals started:

- Basic syscall path (ECALL-based yield) for future expansion.

## Prerequisites

- `riscv32-unknown-elf-gcc` cross toolchain (bare-metal, no glibc)
- `qemu-system-riscv32`
- `make`

Ensure the cross tools are available on your `PATH`.

## Build

```sh
make
```

Outputs `kernel.elf`.

## Run

```sh
make run
```

Which executes:

```sh
qemu-system-riscv32 -machine virt -kernel kernel.elf -nographic
```

### Sample Output

```text
[0][BOOT] Kernel starting...
[0][BOOT] Starting scheduler
[0][SCHED] Switching to thread 1
[0][WORK-A] Produced token 0
[1][TIMER] Tick 1
[1][SCHED] Switching to thread 2
[1][WORK-B] Semaphore acquired
[2][TIMER] Tick 2
[2][WORK-A] Produced token 1
...
```

Timestamps correspond to CLINT ticks. `[WORK-A]` and `[WORK-B]` are the demo producer/consumer kernel threads synchronized by a semaphore. `[SCHED]` lines show timer-driven preemption.

## Architecture Overview

- **Boot + traps**: `_start` configures the stack, clears `.bss`, installs the trap vector, and calls `kernel_main`. The trap vector in `start.S` saves all GPRs/CSRs, calls `trap_handle`, then restores state and executes `mret`.
- **Timer + interrupts**: `timer_init()` programs `mtimecmp` for a 10 ms cadence and enables MTIE. `timer_handle_tick()` increments the global tick counter, logs `[TIMER] Tick N`, reprograms CLINT, and invokes the scheduler.
- **Scheduler + threads**: `sched.c` maintains the current thread, services timer/ECALL events, and swaps trap frames between thread contexts. `thread.c` provisions stacks, initializes register frames (including `gp`), and spawns demo threads.
- **Memory**: `pmm.c` exposes a page-sized bump allocator rooted at `__heap_start`. `vm.c` allocates a root Sv32 page table, identity maps the first 16 MiB of RAM with RWX permissions, and installs the table via `satp`.
- **Sync**: `sync.c` provides spinlocks (using GCC atomics) and a semaphore that blocks by yielding to the scheduler until a token becomes available. The logger and demo threads both use these primitives.
- **Syscalls**: `sched_yield()` issues an `ecall` with ID `0`, letting the trap handler reuse the same scheduling path outside interrupts—laying groundwork for future syscall expansion.

## Future Enhancements

- Flesh out syscall catalog (IPC, timers, user-mode services).
- Add a simple ELF loader and user-mode task support.
- Implement a richer physical allocator (free lists or buddy).
- Add SMP boot stubs for multiple harts and PLIC routing.
- Explore a lightweight buffer/page cache abstraction.

## Repository Layout Recap

```text
src/
  boot/start.S           # Entry + trap vector + context launch helper
  kernel/*.c             # Core kernel subsystems
include/*.h              # Shared interfaces
linker.ld                # Memory layout + symbols for allocators
Makefile                 # Build/run/clean
```

Feel free to extend any subsystem—each module is documented and compartmentalized for experimentation. Have fun hacking on RV32!
