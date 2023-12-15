#include "cmd.h"
#include "work.h"
#include "pressure.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char charBuf[128];

const char helpStr[] = "this is a help str!";
const char validStr[] = "this is a valid str!";

const char startStr[] = "start!";
const char stopStr[] = "stop!";

extern work_state_t g_work_state;
extern float g_target_pressure_value; // 目标压力
extern int g_hold_time;
extern int g_deflating_time;

extern uint32_t App_Printf(uint8_t *data, uint16_t length);

#define CMD_NUM 8

static int do_start(const char *cmd, char *params[], int param_size);
static int do_stop(const char *cmd, char *params[], int param_size);
static int do_getPressure(const char *cmd, char *params[], int param_size);
static int do_setTargetPressure(const char *cmd, char *params[], int param_size);
static int do_setDwellTime(const char *cmd, char *params[], int param_size);
static int do_getDwellTime(const char *cmd, char *params[], int param_size);
static int do_setintervalTime(const char *cmd, char *params[], int param_size);
static int do_getintervalTime(const char *cmd, char *params[], int param_size);

static const cmd_t app_cmd_array[CMD_NUM] = {
    {"Start", do_start, "start run"},
    {"Stop", do_stop, "stop run"},
    {"getPressure", do_getPressure, "get Pressure"},
    {"setTargetPressure", do_setTargetPressure, "set Target Pressure"},
    {"setDwellTime", do_setDwellTime, "set Dwell Time"},
    {"getDwellTime", do_getDwellTime, "get Dwell Time"},
    {"setintervalTime", do_setintervalTime, "set interval Time"},
    {"getintervalTime", do_getintervalTime, "get interval Time"},
};

//
//
//
//
//
static int do_start(const char *cmd, char *params[], int param_size)
{
    g_work_state = WORK_START;
    App_Printf((uint8_t *)startStr, strlen(startStr));
    return 0;
}

static int do_stop(const char *cmd, char *params[], int param_size)
{
    g_work_state = WORK_IDLE;
    App_Printf((uint8_t *)stopStr, strlen(stopStr));
    return 0;
}

static int do_getPressure(const char *cmd, char *params[], int param_size)
{
    char strbuf[32];
    float pre = get_pressure_value();
    sprintf(strbuf, "pressure:%0.2f\n", pre);
    App_Printf((uint8_t *)strbuf, strlen(strbuf));
    return 0;
}

static int do_setTargetPressure(const char *cmd, char *params[], int param_size)
{
    char strbuf[32];

    if (param_size != 1)
    {
        return 0;
    }

    float pre = atof(params[0]);
    g_target_pressure_value = pre;

    sprintf(strbuf, "set pressure:%0.2f\n", pre);
    App_Printf((uint8_t *)strbuf, strlen(strbuf));

    return 0;
}

static int do_setDwellTime(const char *cmd, char *params[], int param_size)
{
    char strbuf[32];

    if (param_size != 1)
    {
        return 0;
    }

    int setTime = atoi(params[0]);
    g_hold_time = setTime;

    sprintf(strbuf, "hold time :%d\n", setTime);
    App_Printf((uint8_t *)strbuf, strlen(strbuf));

    return 0;
}

static int do_getDwellTime(const char *cmd, char *params[], int param_size)
{
    char strbuf[32];

    int setTime = g_hold_time;

    sprintf(strbuf, "hold time :%d\n", setTime);
    App_Printf((uint8_t *)strbuf, strlen(strbuf));

    return 0;
}

static int do_setintervalTime(const char *cmd, char *params[], int param_size)
{
    char strbuf[32];

    if (param_size != 1)
    {
        return 0;
    }

    int setTime = atoi(params[0]);
    g_deflating_time = setTime;

    sprintf(strbuf, "interval time :%d\n", setTime);
    App_Printf((uint8_t *)strbuf, strlen(strbuf));

    return 0;
}

static int do_getintervalTime(const char *cmd, char *params[], int param_size)
{
    char strbuf[32];

    int setTime = g_deflating_time;

    sprintf(strbuf, "interval time :%d\n", setTime);
    App_Printf((uint8_t *)strbuf, strlen(strbuf));

    return 0;
}

//
//
//
//
//
//
int ssz_str_split(char *str, const char *accept_delimiter, char *substrs[], int substr_max)
{
    if (!str)
    {
        return 0;
    }

    int substr_index = 0;
    bool is_in_substr = false;
    while (*str != 0 && substr_index < substr_max)
    {
        if (strchr(accept_delimiter, *str) != NULL)
        {
            // if find the delimiter, set it as 0 and skip
            *str = 0;
            is_in_substr = false; // substr is end
        }
        else if (!is_in_substr)
        {
            is_in_substr = true;
            // get one substr start
            substrs[substr_index] = str;
            substr_index++;
        }
        str++;
    }

    return substr_index;
}

void CmdParse(char *cmd)
{
    char *work_str;
    char *params[5];
    uint8_t param_size = 0;

    work_str = strchr(cmd, ' ');
    if (work_str != NULL)
    {
        *work_str = 0;
        work_str++;
    }
    param_size = ssz_str_split(work_str, " ", params, 5);

    for (int i = 0; i < CMD_NUM; i++)
    {
        if (strcmp(app_cmd_array[i].cmd, cmd) == 0)
        {
            app_cmd_array[i].handle(cmd, params, param_size);
            return;
        }
    }
    App_Printf((uint8_t *)validStr, strlen(validStr));
}
