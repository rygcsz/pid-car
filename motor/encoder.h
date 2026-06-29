/*
 * encoder.h - MG513 Hall encoder driver
 * MSPM0G3507 PID Line-Following Car
 *
 * Left  encoder: E1A=PB6, E1B=PB7 (GPIO interrupt, software decode)
 * Right encoder: E2A=PB2, E2B=PB3 (GPIO interrupt, software decode)
 *
 * Note: PRD specifies Timer capture mode for future speed闭环.
 *       Initial version uses GPIO interrupt software decoding for simplicity.
 */

#ifndef __ENCODER_H__
#define __ENCODER_H__

#include "ti_msp_dl_config.h"

/* Encoder pin definitions - matching PRD Section 2.2.2 */
#define LEFT_ENC_A_PIN    DL_GPIO_PIN_6
#define LEFT_ENC_B_PIN    DL_GPIO_PIN_7
#define RIGHT_ENC_A_PIN   DL_GPIO_PIN_2
#define RIGHT_ENC_B_PIN   DL_GPIO_PIN_3

void encoder_init(void);

/* Encoder counts (signed, positive = forward) */
extern volatile int encoder_left_count;
extern volatile int encoder_right_count;

/* Get speed in counts per period (call periodically) */
int encoder_get_left_speed(void);
int encoder_get_right_speed(void);

#endif /* __ENCODER_H__ */
