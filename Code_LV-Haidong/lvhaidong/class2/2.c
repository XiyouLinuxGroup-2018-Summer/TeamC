#include<stdio.h>
#include<unistd.h>
int main()
{
    for(int i = 0;i< 2; i++)
    {
        pid_t pid = fork();
        printf("hell0,word\n");
    }
    return 0;
}
//一共执行六次hello,word
//在产生第一个子进程的同时,父进程执行了一次printf
//子进程执行了一次printf,
//第二次执行时，然后之前的父进程再执行一次，子进程在执行一次,新产生的再执行两次
//第二次产生了两个子进程
//每次fork会产生一个子进程,再加上原本的进程应该是4个
//一共产生了共4个进程
