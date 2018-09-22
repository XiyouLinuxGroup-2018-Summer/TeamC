all:
	g++ ./Client/cl.cpp ./Base/base.cpp ./Client/view.cpp ./Base/internet.cpp -pthread -lm -g -Wall -o client
	g++ ./Server/server.cpp ./Base/base.cpp ./Base/internet.cpp ./Server/sql.cpp -lpthread -lmysqlclient -lm -o server -g -Wall

