//#include <libcli.h>

typedef void (*action_void) (void);
typedef void (*action_int) (void);
typedef void (*action_void_arg) (void *arg);
typedef int (*action_int_arg) (void);

typedef enum {
    ACTION_1 = 0,
    ACTION_2
} action_type_t;

typedef struct action {
    char *action_name;
    char *action_syscall;
    action_type_t action_type;
    void *action_fn;
    int (*action_admin)(struct cli_def *cli, const char *command, char *argv[], int argc);
} action_t;
