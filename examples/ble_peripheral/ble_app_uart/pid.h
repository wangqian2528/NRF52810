/*
 * @Description:
 * @Author: Walker
 * @Date: 2023-04-21 15:35:03
 * @LastEditTime: 2023-04-27 11:30:21
 * @LastEditors: Walker
 * @Version:
 * @History:
 */

#ifndef PID_H_
#define PID_H_

#include <stdint.h>

/*
 * Incremental PID
 * Out = Kp*[e(k)-e(k-1)] + Ki*e(k) + Kd[e(k)-2e(k-1)+e(k-2)]
 * e(k) = SV - PV, SV: set value, PV: process vale
 */
typedef struct
{
    float kp;
    float ki;
    float kd;
    float ek;
    float ek_1;
    float ek_2;
    float out_min;
    float out_max;
    float out;
} pid_t;

void pid_init(pid_t *pid, float kp, float ki, float kd, float out_min, float out_max);
float pid_update(pid_t *pid, float set_value, float process_value);

#endif /* PID_H_ */
