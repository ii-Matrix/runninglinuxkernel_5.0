#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <malloc.h>

#define DEMO_DEV_NAME "/dev/my_demo_drv"
#define MYDEV_GET_BUFSIZE 1

int main()
{
    int fd;
    int i;
    size_t len;
    //char message[] = " Testing the virtual FIFO device";
    //char *read_buffer,*mmap_buffer;
    char *read_buffer,*write_buffer;

    //len = sizeof(message);
    fd = open(DEMO_DEV_NAME,O_RDWR);
    if(fd < 0) {
        printf("open device %s failed\n",DEMO_DEV_NAME);
        return -1;
    }

    if(ioctl(fd,MYDEV_GET_BUFSIZE,&len) < 0) {
        printf("ioctl fail\n");
        goto open_fail;
    }

    printf("driver max buffer size = %d\n",len);

	read_buffer = mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, -1, 0);
	if (read_buffer == MAP_FAILED)
		goto open_fail;

	write_buffer = mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, -1, 0);
	if (write_buffer == MAP_FAILED)
		goto buffer_fail;
        /*
    read_buffer = malloc(len);
    if(!read_buffer)
        goto open_fail;
    
    /*mmap_buffer = mmap(NULL,len,PROT_READ | PROT_WRITE,MAP_SHARED,fd,0);
    if(mmap_buffer == (char *)MAP_FAILED) {
        printf("mmap driver buffer fail\n");
        goto map_fail;
    }*/
    /*
    write_buffer = malloc(len);
    if(!write_buffer)
        goto buffer_fail; */

    //printf("mmap driver buffer succeeded:%p\n",mmap_buffer);

    for(i = 0;i < len;i++)
       // *(mmap_buffer + i) = (char)random();
       *(write_buffer + i) = 0x55;

    if(write(fd,write_buffer,len) != len) {
        printf("write fail\n");
        goto rw_fail;
    }

    if(read(fd,read_buffer,len) != len) {
        printf("read fail\n");
        goto rw_fail;
    }

    if(memcmp(write_buffer,read_buffer,len)) {
        printf("buffer compare fail\n");
        goto rw_fail;
    }
    printf("data modify and compare succussful\n");

    //munmap(mmap_buffer,len);
    free(write_buffer);
    free(read_buffer);
    close(fd);

    return 0;

rw_fail:
    //munmap(mmap_buffer,len);
    if(write_buffer)
        free(write_buffer);
buffer_fail:
    if(read_buffer)
        free(read_buffer);
open_fail:
    close(fd);
    return -1;
}