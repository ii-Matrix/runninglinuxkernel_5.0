#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <errno.h>

int main(int argc,char *argv[])
{
    pid_t my_pid;
    int old_prio,new_prio,i,rc;

    if(argc > 1) {
        my_pid = atoi(argv[1]);
    } else {
        my_pid = getpid();
    }

    printf("\n Examining priorities for PID = %d\n",my_pid);
    printf("%10s%10s%10s\n","Previous","Requested","Assigned");

    for(i = -20;i < 20;i+=2) {

        old_prio = getpriority(PRIO_PROCESS,(int)my_pid);
        rc = setpriority(PRIO_PROCESS,(int)my_pid,i);
        if(rc)
            fprintf(stderr,"setpriority() failed");

        errno = 0;
        new_prio = getpriority(PRIO_PROCESS,(int)my_pid);
        printf("%10d%10d%10d\n",old_prio,i,new_prio);
    }

    exit(EXIT_SUCCESS);
}