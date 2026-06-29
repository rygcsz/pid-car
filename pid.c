/*
 * pid.c - PID controller implementation
 * MSPM0G3507 PID Line-Following Car
 */

#include "pid.h"
#include <math.h>

void pid_init(pid_t *pid, float kp, float ki, float kd,
              float integral_limit, float output_limit)
{
    pid->kp             = kp;
    pid->ki             = ki;
    pid->kd             = kd;
    pid->integral       = 0.0f;
    pid->integral_limit = integral_limit;
    pid->prev_error     = 0.0f;
    pid->output_limit   = output_limit;
    pid->output         = 0.0f;
    pid->p_term         = 0.0f;
    pid->i_term         = 0.0f;
    pid->d_term         = 0.0f;
}

void pid_reset(pid_t *pid)
{
    pid->integral   = 0.0f;
    pid->prev_error = 0.0f;
    pid->output     = 0.0f;
    pid->p_term     = 0.0f;
    pid->i_term     = 0.0f;
    pid->d_term     = 0.0f;
}

float pid_calculate(pid_t *pid, float setpoint, float measurement)
{
    float error = measurement - setpoint;

    /* P term */
    pid->p_term = pid->kp * error;

    /* I term with anti-windup and decay */
    pid->integral += pid->ki * error;
    /* Decay integral toward zero when error is small (prevents stale buildup) */
    if (error > -1.0f && error < 1.0f) {
        pid->integral *= 0.95f;
    }
    if (pid->integral > pid->integral_limit)
        pid->integral = pid->integral_limit;
    else if (pid->integral < -pid->integral_limit)
        pid->integral = -pid->integral_limit;
    pid->i_term = pid->integral;

    /* D term */
    pid->d_term = pid->kd * (error - pid->prev_error);
    pid->prev_error = error;

    /* Sum and clamp */
    pid->output = pid->p_term + pid->i_term + pid->d_term;
    if (pid->output > pid->output_limit)
        pid->output = pid->output_limit;
    else if (pid->output < -pid->output_limit)
        pid->output = -pid->output_limit;

    return pid->output;
}
