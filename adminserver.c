#include <stdlib.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <libcli.h>

//static pthread_t threadid;
unsigned int regular_count = 0;
unsigned int debug_regular = 0;

int idle_timeout(struct cli_def *cli) {
    cli_print(cli, "Custom idle timeout");
    return CLI_QUIT;
}

int regular_callback(struct cli_def *cli) {
    regular_count++;
    if (debug_regular) {
        cli_print(cli, "Regular callback - %u times so far", regular_count);
        cli_reprompt(cli);
    }
    return CLI_OK;
}

int cmd_test(struct cli_def *cli, const char *command, char *argv[], int argc) {
    int i,delay;
    cli_print(cli, "called %s with \"%s\"", __func__, command);
    cli_print(cli, "%d arguments:", argc);
    for (i = 0; i < argc; i++) cli_print(cli, "        %s", argv[i]);

    delay = atoi(argv[0]);
    setwritedelay(delay);
    setreaddelay(delay);

    return CLI_OK;
}

void create_adminport(void)
{
    printf("Creating socket\n");
    int sockfd = socket(AF_INET,SOCK_STREAM,0);
    int newsockfd, n, clilen,reuseaddr;
    int portno = 6666;
    struct sockaddr_in serv_addr, cliaddr;
    struct cli_def *cli;
    char prompt[256];

    sprintf(prompt,"PID %d>",getpid());
    cli = cli_init();
    cli_set_banner(cli, "Interpose Admin Console");
    cli_set_hostname(cli, prompt);
    cli_telnet_protocol(cli, 1);
    cli_regular(cli, regular_callback);
    cli_regular_interval(cli, 5);                          // Defaults to 1 second
    cli_set_idle_timeout_callback(cli, 60, idle_timeout);  // 60 second idle timeout

    cli_register_command(cli, NULL, "delay", cmd_test, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, NULL);
    cli_register_command(cli, NULL, "simple", cmd_test, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, NULL);

    bzero((char *) &serv_addr, sizeof (serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&serv_addr, sizeof (reuseaddr));
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof (serv_addr)) < 0) 
        error("ERROR on binding");
    listen(sockfd,5);

    while (1)
    {
        newsockfd = accept(sockfd, NULL, &clilen);

        if (newsockfd < 0) 
            error("ERROR on accept");
        int j = 0;
        while (1)
        {
            cli_loop(cli, newsockfd);
            shutdown(newsockfd, SHUT_RDWR);
            close(newsockfd);
            break;
        }
    }
    cli_done(cli);
}
