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
#include <pthread.h>


#define COUNT 5
int socket_fd[COUNT]={0,-1,-1,-1,-1};
pthread_mutex_t emptymutex;

int empty=5;
/*
the fuction for the thread
*/

void pthread_function(void* clientfd){
    char message[1500];
    char buf[1024];
    int i;
    long recvbytes;
    char name[20];
    int client_fd=*(int*)clientfd;

    pthread_mutex_lock(&emptymutex);
    if(empty>0)
        empty--;
    else pthread_exit(NULL);
    pthread_mutex_unlock(&emptymutex);



    /*first connect,need to save the name of clients*/
    recvbytes=recv(client_fd,name,20,0);
    name[recvbytes]=':';
    name[recvbytes+1]='\0';
    send(client_fd,"welcome to this chatroom,enjoy chatting:",100,0);


    while(1){
        if((recvbytes=recv(client_fd,buf,1024,0))==-1){
            perror("recv");
            pthread_exit(NULL);
        }
        if(recvbytes==0){
            printf("%sbye\n",name);
            break;
        }
        buf[recvbytes]='\0';
        for (i = 0; i < COUNT; ++i)
        {
            int tmpclient=socket_fd[i];
            if(tmpclient!=-1){
                message[0]='\0';
                strcat(message,name);
                strcat(message,buf);
                if(send(tmpclient,message,strlen(message),0)==-1){
                    perror("send error");
                    pthread_exit(NULL);
                }
            }
        }
    }
    //close socket and reset the fd -1 
    close(client_fd);

    pthread_mutex_lock(&emptymutex);
    if(empty<5)
        empty++;
    else pthread_exit(NULL);
    pthread_mutex_unlock(&emptymutex);
    for (i = 0; i < COUNT; ++i)
    {
        if(socket_fd[i]==client_fd)
            socket_fd[i]=-1;
    }
    pthread_exit(NULL);
}


int main(){
    int i;
    pthread_mutex_init(&emptymutex,NULL);

    pthread_t id;
    int sockfd,client_fd;
    socklen_t sin_size;
    struct sockaddr_in my_addr;
    struct sockaddr_in remote_addr;
    if((sockfd=socket(AF_INET,SOCK_STREAM,0))==-1){
        perror("socket");
        exit(1);
    }

    my_addr.sin_family=AF_INET;
    my_addr.sin_port=htons(12363);
    my_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    bzero(&(my_addr.sin_zero),8);


    if(bind(sockfd,(struct sockaddr*)&my_addr,sizeof(struct sockaddr))==-1){
        perror("bind");
        exit(1);
    }

    if(listen(sockfd,10)==-1){
        perror("listen");
        exit(1);
    }

    i=0;
    while(1){
        sin_size=sizeof(struct sockaddr_in);
        if((client_fd=accept(sockfd,(struct sockaddr*)&remote_addr,&sin_size))==-1){
            perror("accept");
            exit(1);
        }
        if(empty>0){    
            while(socket_fd[i]==-1)
                i=(i+1)%COUNT;
            socket_fd[i]=client_fd;
            pthread_create(&id,NULL,(void*)pthread_function,(void*)&client_fd);}
        else break;
    }
    pthread_mutex_destroy(&emptymutex);
    return 0;    
}
