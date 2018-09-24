#define _GNU_SOURCE
#include <stdlib.h>
#include <dlfcn.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

static ssize_t (*real_write)(int fd, const void *buf, size_t count) = NULL;
static int writedelay = 0;

extern pthread_t threadid;

void setwritedelay(int delay)
{
      writedelay = delay;
}

ssize_t write(int fd, const void *buf, size_t count)
{
    real_write  = dlsym(RTLD_NEXT, "write");
    if (!pthread_equal(pthread_self(),threadid))
    {
        printf("write:chars#:%lu %d\n", count, fd);
        printf("write:chars#:%s\n", (char *)buf);
        printf("delaying by %d %p\n", writedelay, &writedelay);
        sleep(writedelay);
        return real_write(fd, buf, count);
    }
    else
    {
        return real_write(fd, buf, count);
    }
}
