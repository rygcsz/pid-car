/*
 * board.h - Board level definitions
 * MSPM0G3507 PID Line-Following Car
 */

#ifndef __BOARD_H__
#define __BOARD_H__

#include "ti_msp_dl_config.h"

void board_init(void);
void delay_us(unsigned long us);
void delay_ms(unsigned long ms);

#endif /* __BOARD_H__ */
