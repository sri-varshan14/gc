CC=gcc
CFLAGS=-Wall -Wextra -pedantic -std=c99 -ggdb

alloc: main.c
	${CC} ${CFLAGS} $< -o $@

run: alloc
	./alloc

clean:
	rm alloc