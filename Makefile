all:
	gcc -Wall -c main.c
	gcc -Wall -Wextra -std=c11 -ggdb3 main.o -o tp2virtual

clean:
	rm tp2virtual
