all : client_first server_first 
.PHONY : all
client_first : my_chattingroom.h menu.h main_menu.h friend_menu.h  my_send_recv.h  client_first.c
	gcc  -lpthread -o client_first  client_first.c -g  
server_first : my_chattingroom.h    my_send_recv.h   server_first.c
	gcc  -lpthread -lmysqlclient -L/usr/lib64/mysql  -o server_first  server_first.c 

