#include "base.h"
#include "internet.h"

int SendMSG(int socket, const Package msg, int PACK_SIZE, int flags)
{
    int count = 0, len;
    do
    {
        len = send(socket, &msg, PACK_SIZE, flags);
        if (len < 0)
            return -1;
        count += len;
    }while(count < PACK_SIZE);

    return count;
}

int RecvMSG(int socket, const Package msg, int PACK_SIZE, int flags)
{
    int count = 0, len;
    do
    {
        len = recv(socket, &msg, PACK_SIZE, flags);
        if (len < 0)
            return -1;
        if (len == 0)           // 软件中断
            return 0;
        count += len;
    }while(count < PACK_SIZE);

    return count;
}
