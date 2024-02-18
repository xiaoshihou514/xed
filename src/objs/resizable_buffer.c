#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifndef _RESIZABLE_BUFFER_C
#define _RESIZABLE_BUFFER_C

struct ResizableBuffer {
    char *data;
    int size;
    int used;
};

struct ResizableBuffer rbuf_new() {
    return (struct ResizableBuffer){malloc(1024), 1024, 0};
}

void rbuf_append(struct ResizableBuffer *rbuf, const char *contents) {
    int len = strlen(contents);
    int needed_size = len + rbuf->used;
    if (needed_size > rbuf->size) {
        int size = rbuf->size;
        // try to allocate 2^n bytes
        while (size < needed_size)
            size *= 2;
        rbuf->data = realloc(rbuf->data, needed_size);
        rbuf->size = needed_size;
    }
    memcpy(rbuf + rbuf->used, contents, len);
    rbuf->used += len;
}

void rbuf_flush(struct ResizableBuffer *rbuf) {
    write(STDIN_FILENO, rbuf->data, rbuf->used);
    // we don't need to clean the buffer because when we write we only write
    // that many bytes
    rbuf->used = 0;
}

void rbuf_ensure_space(struct ResizableBuffer *rbuf, int required) {
    while (rbuf->size - rbuf->used < required) {
        rbuf->data = realloc(rbuf->data, rbuf->size * 2);
    }
}

void rbuf_write_move_cursor_command(struct ResizableBuffer *rbuf, int row,
                                    int col) {
    rbuf_ensure_space(rbuf, 16);
    sprintf(rbuf->data, "\x1b[%d;%dH", row, col);
}

void rbuf_free(struct ResizableBuffer *rbuf) { free(rbuf->data); }

#endif
