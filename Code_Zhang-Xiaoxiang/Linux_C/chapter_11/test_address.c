/**
* 6.c
* > author: PhoenixXC
* > email: xuancbm@xiyoulinux.org
* > description: test_address.c
* > created: Mon Aug 13 2018 14:02:10 GMT+0800 (CST)
* > last-modified: Mon Aug 13 2018 14:02:10 GMT+0800 (CST)
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(void)
{
    char buffer[32];
    int  ret = 0;
    int  host = 0;
    int  network = 0;
    unsigned int address = 0;
    struct in_addr in;          // sockaddr_in.sin.addr.in_addr 存储ip地址

    in.s_addr = 0;

    // 输入一个以"."分割的字符串形式的ip地址
    printf("please input your ip address:");
        fgets(buffer, 31, stdin);
    buffer[31] = '\0';

    // 使用inet_aton()函数 - 将参数cp所指向的字符串形式的ip地址转换为二进制的网络字节顺序的ip地址，存入第二个参数所指向的空间中
    // 成功返回非0值，失败返回0
    if ((ret = inet_aton(buffer, &in)) == 0)
        printf("inet_aton:\t invalid address\n");
    else
        printf("inet_aton:\t 0x%x\n", in.s_addr);

    // 使用inet_addr()函数
    if ((address = inet_addr(buffer)) == INADDR_NONE)
        printf("inet_aton:\t invalid address\n");
    else
        printf("inet_addr:\t 0x%x\n", address);

    // 使用inet_network()函数 - 将参数所指向的字符串形式的网络地址转换成为以主机字节顺序形式的二进制ip地址，执行成功返回转换后的结果，参数无效返回-1
    if ((address = inet_network(buffer)) == -1)
        printf("inet_network:\t invalid address\n");
    else
        printf("inet_network:\t0x%x\n", address);

    // 使用inet_ntoa()函数 - 将值为in的网络字节顺序形式的二进制ip地址转化为以"."分割的字符串形式，执行成功返回结果字符串的指针，参数无效返回NULL
    if (inet_ntoa(in) == NULL)
        printf("inet_ntoa:\t invalid address\n");
    else
        printf("inet_ntoa:\t %s\n", inet_ntoa(in));

    // 使用inet_lnaof - 从参数in中提取出主机地址，执行成功返回主机字节顺序形式的主机地址
    // 使用inet_netof - 从参数in中提取出网络地址，执行成功返回主机字节顺序形式的网络地址
    host = inet_lnaof(in);
    network = inet_netof(in);
    printf("inet_lnaof:\t0x%x\n", host);
    printf("inet_netof:\t0x%x\n", network);

    // inet_makeaddr - 把网络号和主机号合成一个地址
    in = inet_makeaddr(network, host);
    printf("inet_makeaddr:\t0x%x\n", in.s_addr);

    return 0;
}
