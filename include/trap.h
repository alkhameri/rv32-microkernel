// SPDX-License-Identifier: MIT
#pragma once

#include <stdint.h>

typedef struct trap_frame {
    uint32_t x[32];      /* General purpose registers */
    uint32_t mepc;
    uint32_t mstatus;
    uint32_t mcause;
    uint32_t mtval;
} trap_frame_t;

enum {
    REG_X0 = 0,
    REG_RA = 1,
    REG_SP = 2,
    REG_GP = 3,
    REG_TP = 4,
    REG_T0 = 5,
    REG_T1 = 6,
    REG_T2 = 7,
    REG_S0 = 8,
    REG_S1 = 9,
    REG_A0 = 10,
    REG_A1 = 11,
    REG_A2 = 12,
    REG_A3 = 13,
    REG_A4 = 14,
    REG_A5 = 15,
    REG_A6 = 16,
    REG_A7 = 17,
    REG_S2 = 18,
    REG_S3 = 19,
    REG_S4 = 20,
    REG_S5 = 21,
    REG_S6 = 22,
    REG_S7 = 23,
    REG_S8 = 24,
    REG_S9 = 25,
    REG_S10 = 26,
    REG_S11 = 27,
    REG_T3 = 28,
    REG_T4 = 29,
    REG_T5 = 30,
    REG_T6 = 31,
};

void trap_init(void);
void trap_handle(trap_frame_t *frame);
