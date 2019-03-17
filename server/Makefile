CC=g++
LFLAGS=
CFLAGS=-c -Wall -std=c++11 -O2
OBJS=digraph.o dijkstra.o server_util.o server.o serialport.o
EXE=server

$(EXE): $(OBJS)
	$(CC) $(OBJS) -o $(EXE) $(LFLAGS)

server.o: server.cpp  digraph.h dijkstra.h wdigraph.h server_util.h
	$(CC) server.cpp -o server.o $(CFLAGS)

dijkstra.o: dijkstra.cpp dijkstra.h heap.h digraph.h wdigraph.h
	$(CC) dijkstra.cpp -o dijkstra.o $(CFLAGS)

digraph.o: digraph.cpp digraph.h
	$(CC) digraph.cpp -o digraph.o $(CFLAGS)

server_util.o: server_util.cpp server_util.h wdigraph.h
	$(CC) server_util.cpp -o server_util.o $(CFLAGS)

# server: $(OBJS)
# 	g++ $(OBJS) -o $(EXE)

server.o: serialport.h server.cpp
	g++ server.cpp -o server.o $(CFLAGS)

serialport.o: serialport.h server.cpp
	g++ serialport.cpp -o serialport.o $(CFLAGS)

clean:
	@rm -f $(OBJS) $(EXE)
