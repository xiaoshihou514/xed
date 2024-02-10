.RECIPEPREFIX = "    "
main: main.c
    $(CC) main.c -o xed -Wall -Wextra -pedantic -std=c99
