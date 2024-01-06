#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <libcli.h>

#include "action.h"


void register_action(action_t *action);

void delay(int delay);

int delay_admin(struct cli_def *cli, const char *command, char *argv[], int argc) {
    int i,delay;
    cli_print(cli, "called %s with \"%s\"", __func__, command);
    cli_print(cli, "%d arguments:", argc);
    for (i = 0; i < argc; i++) cli_print(cli, "        %s", argv[i]);

    delay = atoi(argv[0]);

    return CLI_OK;
}

static void __init() __attribute__((constructor));
void __init(void)
{
    action_t *action = (action_t *)malloc(sizeof (action_t));

    action->action_name = "delay";
    action->action_syscall = "read";
    action->action_type = ACTION_2;
    action->action_fn = (void *)delay;
    action->action_admin = delay_admin;

    register_action(action);
}

void delay(int delay)
{
    printf("Called delay with %d\n", delay);
    sleep(delay);
}
