#include "cmd.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CMD_NUM 1

static int do_help(const char *cmd, char *params[], int param_size);

static const cmd_t app_cmd_array[CMD_NUM] = {
    {"help", do_help, "show all support cmd"},
};

static int do_help(const char *cmd, char *params[], int param_size)
{
    for (int i = 0; i < CMD_NUM; i++)
    {
        printf("%s", app_cmd_array[i].cmd);
        printf("--------%s", app_cmd_array[i].des);
        printf("\n");
    }

    return 0;
}

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
    printf("not valid cmd!\n");
}
