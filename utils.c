#include <stdio.h>
#include <stdlib.h>
void panic(const char *reason) {
    perror(reason);
    exit(-1);
}
