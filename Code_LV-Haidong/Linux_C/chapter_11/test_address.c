//可以看出,inet_addr()和inet_network()把地址255.255.255.255当成了无效的地址
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include<sys/types.h>
#include <netinet/in.h>
#include<arpa/inet.h>

int main()
{
    char buff[32];
    int ret = 0;
    int host = 0;//存主机号
    int network = 0;//存网络号
    unsigned int address = 0;//地址
    struct in_addr in; //设置IP地址

    in.s_addr = 0;
    printf("please input your ip address\n");//输入IP地址
    fgets(buff,31,stdin);
    buff[31]='\0';

    //使用inet_aton函数     将inet_aton函数转化为网络序号
    if((ret = inet_aton(buff,&in)) == 0)
    {
         printf("inet_aton : \t invalid address\n");    
    }
    else
    {
        printf("inet_aton :\t0x%x\n",in.s_addr);
    }
    
    //使用inet_addr()函数          把字符串转化为网络地址中的二进制IP地址
    if((address = inet_addr(buff)) == INADDR_NONE)//参数无效返回的值为-1
    {
             printf("inet_addr : \t invalid address \n");
    }
    else
    {
            printf("inet_addr : \t0x%lx\n",address);
    }

    //使用inet_network()函数把网络地址转化为主机字节顺序形成的二进制IP地址
    if((address = inet_network(buff)) == -1)
    {
        printf("inet_network : \t invalid address\n");
    }
    else
    {
        printf("inet_network : \t 0x%lx\n",address);
    }
    //使用inet_ntoa函数    该函数将值为in的网络字节顺序形式的二进制IP地址转换为".""分割形式
    //的字符串，成功则返回指针，无效返回NULL
    if(inet_ntoa(in) == NULL)
    {
        printf("inet_ntoa: \t invalid address \n");
    }
    else
    {
        printf("inet_ntoa : \t %s\n",inet_ntoa(in));
    }
    //示例使用inet_lnaof()与inet_netof函数
    host = inet_lnaof(in);//获取主机顺序形式的主机地址
    network = inet_netof(in);//获取主机顺序形式的网络地址
    printf("inet_lnaof:\t0x%x\n",host);
    printf("inet_netof:\t0x%x\n",network);

    //以网络字节形式所展示的network
    in = inet_makeaddr(network,host);
    printf("inet_makeaddr : 0x %x \n",in.s_addr);
    
    return 0;
}