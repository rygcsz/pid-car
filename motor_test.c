/*
 * motor_test.c - Sensor calibration & motor test
 * MSPM0G3507 PID Line-Following Car
 *
 * Step 1: Run this program to calibrate thresholds
 * Step 2: Copy the printed threshold values into sys_config.h
 * Step 3: Exclude this file, build empty.c to run PID
 *
 * Usage: In CCS, exclude empty.c from build, include this file instead.
 */

#include "ti_msp_dl_config.h"
#include "sys_config.h"
#include "board.h"
#include "motor.h"
#include "grayscale.h"
#include "uart_debug.h"
#include "encoder.h"

int main(void)
{
    SYSCFG_DL_init();
    // motor_init();  /* Not needed for sensor calibration only */
    grayscale_init();
    encoder_init();
    uart_debug_init();

    uart_debug_printf("=== Sensor Calibration & Test ===\r\n");

    /* ---- Calibration ---- */
    uart_debug_printf("Step1: Place ALL sensors on WHITE, wait 3s...\r\n");
    delay_ms(3000);

    /* Sample white surface */
    uint16_t white_val[GRAY_NUM_SENSORS] = {0, 0, 0};
    for (int i = 0; i < GRAY_CALIBRATION_SAMPLES; i++) {
        grayscale_read_all();
        for (int s = 0; s < GRAY_NUM_SENSORS; s++)
            white_val[s] += grayscale_value[s];
        delay_ms(5);
    }
    for (int s = 0; s < GRAY_NUM_SENSORS; s++)
        white_val[s] /= GRAY_CALIBRATION_SAMPLES;

    uart_debug_printf("White: L=%u C=%u R=%u\r\n",
        white_val[GRAY_LEFT], white_val[GRAY_CENTER], white_val[GRAY_RIGHT]);

    uart_debug_printf("Step2: Place ALL sensors on BLACK, wait 3s...\r\n");
    delay_ms(3000);

    /* Sample black line */
    uint16_t black_val[GRAY_NUM_SENSORS] = {0, 0, 0};
    for (int i = 0; i < GRAY_CALIBRATION_SAMPLES; i++) {
        grayscale_read_all();
        for (int s = 0; s < GRAY_NUM_SENSORS; s++)
            black_val[s] += grayscale_value[s];
        delay_ms(5);
    }
    for (int s = 0; s < GRAY_NUM_SENSORS; s++)
        black_val[s] /= GRAY_CALIBRATION_SAMPLES;

    uart_debug_printf("Black: L=%u C=%u R=%u\r\n",
        black_val[GRAY_LEFT], black_val[GRAY_CENTER], black_val[GRAY_RIGHT]);

    /* Calculate thresholds: (white + black) * 3 / 5 */
    uint16_t threshold[GRAY_NUM_SENSORS];
    for (int s = 0; s < GRAY_NUM_SENSORS; s++)
        threshold[s] = (white_val[s] + black_val[s]) * 3 / 5;

    uart_debug_printf("========================================\r\n");
    uart_debug_printf("Thresholds -> Copy to sys_config.h:\r\n");
    uart_debug_printf("  GRAY_THR_LEFT   = %u\r\n", threshold[GRAY_LEFT]);
    uart_debug_printf("  GRAY_THR_CENTER = %u\r\n", threshold[GRAY_CENTER]);
    uart_debug_printf("  GRAY_THR_RIGHT  = %u\r\n", threshold[GRAY_RIGHT]);
    uart_debug_printf("========================================\r\n");

    /* Apply thresholds for live test */
    for (int s = 0; s < GRAY_NUM_SENSORS; s++)
        grayscale_threshold[s] = threshold[s];

    uart_debug_printf("\r\nLive test: L/C/R values + encoder speed\r\n");

    /* ---- Live test loop ---- */
    while (1) {
        grayscale_read_all();
        int left_speed  = encoder_get_left_speed();
        int right_speed = encoder_get_right_speed();

        uart_debug_printf("L:%u C:%u R:%u  Spd:%d/%d\r\n",
            grayscale_value[GRAY_LEFT],
            grayscale_value[GRAY_CENTER],
            grayscale_value[GRAY_RIGHT],
            left_speed, right_speed);

        delay_ms(100);
    }
}
