CC=gcc
CFLAGS=-Werror -Wall
LFLAGS=-lpthread -lm -lcurl
EXEC=programme
FILENAME=ProgFinal

all:$(EXEC)

programme: $(FILENAME).o
	$(CC) $(FILENAME).o -o $(EXEC) $(CFLAGS) $(LFLAGS)

programme.o: $(FILENAME).c
	$(CC) -c $(FILENAME).c -o $(FILENAME).o $(CFLAGS) $(LFLAGS)

osef: $(FILENAME).c
	gcc -c $(FILENAME).c -o $(FILENAME).o -lpthread -lm
	gcc $(FILENAME).o -o $(FILENAME) -lpthread -lm

