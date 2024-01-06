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
#include "action.h"
#include <map>
#include <vector>
#include <string>

static ssize_t (*real_read)(int fd, void *buf, size_t count) = NULL;
static ssize_t (*real_write)(int fd, const void *buf, size_t count) = NULL;
void create_adminport(void);

pthread_t threadid;

class actiontbl {
    public:
        std::map<std::string,std::vector<action_t *> > tbl;
        static actiontbl& getinstance()
        {
            static actiontbl globalactiontbl;

            return globalactiontbl;
        }
};

void register_action(action_t *action)
{
    //void (*fptr)(int);
    printf("%s %p\n", action->action_name, action->action_fn);
    class actiontbl a = actiontbl::getinstance();
    a.tbl[action->action_name].push_back(action);
    //fptr = (void(*)(int))(action->action_fn);
}

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

void * _run(void *arg)
{
    printf("Creating socket\n");
    threadid = pthread_self();
    create_adminport();
}

static void __init() __attribute__((constructor));
void __init(void)
{
    pthread_t admin;

    real_read = reinterpret_cast<ssize_t (*)(int,void *,size_t)>(dlsym(RTLD_NEXT, "read"));
    real_write = reinterpret_cast<ssize_t (*)(int,const void *,size_t)>(dlsym(RTLD_NEXT, "write"));

    dlopen("./libdelay.so", RTLD_NOW);

    pthread_create(&admin, NULL, _run, NULL);
}

ssize_t read(int fd, void *buf, size_t count)
{
    printf("threadid in read %lu\n", threadid);
    if (pthread_self() != threadid)
    {
        class actiontbl a = actiontbl::getinstance();

        printf("read:chars#:%lu %d\n", count, fd);
        printf("read:chars#:%s\n", (char *)buf);
        return real_read(fd, buf, count);
    }
    else
    {
        return real_read(fd, buf, count);
    }
}

ssize_t write(int fd, const void *buf, size_t count)
{
    if (!pthread_equal(pthread_self(),threadid))
    {
        printf("write:chars#:%lu %d\n", count, fd);
        printf("write:chars#:%s\n", (char *)buf);
        return real_write(fd, buf, count);
    }
    else
    {
        return real_write(fd, buf, count);
    }
}
