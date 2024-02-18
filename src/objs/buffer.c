#include "stddef.h"
#include <stdlib.h>
#include <string.h>

#ifndef _BUFFER_C
#define _BUFFER_C

// BUG: almost certainly have UTF-8 related bugs
// we use size_t here because it's more portable
// on 64 bit machines they are both 8 bytes
struct GapBuffer {
    char *data;
    size_t gap_start;
    size_t gap_end;
    size_t size;
};

struct Buffer {
    // RW buffers
    struct GapBuffer *gb;
    // RO buffers
    char **lines;
};

// TODO: insertion, get lines, etc.

// Gap buffer related functions
struct GapBuffer gap_buffer_new(size_t size) {
    return (struct GapBuffer){malloc(size), 0, size, size};
}

void gap_buffer_shift_cursor(struct GapBuffer *gb, size_t dest) {
    size_t gap_len = gb->gap_end - gb->gap_start;
    // TODO: why don't I need a reallocation here?
    // can't move outta bounds!
    dest = (dest < gb->size - gap_len) ? dest : gb->size - gap_len;

    if (dest == gb->gap_start)
        return;
    if (gb->gap_start < dest) {
        // gap starts before dest
        // move some contents after the gap backwards so that gap_start is the
        // destination specified
        // before:
        // [12]|          [34567]
        //     <-- gap -->
        //       | <- want to move cursor here
        //
        // after:
        // [1234]|          [567]
        //       <-- gap -->
        size_t delta = dest - gb->gap_start;
        memcpy(gb->data + gb->gap_start, gb->data + gb->gap_end, delta);
        gb->gap_start += delta;
        gb->gap_end += delta;
    } else {
        // gap starts after dest
        // similarly:
        // before:
        // [12]|          [34567]
        //     <-- gap -->
        //   | <- want to move cursor here
        //
        // after:
        // [1]|           [234567]
        //     <-- gap -->
        size_t delta = gb->gap_start - dest;
        memcpy(gb->data + gb->gap_end - delta, gb->data + dest, delta);
        gb->gap_start -= delta;
        gb->gap_end -= delta;
    }
}

// TODO
void gap_buffer_ensure_gap_size(struct GapBuffer *gb, size_t size);

// insert in the gap buffer
void gap_buffer_insert(struct GapBuffer *gb, size_t location, char c) {
    gap_buffer_ensure_gap_size(gb, 1);
    gap_buffer_shift_cursor(gb, location);
    gb->data[gb->gap_start] = c;
    gb->gap_start++;
}

void gap_buffer_free(struct GapBuffer *gb) { free(gb->data); }

// buffer related functions
// returns a writable buffer
struct Buffer buffer_new() {
    // this is a damn scratch buffer, it shouldn't need to to be too big
    struct GapBuffer gb = gap_buffer_new(4096);
    return (struct Buffer){&gb, NULL};
}

void buffer_free(struct Buffer *buffer) {
    if (buffer->gb != NULL) {
        free(buffer->gb);
    } else {
        // TODO: not implemented yet
    }
}

#endif
