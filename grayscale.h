/*
 * grayscale.h - 3-channel HJ-AXJ1 grayscale sensor driver
 * MSPM0G3507 PID Line-Following Car
 *
 * Left   sensor: PA17 (ADC1_CH2)
 * Center sensor: PB18 (ADC1_CH5)
 * Right  sensor: PB19 (ADC1_CH6)
 *
 * Black line -> high ADC value, White surface -> low ADC value
 */

#ifndef __GRAYSCALE_H__
#define __GRAYSCALE_H__

#include <stdint.h>

/* Sensor indices */
#define GRAY_LEFT     0
#define GRAY_CENTER   1
#define GRAY_RIGHT    2

/* Number of sensors */
#define GRAY_NUM_SENSORS  3

/* Sensor polarity: White surface = HIGH ADC, Black line = LOW ADC */
/* ADC < threshold = on black line */

void grayscale_init(void);

/* Read all 3 sensors at once (sequence mode) */
void grayscale_read_all(void);

/* Get individual sensor ADC value (after read_all) */
uint16_t grayscale_get(uint8_t index);

/*
 * Calculate line error using weighted average
 * Returns: negative = line on left, 0 = centered, positive = line on right
 * Range: approximately -2.0 ~ +2.0
 *
 * Examples (threshold=800):
 *   L:2100 C:964 R:1034 → error ≈ -1.2 (slightly left)
 *   L:1909 C:984 R:630  → error ≈ -1.7 (far left)
 *   L:600  C:2200 R:700 → error ≈ 0    (centered)
 *   L:500  C:900  R:2500 → error ≈ +1.8 (far right)
 */
float grayscale_get_error(void);

/* Check if line is currently lost (all sensors on white) */
uint8_t grayscale_is_line_lost(void);

/* Calibration */
void grayscale_calibrate(void);

/* Per-sensor threshold */
extern uint16_t grayscale_threshold[GRAY_NUM_SENSORS];

/* Latest ADC values */
extern uint16_t grayscale_value[GRAY_NUM_SENSORS];

#endif /* __GRAYSCALE_H__ */
