#ifndef _CMD_H
#define _CMD_H

typedef int (*cmd_fun_t)(const char *cmd, char *params[], int param_size);

typedef struct
{
    const char *cmd;
    cmd_fun_t handle;
    const char *des;
} cmd_t;

void CmdParse(char *cmd);

#endif
