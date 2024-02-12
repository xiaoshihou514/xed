.PHONY: main debug run

main:
	$(CC) src/main.c -o xed -Wall -Wextra -pedantic -std=c99

debug:
	$(CC) src/main.c -g -Og -o xed -Wall -Wextra -pedantic -std=c99

run: main
	./xed
