/*
 * grayscale.c - 3-channel HJ-AXJ1 grayscale sensor driver
 * MSPM0G3507 PID Line-Following Car
 *
 * ADC1 sequence mode: CH5(PB18) -> CH2(PA17) -> CH6(PB19)
 *   MEM0 = Center (CH5, PB18)
 *   MEM1 = Left   (CH2, PA17)
 *   MEM2 = Right  (CH6, PB19)
 *
 * Black line -> high ADC value, White surface -> low ADC value
 */

#include "grayscale.h"
#include "ti_msp_dl_config.h"
#include "sys_config.h"
#include "board.h"

/* Per-sensor threshold: ADC > threshold = on black line */
uint16_t grayscale_threshold[GRAY_NUM_SENSORS] = {
    GRAY_THR_LEFT,     /* Left   */
    GRAY_THR_CENTER,   /* Center */
    GRAY_THR_RIGHT,    /* Right  */
};

/* Latest ADC readings */
uint16_t grayscale_value[GRAY_NUM_SENSORS] = {0, 0, 0};

/* Previous error for direction memory when line is lost */
static float prev_error = 0.0f;

/* Peak direction: remembers the direction of the strongest error.
 * When the line sweeps across sensors before being lost, the last
 * sensor to see the line is on the OPPOSITE side, giving wrong
 * prev_error.  Only update when error magnitude exceeds the previous
 * peak, so brief sign-flips during line sweep don't overwrite it. */
static float peak_direction = 0.0f;

/* Line-lost tracking: set when line is lost, cleared when found */
static uint8_t line_lost = 0;

/* Recovery hold: when the line is briefly found during a search,
 * keep the "line found" state for a few extra cycles so the PID
 * has time to respond before the sensor sweeps past the line. */
static uint8_t recovery_hold = 0;

void grayscale_init(void)
{
    /* ADC pins are in analog mode by default after reset */
    /* ADC is initialized by SysConfig (SYSCFG_DL_init) */
    /* Dummy read to prime the ADC */
    grayscale_read_all();
}

/*
 * Read all 3 sensors in one sequence conversion
 * Follows SDK example pattern: start → wait → read → re-enable
 */
void grayscale_read_all(void)
{
    /* Start sequence conversion (all 3 MEM slots) */
    DL_ADC12_startConversion(Grayscale_ADC_INST);

    /* Wait for last MEM (MEM2) to complete */
    while (!(DL_ADC12_getRawInterruptStatus(Grayscale_ADC_INST,
                 DL_ADC12_INTERRUPT_MEM2_RESULT_LOADED) &
             DL_ADC12_INTERRUPT_MEM2_RESULT_LOADED))
        ;

    /* Read results: MEM0=Center, MEM1=Left, MEM2=Right */
    grayscale_value[GRAY_CENTER] = DL_ADC12_getMemResult(
        Grayscale_ADC_INST, DL_ADC12_MEM_IDX_0);
    grayscale_value[GRAY_LEFT]   = DL_ADC12_getMemResult(
        Grayscale_ADC_INST, DL_ADC12_MEM_IDX_1);
    grayscale_value[GRAY_RIGHT]  = DL_ADC12_getMemResult(
        Grayscale_ADC_INST, DL_ADC12_MEM_IDX_2);

    /* Clear all interrupt flags and re-enable for next sequence */
    DL_ADC12_clearInterruptStatus(Grayscale_ADC_INST,
        DL_ADC12_INTERRUPT_MEM0_RESULT_LOADED |
        DL_ADC12_INTERRUPT_MEM1_RESULT_LOADED |
        DL_ADC12_INTERRUPT_MEM2_RESULT_LOADED);
    DL_ADC12_enableConversions(Grayscale_ADC_INST);
}

/*
 * Get individual sensor value
 */
uint16_t grayscale_get(uint8_t index)
{
    if (index < GRAY_NUM_SENSORS)
        return grayscale_value[index];
    return 0;
}

/*
 * Calculate line error using weighted average
 *
 * For this sensor: White surface = HIGH ADC, Black line = LOW ADC
 * So "on the line" means ADC < threshold.
 *
 * For each sensor, calculate how much it's "on the line":
 *   on_line = max(0, threshold - ADC_value)
 *
 * Then weighted average:
 *   error = (left_on * (-2) + center_on * 0 + right_on * (+2)) / (left_on + center_on + right_on)
 *
 * Examples (threshold_center=1130):
 *   L:1975 C:449 R:2179 → left_on=0 center_on=681 right_on=0
 *     error = 0 / 681 = 0 (centered)
 *   L:701 C:1604 R:2182 → left_on=759 center_on=0 right_on=0
 *     error = (-1518) / 759 = -2.0 (far left)
 */
