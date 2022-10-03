#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#define DEMO_DEV_NAME "/dev/my_demo_dev_fifo"

int main(int argc,char **argv)
{
    int fd;
    char buf[64];
    size_t len;

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
        len = len < 64 ? len : 64;
        write(fd,argv[2],len);
    }
    else if((0 == strcmp(argv[1],"-r")) && (argc == 2))
    {
        memset(buf,0,sizeof(buf));
        len = read(fd,buf,64);
       // buf[len] = "\0";
        printf("APP read : %d bytes %s\n",len,buf);
    }
    else
    {
        printf("Usage: %s -w <string>\n",argv[0]);
        printf("    %s -r\n",argv[0]);
    }

    close(fd);

    return 0;
}