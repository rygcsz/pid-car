/*
 * motor.c - TB6612 D153B motor driver
 * MSPM0G3507 PID Line-Following Car
 *
 * After pin verification:
 *   PA28 (PINCM3) supports TIMA0_CCP3, TIMA1_CCP0
 *   PB4  (PINCM17) supports TIMA0_CCP2, TIMA1_CCP0
 *
 * Final assignment (hardware wiring adjusted):
 *   Left  Motor A: PWMA=PB4  (TIMA0_CCP2), AIN1=PA26, AIN2=PA27
 *   Right Motor B: PWMB=PA28 (TIMA1_CCP0), BIN1=PA25, BIN2=PB24
 *
 * TB6612 Truth Table:
 *   IN1=0 IN2=1 + PWM -> Forward
 *   IN1=1 IN2=0 + PWM -> Backward
 *   IN1=1 IN2=1 + PWM -> Brake
 *   IN1=0 IN2=0 + 0   -> Coast/Stop
 */

#include "motor.h"
#include "board.h"

/* ======== PWM manual initialization ======== */

/* TIMA0 clock config: BUSCLK / 1 = 32MHz */
static const DL_TimerA_ClockConfig gMotorAClockConfig = {
    .clockSel    = DL_TIMER_CLOCK_BUSCLK,
    .divideRatio = DL_TIMER_CLOCK_DIVIDE_1,
    .prescale    = 0U,
};

/* TIMA0 PWM config: edge-aligned up, period=3200 -> 10kHz @ 32MHz */
static const DL_TimerA_PWMConfig gMotorAPWMConfig = {
    .pwmMode         = DL_TIMER_PWM_MODE_EDGE_ALIGN_UP,
    .period          = PWM_PERIOD,
    .isTimerWithFourCC = true,
    .startTimer      = DL_TIMER_START,
};

/* TIMA1 clock config: BUSCLK / 1 = 32MHz */
static const DL_TimerA_ClockConfig gMotorBClockConfig = {
    .clockSel    = DL_TIMER_CLOCK_BUSCLK,
    .divideRatio = DL_TIMER_CLOCK_DIVIDE_1,
    .prescale    = 0U,
};

/* TIMA1 PWM config: edge-aligned up, period=3200 -> 10kHz @ 32MHz */
static const DL_TimerA_PWMConfig gMotorBPWMConfig = {
    .pwmMode         = DL_TIMER_PWM_MODE_EDGE_ALIGN_UP,
    .period          = PWM_PERIOD,
    .isTimerWithFourCC = true,
    .startTimer      = DL_TIMER_START,
};

