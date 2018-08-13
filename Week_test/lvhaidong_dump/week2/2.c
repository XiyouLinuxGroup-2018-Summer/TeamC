#include<stdio.h>
#include<unistd.h>
int main()
{
    for(int i = 0;i< 1; i++)
    {
        pid_t pid = fork();
        printf("hell0,word");
    }
    return 0;
}
//一共执行八次hello,word，因为printf的换行符是为了清除缓存，没有的话，第一次循环的时候，产生的两个进程会保存第一次的hello,word
//在产生第一个子进程的同时,父进程执行了一次printf
//子进程执行了一次printf,
//第二次执行时，然后之前的父进程再执行一次，子进程在执行一次,新产生的再执行两次
//第二次产生了两个子进程
//每次fork会产生一个子进程,再加上原本的进程应该是4个
//一共产生了共4个进程
