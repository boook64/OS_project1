all: main

process.o: process.c scheduler.h
	gcc -std=c11 -W -Wall -Wextra -g -O3 process.c -c -o process.o

main: scheduler.c scheduler.h process.o 
	gcc -std=c11 -W -Wall -Wextra -g -O3 scheduler.c process.o -o main

clean:
	rm process.o main 
