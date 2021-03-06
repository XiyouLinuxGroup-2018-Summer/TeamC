#include "base.h"
#include "internet.h"

int SendMSG(int socket, const Package* msg, int pSize, int flags)
{
    int count = 0, len = 0;
    const char * ptr  = (const char *)msg ;
    do
    {
        len = send(socket, (void *)ptr, pSize - count, flags);
        if (len < 0)
        {
            if (errno == EINTR)
                len = 0;
            else    
                return -1;
        }
        count += len;
        ptr += len;
    } while(count < pSize);

    return count;
}

int RecvMSG(int socket, Package* msg, int pSize, int flags)
{
    int count = 0, len = 0;
    char * ptr = (char *)msg;
    do
    {
        len = recv(socket, (void *)ptr, pSize - count, flags);

        if (len < 0)
        {
            if (errno == EINTR)
                len = 0;
            else
                return -1;
        }
        else if (len == 0)
        {
            return 0;
        }
        count += len;
        ptr += len;
    } while (count < pSize);

    return count;
}

