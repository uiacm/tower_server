BIN = towerserver
CC = gcc
CFLAGS = -Wall -pthread -DUSE_WEBSOCKET `pkg-config --cflags jansson` -ggdb
CLIBS = -ldl `pkg-config --libs jansson` -lwiringPi
OBJS = mongoose.o driver.o sendmessage.o towerlights.o
SRCS = towerserver.c mongoose.c mongoose.h driver.c driver.h gamma.h sendmessage.c sendmessage.h towerlights.c towerlights.h

all: $(BIN).c $(BIN).o $(OBJS) $(SRCS)
	$(CC) $(CFLAGS) -o $(BIN) $(BIN).o $(OBJS) $(CLIBS)

clean:
	rm -f $(BIN) $(BIN).o $(OBJS)
