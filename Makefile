CC = gcc
CFLAGS = -Wall

all : power-up

power-up : ./src/libpower.h ./src/libpower.c ./src/main.c
	$(CC) -c  ./src/libpower.c -o ./src/libpower.o $(CFLAGS)
	$(CC) -c  ./src/main.c -o ./src/main.o $(CFLAGS)
	$(CC) ./src/main.o ./src/libpower.o -o power-up $(CFLAGS)


clean :
	rm -f *.o power-up *.c~
	rm ./src/*.o
