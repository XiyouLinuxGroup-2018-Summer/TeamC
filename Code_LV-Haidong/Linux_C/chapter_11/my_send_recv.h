#pragma once
#ifndef _MY_SEND_RECV_H
#define _MY_SEND_RECV_H

#include "my_chattingroom.h"


int sendn(int sockfd,void * buf,int len,int flags)
{
    int n,sum = 0;
    while((n = send(sockfd,buf,len,flags)) && (n != len) )
    {
         sum +=n;
         if(sum == len)
         {
            return sum;
         }
    }
    sum = n;
    return sum ;
}

int recvn(int sockfd,void *buf,int len,int flags)
{
    int n,sum = 0;
    while((n= recv(sockfd,buf,len,flags)) && (n != len))
    {
            sum += n;
            if(sum == len)
            {
                return sum;
            }
    }
    sum = n;
    return sum;
}


#endif