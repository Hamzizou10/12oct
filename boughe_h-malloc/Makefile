CC = gcc
CFLAGS = -std=c99 -pedantic -Wextra -Wall -Werror -fPIC -g

VPATH = include

SRC = malloc.c
OBJS = $(SRC:.c=.o)
BIN = malloc
LIB = libtest.so


all: $(BIN) $(LIB) 
	rm *.o

$(LIB): 
	$(CC) $(CFLAGS) -shared -fvisibility=hidden $(OBJS) -o $(LIB)
$(BIN): $(OBJS)

check: $(BIN)
	rm *.o


clean:
	$(RM) $(BIN) $(LIB)
