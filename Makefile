.PHONY: main debug run

main:
	gcc src/main.c -o xed -Wall -Wextra -pedantic -std=c2x

debug:
	gcc src/main.c -g -Og -o xed -Wall -Wextra -pedantic -std=c2x
check:
	scan-build clang src/main.c
	splint src/main.c | less

run: main
	./xed
