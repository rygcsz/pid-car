/*
 * pid.h - PID controller
 * MSPM0G3507 PID Line-Following Car
 */

#ifndef __PID_H__
#define __PID_H__

#include <stdint.h>

typedef struct {
    float kp;               /* Proportional gain */
    float ki;               /* Integral gain */
    float kd;               /* Derivative gain */
    float integral;         /* Integral accumulator */
    float integral_limit;   /* Anti-windup limit */
    float prev_error;       /* Previous error for derivative */
    float output_limit;     /* Output clamping limit */
    float output;           /* Latest output value */
    float p_term;           /* P term (for debug) */
    float i_term;           /* I term (for debug) */
    float d_term;           /* D term (for debug) */
} pid_t;

void pid_init(pid_t *pid, float kp, float ki, float kd,
              float integral_limit, float output_limit);
void pid_reset(pid_t *pid);
float pid_calculate(pid_t *pid, float setpoint, float measurement);

#endif /* __PID_H__ */
