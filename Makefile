CC=gcc

all: main

main: main.c hashmap.o
	$(CC) -c main.c -o main.o

hashmap.o: hashmap.c
	$(CC) -c hashmap.c -o hashmap.o

clean:
	rm hashmap.o main.o
