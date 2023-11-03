#ifndef _WORK_H
#define _WORK_H

typedef enum
{
    WORK_IDLE = 0,
    WORK_START,
    WORK_INFLATING,
    WORK_HOLDING,
    WORK_DEFLATING,
} work_state_t;

void pump_init(void);
void valve_init(void);
void state_machine_loop(int tick);

#endif
