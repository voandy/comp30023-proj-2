OBJS = crack.o sha256.o compare.o generate.o
HEADER = src/sha256.h src/compare.h src/generate.h
OUT = crack
CC = gcc
FLAGS = -c -Wall -Wpedantic #-g

all: mkbin $(OBJS)
	$(CC) bin/crack.o bin/sha256.o bin/compare.o bin/generate.o -o $(OUT)

crack.o: src/crack.c $(HEADER)
	$(CC) $(FLAGS) src/crack.c -o bin/crack.o -std=c99

sha256.o: src/sha256.c $(HEADER)
	$(CC) $(FLAGS) src/sha256.c -o bin/sha256.o -std=c99

compare.o: src/compare.c $(HEADER)
	$(CC) $(FLAGS) src/compare.c -o bin/compare.o -std=c99

generate.o: src/generate.c $(HEADER)
	$(CC) $(FLAGS) src/generate.c -o bin/generate.o -std=c99

mkbin:
	mkdir -p bin

clean:
	rm -f $(OUT) -r bin
