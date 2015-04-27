CC=gcc
CFLAGS=-Werror -Wall
LFLAGS=-lpthread -lm
EXEC=programme

all:$(EXEC)

programme: programme.o
	$(CC) programme.o -o programme $(CFLAGS) $(LFLAGS)

programme.o: programme.c
	$(CC) -c programme.c -o programme.o $(CFLAGS) $(LFLAGS)

osef: programme.c
	gcc -c programme.c -o programme.o -lpthread
	gcc programme.o -o programme -lpthread

