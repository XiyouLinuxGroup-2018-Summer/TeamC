
all:
	g++ cl.cpp base.cpp view.cpp internet.cpp -pthread -lm -g -Wall
	g++ server.cpp base.cpp internet.cpp sql.cpp -lpthread -lmysqlclient -lm -o server -g -Wall

