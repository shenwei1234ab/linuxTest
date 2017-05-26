.PHONY:clean all
CC=gcc
CFLAGS=-Wall -g
BIN=cli3 srv3
all:$(BIN)
%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@
	
cli3:cli3.o commsocket.o
	$(CC) $(CFLAGS) $^ -o $@ 
	
srv3:srv3.o commsocket.o
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f *.o $(BIN)




