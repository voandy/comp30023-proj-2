OBJS = crack.o sha256.o compare.o
HEADER = src/sha256.h src/compare.h
OUT = crack
CC = gcc
FLAGS = -c -Wall -Wpedantic -std=c99 #-g

all: mkbin $(OBJS)
	$(CC) bin/crack.o bin/sha256.o bin/compare.o -o $(OUT)

crack.o: src/crack.c $(HEADER)
	$(CC) $(FLAGS) src/crack.c -o bin/crack.o

sha256.o: src/sha256.c $(HEADER)
	$(CC) $(FLAGS) src/sha256.c -o bin/sha256.o

compare.o: src/compare.c $(HEADER)
	$(CC) $(FLAGS) src/compare.c -o bin/compare.o

mkbin:
	mkdir -p bin

clean:
	rm -f $(OUT) -r bin
