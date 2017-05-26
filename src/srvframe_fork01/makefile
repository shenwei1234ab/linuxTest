.PHONY:clean all
CC=gcc
CFLAGS=-Wall -g
BIN=cli4 srv4
all:$(BIN)
%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@
	
cli4:cli4.o commsocket.o
	$(CC) $(CFLAGS) $^ -o $@ 
	
srv4:srv4.o commsocket.o
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f *.o $(BIN)




