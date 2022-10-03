#define _GNU_SOURCE
#include <sched.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <malloc.h>
#include <sys/types.h>
#include <sys/wait.h>
/*
int param = 0;

int thread_func(void *data)
{
    int i;
    printf("starting chid thread_fn,pid = %d\n",getpid());
    for(i = 0;i < 10;i++) {
        param = i + 1000;
        sleep(1);
        printf("child thread running:i = %d,param = %d secs\n",i,param);
    }
    printf("child thread_fn exit\n");
    return 0;
}
int main (int argc,char **argv)
{
    int j,tid,pagesize,stacksize;
    void *stack;

    printf("starting parent process,pid = %d\n",getpid());

    pagesize = getpagesize();
    stacksize = 4 * pagesize;

    posix_memalign(&stack,pagesize,stacksize);

    printf("Setting a clone child thread with stacksize = %d...",stacksize);
    tid  = clone(thread_func,(char *)stack + stacksize,CLONE_VM | SIGCHLD,0);
    printf("with tid = %d\n",tid);
    if(tid < 0)
        exit(EXIT_FAILURE);

    for(j = 0; j < 6;j++) {
        param = j;
        sleep(1);
        printf("parent running:j = %d,param = %d secs\n",j,param);
    }
    printf("parent killitself\n");
    exit(EXIT_FAILURE);
}   
*/

int main(int argc,char **argv)
{
    int i,tid;
    printf("curent_pid = %d\n",getpid());
    for(i = 0;i < 2;i++)
    {
        tid = fork();
        if(tid == 0)
            printf("child:pid = %d_\n",getpid());
        else if(tid > 0)
            printf("parent:child_pid = %d_,parent_pid = %d\n",tid,getpid());
        else
            exit(EXIT_FAILURE);
    }
    wait(NULL);
    wait(NULL);

    return 0;
}