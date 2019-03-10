CC=g++
CFLAGS=-c -Wall -O2 -std=c++11
LFLAGS=
PROGS= server
OBJS= server.o digraph.o dijkstra.o

# executable targets
all: server

server: server.o digraph.o dijkstra.o
	$(CC) server.o dijkstra.o digraph.o -o server $(LFLAGS)

# object targets
server.o: server.cpp digraph.h dijkstra.h heap.h wdigraph.h
	$(CC) server.cpp -o server.o $(CFLAGS)

dijkstra.o: dijkstra.cpp dijkstra.h digraph.h heap.h wdigraph.h
	$(CC) dijkstra.cpp -o dijkstra.o $(CFLAGS)

digraph.o: digraph.cpp digraph.h
	$(CC) digraph.cpp -o digraph.o $(CFLAGS)

clean:
	@rm $(OBJS) $(PROGS)
