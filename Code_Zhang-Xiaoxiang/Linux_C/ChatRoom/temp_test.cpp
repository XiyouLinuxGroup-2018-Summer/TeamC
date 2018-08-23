#include <bits/stdc++.h>
#include <unistd.h>

#define MSG_SIZE 512
using namespace std;

typedef struct message                  // 消息结构
{       
    char msgtime[55];                   // 存放发送消息的时间
    int sourceid;                       // 标记消息来源，相同来源的消息放入同一个vector    
}Message;

struct cmp
{
    bool operator()(vector<Message>&a, vector<Message>&b) const
    {
        return strcmp(a.back().msgtime, b.back().msgtime);
    }
};
priority_queue<vector<Message>, vector<vector<Message>>, cmp>MessageBox;    

// fun: 获取当前时间
void get_time(char * string)
{
    time_t now;                 
    struct tm * timenow;        
    time(&now);                 
    timenow = localtime(&now);
    char temptime[55];
    sprintf(temptime, "%d-%02d-%02d %02d:%02d:%02d", timenow->tm_year + 1900, timenow->tm_mon + 1, timenow->tm_mday, timenow->tm_hour, timenow->tm_min, timenow->tm_sec);
    if (string == NULL)
        printf("%s\n", temptime);
    else
        strcpy(string, temptime);
}

int main() 
{
    while (!MessageBox.empty())
        MessageBox.pop();

    for (int i = 1; i < 10; i++)
    {
        Message m;
        m.sourceid = i;
        get_time(m.msgtime);
        vector<Message>temp;
        temp.clear();
        temp.push_back(m);
        MessageBox.push(temp);
        sleep(1);
    }

    while (!MessageBox.empty())
    {
        vector<Message>temp;
        temp = MessageBox.top();
        int count = 0;
        int k = temp.size();
        while (count < k)
        {
            printf("%d | %s\n", temp[count].sourceid, temp[count].msgtime);
            count++;
        }
        MessageBox.pop();
    }

    return 0;
}
