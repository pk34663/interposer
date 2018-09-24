#define _GNU_SOURCE
#include <stdlib.h>
#include <dlfcn.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <libcli.h>

/* Function pointers to hold the value of the glibc functions */
static ssize_t (*real_write)(int fd, const void *buf, size_t count) = NULL;
static ssize_t (*real_read)(int fd, void *buf, size_t count) = NULL;
static int delay;


static pthread_t threadid;
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
    int i;
    cli_print(cli, "called %s with \"%s\"", __func__, command);
    cli_print(cli, "%d arguments:", argc);
    for (i = 0; i < argc; i++) cli_print(cli, "        %s", argv[i]);

    delay = atoi(argv[0]);
    setreaddelay(delay);

    return CLI_OK;
}

void error(char *msg)
{
    fprintf(stderr,"%s\n",msg);
    exit(1);
}

void *_run(void *arg)
{
    printf("Creating socket\n");
    threadid = pthread_self();
    int sockfd = socket(AF_INET,SOCK_STREAM,0);
    int newsockfd, n, clilen,reuseaddr;
    int portno = 6666;
    struct sockaddr_in serv_addr, cliaddr;
    struct cli_def *cli;
    char buffer[256];

    cli = cli_init();
    cli_set_banner(cli, "libcli test environment");
    cli_set_hostname(cli, "router");
    cli_telnet_protocol(cli, 1);
    cli_regular(cli, regular_callback);
    cli_regular_interval(cli, 5);                          // Defaults to 1 second
    cli_set_idle_timeout_callback(cli, 60, idle_timeout);  // 60 second idle timeout

    cli_register_command(cli, NULL, "delay", cmd_test, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, NULL);
    cli_register_command(cli, NULL, "simple", cmd_test, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, NULL);

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&serv_addr,sizeof(reuseaddr));
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
        error("ERROR on binding");
    listen(sockfd,5);

    //newsockfd = accept(sockfd, (struct sockaddr *) &cliaddr, &clilen);
    while (1)
    {
        newsockfd = accept(sockfd, NULL, &clilen);

        if (newsockfd < 0) 
            error("ERROR on accept");
        int j = 0;
        while (1)
        {
            char *cmd;
            bzero(buffer, 256);
            cli_loop(cli, newsockfd);
            shutdown(newsockfd, SHUT_RDWR);
            close(newsockfd);
            /*
               n = readline(newsockfd,buffer,255);
               if (n < 0) error("ERROR reading from socket");
               printf("Here is the message: %s\n",buffer);
               n = real_write(newsockfd,"I got your message",18);
               j = atoi(buffer);
               delay = j;
               printf("Setting delay %d %p\n",delay, &delay);
               printf("delay %d %p\n",delay, &delay);

               if (n < 0) error("ERROR writing to socket");*/
        }
    }
    cli_done(cli);
}
static void __init() __attribute__((constructor));

void __init(void)
{
    pthread_t admin;

    void *handle = dlopen("libpreloadread.so", RTLD_NOW|RTLD_GLOBAL);
    printf("read addr %p\n", &read);
    real_write = dlsym(RTLD_NEXT, "write");

    pthread_create(&admin, NULL, _run, &delay);
}

/* wrapping write function call */
ssize_t write(int fd, const void *buf, size_t count)
{
    if (pthread_self() != threadid)
    {
        printf("write:chars#:%lu %d\n", count, fd);
        printf("write:chars#:%s\n", (char *)buf);
        printf("delaying by %d %p\n", delay, &delay);
        sleep(delay);
        return real_write(fd, buf, count);
    }
    else
    {
        return real_write(fd, buf, count);
    }
}

/*
ssize_t read(int fd, void *buf, size_t count)
{
    real_read = dlsym(RTLD_NEXT, "read");
    if (pthread_self() != threadid)
    {
        printf("read:chars#:%lu %d\n", count, fd);
        printf("read:chars#:%s\n", (char *)buf);
        printf("delaying by %d %p\n", delay, &delay);
        sleep(delay);
        return real_read(fd, buf, count);
    }
    else
    {
        return real_read(fd, buf, count);
    }
}*/
