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

void create_adminport(void);

pthread_t threadid;

void error(char *msg)
{
    fprintf(stderr,"%s\n",msg);
    exit(1);
}

void print_threadid(pthread_t id)
{
      size_t i;

      for (i = sizeof (id); i; i --)
          printf("%02x", *(((unsigned char*)&id) + i - 1));

      printf("\n");
}

void *_run(void *arg)
{
    printf("Creating socket\n");
    threadid = pthread_self();
    create_adminport();
}

static void __init() __attribute__((constructor));
void __init(void)
{
    pthread_t admin;

    pthread_create(&admin, NULL, _run, NULL);
}