float grayscale_get_error(void)
{
    int32_t left_val   = (int32_t)grayscale_value[GRAY_LEFT];
    int32_t center_val = (int32_t)grayscale_value[GRAY_CENTER];
    int32_t right_val  = (int32_t)grayscale_value[GRAY_RIGHT];

    /* Calculate how much each sensor is "on the line" */
    /* ADC < threshold = on black line; on_line = threshold - ADC */
    int32_t left_on   = (left_val   < grayscale_threshold[GRAY_LEFT])   ? (grayscale_threshold[GRAY_LEFT]   - left_val)   : 0;
    int32_t center_on = (center_val < grayscale_threshold[GRAY_CENTER]) ? (grayscale_threshold[GRAY_CENTER] - center_val) : 0;
    int32_t right_on  = (right_val  < grayscale_threshold[GRAY_RIGHT])  ? (grayscale_threshold[GRAY_RIGHT]  - right_val)  : 0;

    int32_t total = left_on + center_on + right_on;

    float error;

    if (total > 0) {
        /* Weighted average: left=-2, center=0, right=+2 */
        error = (float)(left_on * (-2) + right_on * 2) / (float)total;

        /* Dead zone: ignore tiny deviations, only correct real offsets */
        if (error > -GRAY_DEAD_ZONE && error < GRAY_DEAD_ZONE)
            error = 0.0f;

        /* Update peak direction: only when error exceeds previous peak.
         * This prevents the line sweeping across sensors (right→left)
         * from overwriting the correct direction before losing the line. */
        if (error > 0.0f && error > peak_direction)
            peak_direction = error;
        else if (error < 0.0f && error < peak_direction)
            peak_direction = error;

        /* Decay peak when centered, so stale direction fades after
         * the car has been on the line for a while */
        if (error == 0.0f)
            peak_direction *= 0.8f;

        line_lost = 0;
        recovery_hold = GRAY_RECOVERY_HOLD;
        prev_error = error;
    } else if (recovery_hold > 0) {
        /* Line just lost but still in recovery hold period.
         * Keep using the last sensor-based error so the PID
         * continues the correction instead of immediately
         * switching back to search mode. */
        recovery_hold--;
        line_lost = 0;
        error = prev_error;
    } else {
        /* All below threshold - lost line, search in peak direction */
        error = peak_direction;
        line_lost = 1;
    }

    prev_error = error;
    return error;
}

uint8_t grayscale_is_line_lost(void)
{
    return line_lost;
}

/*
 * Calibrate all 3 sensors
 * Place sensors on WHITE surface first, then on BLACK line
 */
void grayscale_calibrate(void)
{
    uint32_t sum;
    uint16_t white_val[GRAY_NUM_SENSORS];
    uint16_t black_val[GRAY_NUM_SENSORS];

    /* Sample white surface */
    for (int s = 0; s < GRAY_NUM_SENSORS; s++)
        white_val[s] = 0;

    for (int i = 0; i < GRAY_CALIBRATION_SAMPLES; i++) {
        grayscale_read_all();
        for (int s = 0; s < GRAY_NUM_SENSORS; s++)
            white_val[s] += grayscale_value[s];
        delay_ms(5);
    }
    for (int s = 0; s < GRAY_NUM_SENSORS; s++)
        white_val[s] /= GRAY_CALIBRATION_SAMPLES;

    delay_ms(1000);  /* Time to move sensor to black line */

    /* Sample black line */
    for (int s = 0; s < GRAY_NUM_SENSORS; s++)
        black_val[s] = 0;

    for (int i = 0; i < GRAY_CALIBRATION_SAMPLES; i++) {
        grayscale_read_all();
        for (int s = 0; s < GRAY_NUM_SENSORS; s++)
            black_val[s] += grayscale_value[s];
        delay_ms(5);
    }
    for (int s = 0; s < GRAY_NUM_SENSORS; s++)
        black_val[s] /= GRAY_CALIBRATION_SAMPLES;

    /* Threshold = midpoint between white and black, biased slightly toward white */
    for (int s = 0; s < GRAY_NUM_SENSORS; s++)
        grayscale_threshold[s] = (white_val[s] + black_val[s]) * 3 / 5;
}
