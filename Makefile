.PHONY: main debug run

main:
	gcc src/main.c -o xed -Wall -Wextra -pedantic -std=c2x

debug:
	gcc src/main.c -g -O0 -o xed -Wall -Wextra -pedantic -std=c2x
	gdb ./xed
check:
	scan-build clang src/main.c

run: main
	./xed
