#pragma once
#ifndef _TEST_H
#define _TEST_H
#define BUFSIZE 1024

void my_err(const char * err_string, int line);
int my_recv(int conn_fd, char * data_buf, int len);
void get_time();

#endif
