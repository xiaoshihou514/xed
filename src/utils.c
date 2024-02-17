#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#ifndef _UTIL_H
#define _UTIL_H

void panic(const char *reason) {
    // clear screen first
    write(STDOUT_FILENO, "\x1b[2J", 4);
    write(STDOUT_FILENO, "\x1b[H", 3);

    // complain loudly and exit
    perror(reason);
    exit(-1);
}

#endif
