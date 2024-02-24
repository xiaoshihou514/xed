#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifndef _RESIZABLE_BUFFER_C
#define _RESIZABLE_BUFFER_C

typedef struct {
    char *data;
    int size;
    int used;
} ResizableBuffer;

const size_t INITIAL_SIZE = 1024;

ResizableBuffer rbuf_new(void) {
    return (ResizableBuffer){
        .data = malloc(INITIAL_SIZE), .size = INITIAL_SIZE, .used = 0};
}

void rbuf_append(ResizableBuffer *rbuf, const char *contents, size_t len) {
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

void rbuf_flush(ResizableBuffer *rbuf) {
    write(STDOUT_FILENO, rbuf->data, rbuf->used);
    // we don't need to clean the buffer because when we write we only write
    // that many bytes
    rbuf->used = 0;
}

void rbuf_ensure_space(ResizableBuffer *rbuf, int required) {
    while (rbuf->size - rbuf->used < required) {
        rbuf->data = realloc(rbuf->data, rbuf->size * 2);
    }
}

void rbuf_write_move_cursor_command(ResizableBuffer *rbuf, int row, int col) {
    rbuf_ensure_space(rbuf, 16);
    sprintf(rbuf->data, "\x1b[%d;%dH", row, col);
}

void rbuf_free(ResizableBuffer *rbuf) { free(rbuf->data); }

void rbuf_lazy_clear(ResizableBuffer *rbuf) { rbuf->used = 0; }

#endif
