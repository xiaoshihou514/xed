#include "stdio.h"
#include "typedefs.h"

// HACK: temporary

// clang-format off
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
struct View view = {
    {
        &n,&n
    },
    &win
};
// clang-format on