void motor_init(void)
{
    /* === TIMA0: Left motor PWM on PB4 (TIMA0_CCP2, CC2) === */
    DL_TimerA_reset(TIMA0);
    DL_TimerA_enablePower(TIMA0);
    delay_cycles(POWER_STARTUP_DELAY);

    DL_TimerA_setClockConfig(TIMA0, (DL_TimerA_ClockConfig *)&gMotorAClockConfig);
    DL_TimerA_initPWMMode(TIMA0, (DL_TimerA_PWMConfig *)&gMotorAPWMConfig);

    DL_TimerA_setCaptureCompareOutCtl(TIMA0,
        DL_TIMER_CC_OCTL_INIT_VAL_LOW,
        DL_TIMER_CC_OCTL_INV_OUT_DISABLED,
        DL_TIMER_CC_OCTL_SRC_FUNCVAL,
        DL_TIMERA_CAPTURE_COMPARE_2_INDEX);
    DL_TimerA_setCaptCompUpdateMethod(TIMA0,
        DL_TIMER_CC_UPDATE_METHOD_IMMEDIATE,
        DL_TIMERA_CAPTURE_COMPARE_2_INDEX);
    DL_TimerA_setCaptureCompareValue(TIMA0, 0,
        DL_TIMERA_CAPTURE_COMPARE_2_INDEX);

    DL_TimerA_setCCPDirection(TIMA0, DL_TIMER_CC2_OUTPUT);
    DL_TimerA_enableClock(TIMA0);

    /* === TIMA1: Right motor PWM on PA28 (TIMA1_CCP0, CC0) === */
    DL_TimerA_reset(TIMA1);
    DL_TimerA_enablePower(TIMA1);
    delay_cycles(POWER_STARTUP_DELAY);

    DL_TimerA_setClockConfig(TIMA1, (DL_TimerA_ClockConfig *)&gMotorBClockConfig);
    DL_TimerA_initPWMMode(TIMA1, (DL_TimerA_PWMConfig *)&gMotorBPWMConfig);

    DL_TimerA_setCaptureCompareOutCtl(TIMA1,
        DL_TIMER_CC_OCTL_INIT_VAL_LOW,
        DL_TIMER_CC_OCTL_INV_OUT_DISABLED,
        DL_TIMER_CC_OCTL_SRC_FUNCVAL,
        DL_TIMERA_CAPTURE_COMPARE_0_INDEX);
    DL_TimerA_setCaptCompUpdateMethod(TIMA1,
        DL_TIMER_CC_UPDATE_METHOD_IMMEDIATE,
        DL_TIMERA_CAPTURE_COMPARE_0_INDEX);
    DL_TimerA_setCaptureCompareValue(TIMA1, 0,
        DL_TIMERA_CAPTURE_COMPARE_0_INDEX);

    DL_TimerA_setCCPDirection(TIMA1, DL_TIMER_CC0_OUTPUT);
    DL_TimerA_enableClock(TIMA1);

    /* === Configure PWM pins as peripheral function === */
    /* PB4 -> TIMA0_CCP2 (Left motor) */
    DL_GPIO_initPeripheralOutputFunction(Motor_PWM_PWMB_IOMUX,
        IOMUX_PINCM17_PF_TIMA0_CCP2);
    DL_GPIO_enableOutput(Motor_PWM_PWMB_PORT, Motor_PWM_PWMB_PIN);

    /* PA28 -> TIMA1_CCP0 (Right motor) */
    DL_GPIO_initPeripheralOutputFunction(Motor_PWM_PWMA_IOMUX,
        IOMUX_PINCM3_PF_TIMA1_CCP0);
    DL_GPIO_enableOutput(Motor_PWM_PWMA_PORT, Motor_PWM_PWMA_PIN);

    /* Motors start in stop state */
    motor_stop();
}

/*
 * Set left motor (Motor A) speed and direction
 * Left motor PWM: TIMA0 CC2 on PB4
 * pwm_val: positive = forward, negative = backward, 0 = stop
 */
void motor_set_left(int pwm_val)
{
    /* Left motor: physical direction is inverted (AIN1/AIN2 wiring swapped) */
    pwm_val = -pwm_val;  /* Negate so positive = physical forward */

    if (pwm_val > 0) {
        DL_GPIO_setPins(Motor_A_Dir_PORT, Motor_A_Dir_AIN1_PIN);
        DL_GPIO_clearPins(Motor_A_Dir_PORT, Motor_A_Dir_AIN2_PIN);
        if (pwm_val > PWM_PERIOD) pwm_val = PWM_PERIOD;
        DL_TimerA_setCaptureCompareValue(TIMA0, (uint32_t)pwm_val,
            DL_TIMERA_CAPTURE_COMPARE_2_INDEX);
    } else if (pwm_val < 0) {
        DL_GPIO_clearPins(Motor_A_Dir_PORT, Motor_A_Dir_AIN1_PIN);
        DL_GPIO_setPins(Motor_A_Dir_PORT, Motor_A_Dir_AIN2_PIN);
        pwm_val = -pwm_val;
        if (pwm_val > PWM_PERIOD) pwm_val = PWM_PERIOD;
        DL_TimerA_setCaptureCompareValue(TIMA0, (uint32_t)pwm_val,
            DL_TIMERA_CAPTURE_COMPARE_2_INDEX);
    } else {
        DL_GPIO_clearPins(Motor_A_Dir_PORT,
            Motor_A_Dir_AIN1_PIN | Motor_A_Dir_AIN2_PIN);
        DL_TimerA_setCaptureCompareValue(TIMA0, 0,
            DL_TIMERA_CAPTURE_COMPARE_2_INDEX);
    }
}

/*
 * Set right motor (Motor B) speed and direction
 * Right motor PWM: TIMA1 CC0 on PA28
 * BIN1=PA25 (GPIOA), BIN2=PB24 (GPIOB) are on different ports
 * pwm_val: positive = forward, negative = backward, 0 = stop
 */
