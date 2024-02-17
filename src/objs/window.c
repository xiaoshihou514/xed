#include "buffer.c"

#ifndef _WINDOW_C
#define _WINDOW_C

struct Window {
    struct Buffer *buffer;
    int anchor_row;
    int anchor_col;
    int height;
    int width;
    int rowoff;
    int coloff;
    int cursor_row;
    int cursor_col;
};

struct Window scratch_win(int term_width, int term_height) {
    struct Buffer buffer = buffer_new();
    // clang-format off
    return (struct Window) {
        &buffer,
        1,
        1,
        term_height,
        term_width,
        0,
        0,
        5,
        5,
    };
    // clang-format on
}

#endif
