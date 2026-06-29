/*
 * motor.h - TB6612 D153B motor driver
 * MSPM0G3507 PID Line-Following Car
 *
 * Left  Motor A: PWMA=PB4  (TIMA0_CCP2), AIN1=PA26, AIN2=PA27
 * Right Motor B: PWMB=PA28 (TIMA1_CCP0), BIN1=PA25, BIN2=PB24
 */

#ifndef __MOTOR_H__
#define __MOTOR_H__

#include "ti_msp_dl_config.h"
#include "sys_config.h"

/* Motor direction enum */
typedef enum {
    MOTOR_DIR_FORWARD  = 0,
    MOTOR_DIR_BACKWARD = 1,
    MOTOR_DIR_BRAKE    = 2,
    MOTOR_DIR_STOP     = 3,
} motor_dir_t;

/* Function prototypes */
void motor_init(void);
void motor_set_left(int pwm_val);
void motor_set_right(int pwm_val);
void motor_set_both(int left_pwm, int right_pwm);
void motor_stop(void);
void motor_brake(void);

/* Movement helpers */
void car_forward(int speed);
void car_backward(int speed);
void car_turn_left(int base_speed, int diff);
void car_turn_right(int base_speed, int diff);
void car_rotate_left(int speed);
void car_rotate_right(int speed);

#endif /* __MOTOR_H__ */
