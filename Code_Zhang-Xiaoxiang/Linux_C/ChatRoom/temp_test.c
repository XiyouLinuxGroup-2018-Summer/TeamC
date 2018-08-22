#include <stdio.h>
#include <string.h>
#define _END_ "\r\t\n"

int main() 
{
    char * str1, * str2, *str3, *str4;
    char username[12 + 1], pass[12 + 1], question[16 + 1], answer[17 + 1];
    char strmsg[100] = "xuanc\r\t\nbming\r\t\ntest question\r\t\ntest answer\r\t\n";
    str1 = strstr(strmsg, _END_);
    str2 = strstr(str1 + strlen(_END_), _END_);
    str3 = strstr(str2 + strlen(_END_), _END_);
    str4 = strstr(str3 + strlen(_END_), _END_);
    strncpy(username, strmsg, str1 - strmsg);
    username[str1 - strmsg] = '\0';
    strncpy(pass, str1 + strlen(_END_), str2 - str1 - strlen(_END_));
    pass[str2 - str1 - strlen(_END_)] = '\0';
    strncpy(question, str2 + strlen(_END_), str3 - str2 - strlen(_END_));
    question[str3 - str2 - strlen(_END_)] = '\0';
    strncpy(answer, str3 + strlen(_END_), str4 - str3 - strlen(_END_));
    answer[str4-str3 - strlen(_END_)] = '\0';

    return 0;
}