void motor_set_right(int pwm_val)
{
    if (pwm_val > 0) {
        DL_GPIO_setPins(Motor_B_Dir_BIN1_PORT, Motor_B_Dir_BIN1_PIN);
        DL_GPIO_clearPins(Motor_B_Dir_BIN2_PORT, Motor_B_Dir_BIN2_PIN);
        if (pwm_val > PWM_PERIOD) pwm_val = PWM_PERIOD;
        DL_TimerA_setCaptureCompareValue(TIMA1, (uint32_t)pwm_val,
            DL_TIMERA_CAPTURE_COMPARE_0_INDEX);
    } else if (pwm_val < 0) {
        DL_GPIO_clearPins(Motor_B_Dir_BIN1_PORT, Motor_B_Dir_BIN1_PIN);
        DL_GPIO_setPins(Motor_B_Dir_BIN2_PORT, Motor_B_Dir_BIN2_PIN);
        pwm_val = -pwm_val;
        if (pwm_val > PWM_PERIOD) pwm_val = PWM_PERIOD;
        DL_TimerA_setCaptureCompareValue(TIMA1, (uint32_t)pwm_val,
            DL_TIMERA_CAPTURE_COMPARE_0_INDEX);
    } else {
        DL_GPIO_clearPins(Motor_B_Dir_BIN1_PORT, Motor_B_Dir_BIN1_PIN);
        DL_GPIO_clearPins(Motor_B_Dir_BIN2_PORT, Motor_B_Dir_BIN2_PIN);
        DL_TimerA_setCaptureCompareValue(TIMA1, 0,
            DL_TIMERA_CAPTURE_COMPARE_0_INDEX);
    }
}

void motor_set_both(int left_pwm, int right_pwm)
{
    motor_set_left(left_pwm);
    motor_set_right(right_pwm);
}

void motor_stop(void)
{
    DL_GPIO_clearPins(Motor_A_Dir_PORT,
        Motor_A_Dir_AIN1_PIN | Motor_A_Dir_AIN2_PIN);
    DL_GPIO_clearPins(Motor_B_Dir_BIN1_PORT, Motor_B_Dir_BIN1_PIN);
    DL_GPIO_clearPins(Motor_B_Dir_BIN2_PORT, Motor_B_Dir_BIN2_PIN);
    DL_TimerA_setCaptureCompareValue(TIMA0, 0,
        DL_TIMERA_CAPTURE_COMPARE_2_INDEX);
    DL_TimerA_setCaptureCompareValue(TIMA1, 0,
        DL_TIMERA_CAPTURE_COMPARE_0_INDEX);
}

void motor_brake(void)
{
    DL_GPIO_setPins(Motor_A_Dir_PORT,
        Motor_A_Dir_AIN1_PIN | Motor_A_Dir_AIN2_PIN);
    DL_GPIO_setPins(Motor_B_Dir_BIN1_PORT, Motor_B_Dir_BIN1_PIN);
    DL_GPIO_setPins(Motor_B_Dir_BIN2_PORT, Motor_B_Dir_BIN2_PIN);
    DL_TimerA_setCaptureCompareValue(TIMA0, PWM_PERIOD,
        DL_TIMERA_CAPTURE_COMPARE_2_INDEX);
    DL_TimerA_setCaptureCompareValue(TIMA1, PWM_PERIOD,
        DL_TIMERA_CAPTURE_COMPARE_0_INDEX);
}

/* === Movement helpers === */

void car_forward(int speed)
{
    motor_set_both(speed, speed);
}

void car_backward(int speed)
{
    motor_set_both(-speed, -speed);
}

void car_turn_left(int base_speed, int diff)
{
    int left  = base_speed - diff;
    int right = base_speed + diff;
    motor_set_both(left, right);
}

void car_turn_right(int base_speed, int diff)
{
    int left  = base_speed + diff;
    int right = base_speed - diff;
    motor_set_both(left, right);
}

void car_rotate_left(int speed)
{
    motor_set_both(-speed, speed);
}

void car_rotate_right(int speed)
{
    motor_set_both(speed, -speed);
}
