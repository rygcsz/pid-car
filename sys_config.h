/*
 * sys_config.h - System configuration constants
 * MSPM0G3507 PID Line-Following Car
 *
 * All tunable parameters are defined here for easy adjustment.
 */

#ifndef __SYS_CONFIG_H__
#define __SYS_CONFIG_H__

/* ========== Line PID Parameters ========== */
/* Input: line error (-2.0 ~ +2.0 from grayscale_get_error) */
/* Output: PID output, mapped to PWM differential via TURN_SCALE */
#define LINE_PID_KP         3.0f    /* Proportional: responsive */
#define LINE_PID_KI         0.05f   /* Integral: small, correct steady offset */
#define LINE_PID_KD         1.0f    /* Derivative: dampen oscillation */
#define LINE_PID_INT_LIMIT  30.0f   /* Anti-windup */
#define LINE_PID_OUT_LIMIT  15.0f   /* Max PID output magnitude */

/* ========== Motion Parameters ========== */
#define BASE_PWM            525     /* Base PWM for straight line (750*0.7) */
#define TURN_SCALE          80.0f   /* PWM per unit of PID output */
#define RIGHT_TRIM          0.90f   /* Right motor trim: <1.0 if right is faster */
#define MAX_SPEED           1050    /* Max PWM duty (1500*0.7) */
#define PWM_PERIOD          3200    /* PWM period (10kHz @ 32MHz) */
#define SEARCH_PWM          560     /* PWM for arc search during line loss (800*0.7) */

/* ========== Speed PID Parameters (inner loop) ========== */
/* Used when encoder feedback is available for cascade PID */
#define SPEED_PID_KP        10.0f   /* Proportional */
#define SPEED_PID_KI        0.5f    /* Reduced from 2.0 to prevent integral runaway */
#define SPEED_PID_KD        0.0f
#define SPEED_PID_INT_LIMIT 400.0f
#define SPEED_PID_OUT_LIMIT 500.0f
#define BASE_SPEED_COUNTS   5       /* Encoder counts/10ms at SPEED_FF_PWM (calibrated from data) */
#define SPEED_FF_PWM        525
#define TURN_GAIN           2.0f    /* For cascade PID: amplifies differential */

/* ========== Grayscale Sensor ========== */
#define ADC_MAX_VALUE       4095
#define GRAY_DEAD_ZONE      0.2f    /* |error| < 0.2 treated as centered */
#define GRAY_RECOVERY_HOLD  5       /* Keep "line found" state for N cycles
                                       after briefly detecting the line during
                                       search, giving PID time to respond */
#define GRAY_CALIBRATION_SAMPLES 100

/* Per-sensor thresholds: ADC < threshold = on black line */
/* Set close to white values to maximize overlap between adjacent sensors */
/* Lower threshold = wider "on line" detection zone = smoother error values */
#define GRAY_THR_LEFT       2000    /* White~2218, Black~701 */
#define GRAY_THR_CENTER     1700    /* White~1810, Black~444 */
#define GRAY_THR_RIGHT      2300    /* White~2440, Black~898 */

/* Stop threshold: all sensors above this = clearly on white */
#define GRAY_STOP_VALUE     1800

/* Legacy */
#define GRAY_STOP_THRESHOLD 1000

/* ========== Control Loop ========== */
#define PID_PERIOD_MS       10

/* ========== Debug UART ========== */
#define DEBUG_UART_INST     Debug_UART_INST
#define DEBUG_UART_BAUD     115200
#define DEBUG_ENABLE        1

#endif /* __SYS_CONFIG_H__ */
