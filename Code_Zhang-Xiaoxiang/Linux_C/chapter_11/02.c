#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/un.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

char recv_buf[1500],send_buf[1024];

void pthread_function(void* sock_fd){
    int sockfd=*(int*)sock_fd;
    long recvbytes;
    while(1) {
        if((recvbytes=recv(sockfd,recv_buf,1500,0))==-1){
            printf("recv error");
            exit(1);
        }
        else{
            recv_buf[recvbytes]='\0';
            printf("%s\n",recv_buf);
        }
    }
} 
int main(void){
    pthread_t id;
    int sockfd;
    struct sockaddr_in sever_addr;

    sever_addr.sin_family=AF_INET;
    sever_addr.sin_port=htons(12363);
    sever_addr.sin_addr.s_addr=inet_addr("127.0.0.1");

    if((sockfd=socket(AF_INET,SOCK_STREAM,0))==-1){
        printf("socket error");
        exit(1);
    }
    if(connect(sockfd,(struct sockaddr*)&sever_addr,sizeof(sever_addr))==-1){
        printf("connect error");
        exit(1);
    }
    char name[20];
    printf("please input your name\n");
    scanf("%s",name);
    send(sockfd,name,strlen(name),0);

    if(pthread_create(&id,NULL,(void*)pthread_function,(void*)&sockfd)!=0)
        printf("create thread error\n");
    while(1){
        scanf("%s",send_buf);
        fflush(stdin);
        if(send(sockfd,send_buf,strlen(send_buf),0)==-1){
            printf("send error");
            exit(1);
        }
        sleep(1);
    }
    close(sockfd);
    pthread_cancel(id);
    return 0;
}
