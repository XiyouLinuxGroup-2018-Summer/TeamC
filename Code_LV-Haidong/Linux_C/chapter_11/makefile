all : client_first server_first 
.PHONY : all
client_first : my_chattingroom.h menu.h main_menu.h friend_menu.h  client_first.c
	gcc  -lpthread -o client_first  client_first.c -g  -Wall
server_first : my_chattingroom.h  server_first.c
	gcc  -lpthread -lmysqlclient -L/usr/lib64/mysql  -o server_first  server_first.c  -Wall

