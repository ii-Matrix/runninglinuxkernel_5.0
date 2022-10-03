#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#define DEMO_DEV_NAME "/dev/demo_dev"

int main(int argc,char **argv)
{
    int fd;
    char buf[1024];
    int len;

    if(argc < 2)
    {
        printf("Usage: %s -w <string>\n",argv[0]);
        printf("    %s -r\n",argv[0]);
        return -1;
    }

    fd = open(DEMO_DEV_NAME,O_RDWR);
    if(fd < 0) {
        printf("open device %s failed\n",DEMO_DEV_NAME);
        return -1;
    }

    if((0 == strcmp(argv[1],"-w")) && (argc == 3))
    {
        len = strlen(argv[2]) + 1;
        len = len < 1024 ? len : 1024;
        write(fd,argv[2],len);
    }
    else
    {
        len = read(fd,buf,1024);
        buf[1023] = "\0";
        printf("APP read : %s\n",buf);
    }

    close(fd);

    return 0;
}