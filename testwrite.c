#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


int main(void)
{
    int fd = open("/tmp/test",O_CREAT|O_RDWR);
    char buffer[] = "ABC";
    char rbuffer[256];
    printf("main read addr %p\n", &read);

    for (;;)
    {
      printf("Writing out\n");
      write(fd,buffer,sizeof (buffer));
      sleep(1);
      read(fd,rbuffer,sizeof (rbuffer));
    }
}
