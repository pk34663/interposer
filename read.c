#define _GNU_SOURCE
#include <stdlib.h>
#include <dlfcn.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

static ssize_t (*real_read)(int fd, void *buf, size_t count) = NULL;
static int read_delay = 0;


void setreaddelay(int delay)
{
      read_delay = delay;
}

ssize_t read(int fd, void *buf, size_t count)
{
    real_read = dlsym(RTLD_NEXT, "read");
    printf("read:chars#:%lu %d\n", count, fd);
    printf("read:chars#:%s\n", (char *)buf);
    printf("delaying read by %d %p\n", read_delay, &read_delay);
    sleep(read_delay);
    return real_read(fd, buf, count);
}
