/*
 * board.c - Board initialization and delay functions
 * MSPM0G3507 PID Line-Following Car
 * Clock: SYSOSC 32MHz (no external crystal)
 */

#include "board.h"

void board_init(void)
{
    SYSCFG_DL_init();
}

void delay_us(unsigned long us)
{
    while (us--) {
        DL_Common_delayCycles(32);
    }
}

void delay_ms(unsigned long ms)
{
    delay_us(ms * 1000);
}
