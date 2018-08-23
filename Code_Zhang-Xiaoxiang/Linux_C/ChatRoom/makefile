
all:
	g++ client.cpp base.cpp view.cpp internet.cpp -pthread -lm -g 
	g++ server.cpp base.cpp internet.cpp sql.cpp -lpthread -lmysqlclient -lm -o server -g
