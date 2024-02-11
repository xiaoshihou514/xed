main: src/main.c
	$(CC) src/main.c -o xed -Wall -Wextra -pedantic -std=c99
debug: src/main.c
	$(CC) src/main.c -g -Og -o xed -Wall -Wextra -pedantic -std=c99
