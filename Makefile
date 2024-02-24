.PHONY: main debug run

CC=gcc

main:
	gcc src/main.c -o xed -Wall -Wextra -pedantic -std=c2x

debug:
	gcc src/main.c -g -O0 -o xed -Wall -Wextra -pedantic -std=c2x
	gdb ./xed

run: main
	./xed
