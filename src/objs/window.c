#include "buffer.c"
#include "line_segment.c"
#include "linked_list.c"
#include "resizable_buffer.c"
#include <stdbool.h>

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

bool win_crosses_line(struct Window *win, int line) {
    return win->anchor_row <= line && win->anchor_row + win->height >= line;
}

void window_render_segment(struct Window *win, struct ResizableBuffer *rbuf,
                           int line, struct LineSeg *seg) {
    // TODO: wcwidth, mbrtowc?
}

void window_render_segments(struct Window *win, struct ResizableBuffer *rbuf,
                            int line, struct LinkedList *segments) {
    if (!win_crosses_line(win, line)) {
        return;
    }
    struct Node *node = segments->head;
    while (node != NULL) {
        struct LineSeg *seg = node->val;
        window_render_segment(win, rbuf, line, seg);
        node = node->next;
    }
}

#endif
