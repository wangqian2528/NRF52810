#include "pid.h"

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <math.h>

void pid_init(pid_t *pid, float kp, float ki, float kd, float out_min, float out_max)
{
    if (pid == NULL)
        return;

    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;
    pid->ek = 0;
    pid->ek_1 = 0;
    pid->ek_2 = 0;
    pid->out_min = out_min;
    pid->out_max = out_max;
}

float pid_update(pid_t *pid, float set_value, float process_value)
{
    pid->ek = set_value - process_value;

    pid->out += pid->kp * (pid->ek - pid->ek_1) + pid->ki * pid->ek + pid->kd * (pid->ek - 2 * pid->ek_1 + pid->ek_2);

    pid->ek_2 = pid->ek_1;
    pid->ek_1 = pid->ek;

    if (pid->out > pid->out_max)
        pid->out = pid->out_max;
    if (pid->out < pid->out_min)
        pid->out = pid->out_min;

    return pid->out;
}
