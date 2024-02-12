#include "stdio.h"
#include "typedefs.h"

#ifndef GLOBALS_H
#define GLOBALS_H

// clang-format off
// HACK: temporary
struct Window win = {
    1,
    1,
    34,
    147,
    10,
    10,
    0,
    0,
    NULL,
};
struct Node n = {&win,NULL,NULL};

// actual variables that would be helpful
struct View view = {
    {
        &n,&n
    },
    &win
};
// clang-format on
struct LinkedList buffers = {NULL, NULL};

// 0: normal
// 1: insert
int mode = 0;
#endif
