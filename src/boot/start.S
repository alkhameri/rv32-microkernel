    .section .text.init
    .globl _start
    .globl trap_vector
    .globl thread_launch

    .equ    TF_x0,      0
    .equ    TF_x1,      4
    .equ    TF_x2,      8
    .equ    TF_x3,      12
    .equ    TF_x4,      16
    .equ    TF_x5,      20
    .equ    TF_x6,      24
    .equ    TF_x7,      28
    .equ    TF_x8,      32
    .equ    TF_x9,      36
    .equ    TF_x10,     40
    .equ    TF_x11,     44
    .equ    TF_x12,     48
    .equ    TF_x13,     52
    .equ    TF_x14,     56
    .equ    TF_x15,     60
    .equ    TF_x16,     64
    .equ    TF_x17,     68
    .equ    TF_x18,     72
    .equ    TF_x19,     76
    .equ    TF_x20,     80
    .equ    TF_x21,     84
    .equ    TF_x22,     88
    .equ    TF_x23,     92
    .equ    TF_x24,     96
    .equ    TF_x25,     100
    .equ    TF_x26,     104
    .equ    TF_x27,     108
    .equ    TF_x28,     112
    .equ    TF_x29,     116
    .equ    TF_x30,     120
    .equ    TF_x31,     124
    .equ    TF_mepc,    128
    .equ    TF_mstatus, 132
    .equ    TF_mcause,  136
    .equ    TF_mtval,   140
    .equ    TF_SIZE,    144

/*
 * Minimal M-mode entry point.
 * Sets up stack, clears .bss, installs trap vector, jumps to C entry.
 */
_start:
    la      sp, _stack_top

    la      t0, __bss_start
    la      t1, __bss_end
1:
    bgeu    t0, t1, 2f
    sw      zero, 0(t0)
    addi    t0, t0, 4
    j       1b
2:
    la      t0, trap_vector
    csrw    mtvec, t0

    call    kernel_main

.Lhalt:
    wfi
    j       .Lhalt

    .section .text.trap
    .align 2
    .globl trap_vector
    .type trap_vector, @function
trap_vector:
    addi    sp, sp, -TF_SIZE

    /* Save caller registers */
    sw      zero,  TF_x0(sp)
    sw      ra,    TF_x1(sp)
    sw      gp,    TF_x3(sp)
    sw      tp,    TF_x4(sp)
    sw      t0,    TF_x5(sp)
    sw      t1,    TF_x6(sp)
    sw      t2,    TF_x7(sp)
    sw      s0,    TF_x8(sp)
    sw      s1,    TF_x9(sp)
    sw      a0,    TF_x10(sp)
    sw      a1,    TF_x11(sp)
    sw      a2,    TF_x12(sp)
    sw      a3,    TF_x13(sp)
    sw      a4,    TF_x14(sp)
    sw      a5,    TF_x15(sp)
    sw      a6,    TF_x16(sp)
    sw      a7,    TF_x17(sp)
    sw      s2,    TF_x18(sp)
    sw      s3,    TF_x19(sp)
    sw      s4,    TF_x20(sp)
    sw      s5,    TF_x21(sp)
    sw      s6,    TF_x22(sp)
    sw      s7,    TF_x23(sp)
    sw      s8,    TF_x24(sp)
    sw      s9,    TF_x25(sp)
    sw      s10,   TF_x26(sp)
    sw      s11,   TF_x27(sp)
    sw      t3,    TF_x28(sp)
    sw      t4,    TF_x29(sp)
    sw      t5,    TF_x30(sp)
    sw      t6,    TF_x31(sp)

    /* Save stack pointer (original value) after other registers */
    addi    t0, sp, TF_SIZE
    sw      t0,    TF_x2(sp)

    csrr    t0, mepc
    sw      t0, TF_mepc(sp)
    csrr    t0, mstatus
    sw      t0, TF_mstatus(sp)
    csrr    t0, mcause
    sw      t0, TF_mcause(sp)
    csrr    t0, mtval
    sw      t0, TF_mtval(sp)

    mv      a0, sp
    call    trap_handle

    /* Restore CSR state (allow handler to modify mepc/mstatus) */
    lw      t0, TF_mepc(sp)
    csrw    mepc, t0
    lw      t0, TF_mstatus(sp)
    csrw    mstatus, t0

    lw      ra,  TF_x1(sp)
    lw      gp,  TF_x3(sp)
    lw      tp,  TF_x4(sp)
    lw      t0,  TF_x5(sp)
    lw      t1,  TF_x6(sp)
    lw      t2,  TF_x7(sp)
    lw      s0,  TF_x8(sp)
    lw      s1,  TF_x9(sp)
    lw      a0,  TF_x10(sp)
    lw      a1,  TF_x11(sp)
    lw      a2,  TF_x12(sp)
    lw      a3,  TF_x13(sp)
    lw      a4,  TF_x14(sp)
    lw      a5,  TF_x15(sp)
    lw      a6,  TF_x16(sp)
    lw      a7,  TF_x17(sp)
    lw      s2,  TF_x18(sp)
    lw      s3,  TF_x19(sp)
    lw      s4,  TF_x20(sp)
    lw      s5,  TF_x21(sp)
    lw      s6,  TF_x22(sp)
    lw      s7,  TF_x23(sp)
    lw      s8,  TF_x24(sp)
    lw      s9,  TF_x25(sp)
    lw      s10, TF_x26(sp)
    lw      s11, TF_x27(sp)
    lw      t3,  TF_x28(sp)
    lw      t4,  TF_x29(sp)
    lw      t5,  TF_x30(sp)
    lw      t6,  TF_x31(sp)

    addi    sp, sp, TF_SIZE
    mret

    .globl thread_launch
    .type thread_launch, @function
thread_launch:
    lw      t1, TF_mepc(a0)
    csrw    mepc, t1
    lw      t1, TF_mstatus(a0)
    csrw    mstatus, t1

    lw      ra,  TF_x1(a0)
    lw      gp,  TF_x3(a0)
    lw      tp,  TF_x4(a0)
    lw      t0,  TF_x5(a0)
    lw      t1,  TF_x6(a0)
    lw      t2,  TF_x7(a0)
    lw      s0,  TF_x8(a0)
    lw      s1,  TF_x9(a0)
    lw      a1,  TF_x11(a0)
    lw      a2,  TF_x12(a0)
    lw      a3,  TF_x13(a0)
    lw      a4,  TF_x14(a0)
    lw      a5,  TF_x15(a0)
    lw      a6,  TF_x16(a0)
    lw      a7,  TF_x17(a0)
    lw      s2,  TF_x18(a0)
    lw      s3,  TF_x19(a0)
    lw      s4,  TF_x20(a0)
    lw      s5,  TF_x21(a0)
    lw      s6,  TF_x22(a0)
    lw      s7,  TF_x23(a0)
    lw      s8,  TF_x24(a0)
    lw      s9,  TF_x25(a0)
    lw      s10, TF_x26(a0)
    lw      s11, TF_x27(a0)
    lw      t3,  TF_x28(a0)
    lw      t4,  TF_x29(a0)
    lw      t5,  TF_x30(a0)
    lw      t6,  TF_x31(a0)
    lw      sp,  TF_x2(a0)
    lw      a0,  TF_x10(a0)
    mret
