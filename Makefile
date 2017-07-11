CC = gcc
CFLAGS = -Wall

all : power_up

power_up : ./src/libpower.h ./src/libpower.c ./src/main.c
	$(CC) -c -g -pg  ./src/libpower.c -o ./src/libpower.o $(CFLAGS)
	$(CC) -c -g -pg ./src/main.c -o ./src/main.o $(CFLAGS)
	$(CC) -g -pg ./src/main.o ./src/libpower.o -o power_up $(CFLAGS)


clean :
	rm -f *.o power_up *.c~
	rm ./src/*.o

install :
	sudo cp `sudo find ~ -name "power_up" | grep bin/power_up` -uv /etc/bash_completion.d/
