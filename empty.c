/*
 * main.c - Cascade PID line-following car
 * MSPM0G3507 PID Line-Following Car
 *
 * Architecture:
 *   Grayscale → Line PID (outer) → target speed differential
 *              ↘ Speed PID (inner) → PWM ← Encoder feedback
 *
 * The inner speed PID automatically compensates for motor asymmetry.
 */

#include "ti_msp_dl_config.h"
#include "sys_config.h"
#include "board.h"
#include "motor.h"
#include "pid.h"
#include "grayscale.h"
#include "uart_debug.h"
#include "encoder.h"

/* ======== PID instances ======== */
static pid_t line_pid;
static pid_t left_speed_pid;
static pid_t right_speed_pid;

/* ======== Flag set by PID timer interrupt ======== */
static volatile uint8_t pid_update_flag = 0;

/* ======== Debug counter ======== */
static uint32_t debug_counter = 0;

int main(void)
{
    /* 1. Init */
    SYSCFG_DL_init();
    motor_init();
    grayscale_init();
    encoder_init();
    uart_debug_init();

    /* Configure S1 button on PB21 as input with pull-up (active low) */
    DL_GPIO_initDigitalInputFeatures(IOMUX_PINCM49,
        DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_UP,
        DL_GPIO_HYSTERESIS_ENABLE, DL_GPIO_WAKEUP_DISABLE);

    /* 2. Init PID controllers */
    pid_init(&line_pid, LINE_PID_KP, LINE_PID_KI, LINE_PID_KD,
             LINE_PID_INT_LIMIT, LINE_PID_OUT_LIMIT);
    pid_init(&left_speed_pid, SPEED_PID_KP, SPEED_PID_KI, SPEED_PID_KD,
             SPEED_PID_INT_LIMIT, SPEED_PID_OUT_LIMIT);
    pid_init(&right_speed_pid, SPEED_PID_KP, SPEED_PID_KI, SPEED_PID_KD,
             SPEED_PID_INT_LIMIT, SPEED_PID_OUT_LIMIT);

    /* 3. Start PID timer (10ms) */
    NVIC_EnableIRQ(PID_Timer_INST_INT_IRQN);
    DL_TimerG_startCounter(PID_Timer_INST);

    uart_debug_printf("=== Cascade PID Car ===\r\n");
    uart_debug_printf("Thr: L=%u C=%u R=%u\r\n",
        GRAY_THR_LEFT, GRAY_THR_CENTER, GRAY_THR_RIGHT);
    uart_debug_printf("Press S1 to start...\r\n");

    /* Wait for S1 button press (PB21, active low) */
    while (DL_GPIO_readPins(GPIOB, DL_GPIO_PIN_21)) {
        ;   /* Button not pressed, pin is high */
    }
    /* Wait for release */
    while (!DL_GPIO_readPins(GPIOB, DL_GPIO_PIN_21)) {
        ;
    }
    delay_ms(1000);
    uart_debug_printf("GO!\r\n");

    /* 4. Main loop */
    while (1) {
        if (pid_update_flag) {
            pid_update_flag = 0;

            /* ---- Read encoder speeds (counts per 10ms) ---- */
            int left_speed  = encoder_get_left_speed();
            int right_speed = encoder_get_right_speed();

            /* ---- Read grayscale sensors ---- */
            grayscale_read_all();
            float error = grayscale_get_error();

            float left_pwm, right_pwm;

            if (grayscale_is_line_lost()) {
                /* ==== SEARCH MODE: arc turn, not pivot ==== */
                /* Pivot (both wheels opposite) is too fast - the sensor
                 * sweeps past the line before the car can react.
                 * Instead: one wheel drives forward slowly, the other
                 * drives faster → arc turn, giving sensors time to
                 * detect the line and PID time to respond. */
                float search_dir = (error >= 0.0f) ? 1.0f : -1.0f;
                /* search_dir > 0 → line was on right → turn right
                 * search_dir < 0 → line was on left  → turn left  */
                if (search_dir > 0.0f) {
                    left_pwm  =  SEARCH_PWM;      /* fast forward */
                    right_pwm =  SEARCH_PWM / 3;  /* slow forward */
                } else {
                    left_pwm  =  SEARCH_PWM / 3;  /* slow forward */
                    right_pwm =  SEARCH_PWM;      /* fast forward */
                }

                /* Reset all PIDs so they start clean when line is found */
                pid_reset(&line_pid);
                pid_reset(&left_speed_pid);
                pid_reset(&right_speed_pid);
            } else {
                /* ==== TRACKING MODE: normal PID line-following ==== */
                float line_out = pid_calculate(&line_pid, 0.0f, error);
                float base = (float)BASE_SPEED_COUNTS;

                /* line_out < 0 → line on left → left slower, right faster */
                float left_target  = base + line_out * TURN_GAIN;
                float right_target = base - line_out * TURN_GAIN;

                if (left_target >= 0) {
                    float pid_out = pid_calculate(&left_speed_pid,
                                                    (float)left_speed, left_target);
                    left_pwm = (float)SPEED_FF_PWM + pid_out;
                    if (left_pwm < 0) left_pwm = 0;
                } else {
                    pid_reset(&left_speed_pid);
                    float ratio = (-left_target) / base;
                    if (ratio > 1.0f) ratio = 1.0f;
                    left_pwm = -(float)SPEED_FF_PWM * ratio;
                }

                if (right_target >= 0) {
                    float pid_out = pid_calculate(&right_speed_pid,
                                                    (float)right_speed, right_target);
                    right_pwm = (float)SPEED_FF_PWM + pid_out;
                    if (right_pwm < 0) right_pwm = 0;
                } else {
                    pid_reset(&right_speed_pid);
                    float ratio = (-right_target) / base;
                    if (ratio > 1.0f) ratio = 1.0f;
                    right_pwm = -(float)SPEED_FF_PWM * ratio;
                }

                /* Clamp PWM */
                if (left_pwm > MAX_SPEED)   left_pwm  = MAX_SPEED;
                if (left_pwm < -MAX_SPEED)  left_pwm  = -MAX_SPEED;
                if (right_pwm > MAX_SPEED)  right_pwm = MAX_SPEED;
                if (right_pwm < -MAX_SPEED) right_pwm = -MAX_SPEED;
            }

            motor_set_both((int)left_pwm, (int)right_pwm);

            /* Debug every 10 cycles (~100ms) */
            debug_counter++;
            if (debug_counter >= 10) {
                debug_counter = 0;
                int err_i = (int)(error * 100);
                int lp_i = (int)(line_pid.p_term * 100);
                int li_i = (int)(line_pid.i_term * 100);
                int ld_i = (int)(line_pid.d_term * 100);
                uart_debug_printf("L:%u C:%u R:%u E:%d P:%d I:%d D:%d S:%d/%d LOST:%d PWM:%d/%d\r\n",
                    grayscale_value[GRAY_LEFT],
                    grayscale_value[GRAY_CENTER],
                    grayscale_value[GRAY_RIGHT],
                    err_i,
                    lp_i, li_i, ld_i,
                    left_speed, right_speed,
                    grayscale_is_line_lost(),
                    (int)left_pwm, (int)right_pwm);
            }
        }
    }
}

void PID_Timer_INST_IRQHandler(void)
{
    switch (DL_TimerG_getPendingInterrupt(PID_Timer_INST))
    {
        case DL_TIMER_IIDX_LOAD:
            pid_update_flag = 1;
            break;
        default:
            break;
    }
}
