#include <stdio.h>

struct Buffer {
    FILE file;
    char *name;
    GapBuffer data;
};
