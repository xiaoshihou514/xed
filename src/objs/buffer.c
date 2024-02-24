#include "resizable_buffer.c"
#include "stddef.h"
#include <stdlib.h>
#include <string.h>

#ifndef _BUFFER_C
#define _BUFFER_C

// BUG: almost certainly have UTF-8 related bugs
// we use size_t here because it's more portable
// on 64 bit machines they are both 8 bytes
typedef struct {
    char *data;
    size_t start_offset;
    size_t end_offset;
    size_t size;
} GapBuffer;

// originally I thought maybe we could differentiate between RW
// buffers and RO buffers, but since you would need set_text
// and set_line for them anyway it seems redundant
typedef struct {
    GapBuffer *gb;
} Buffer;

// TODO: insertion, get lines, etc.

// Gap buffer related functions
GapBuffer gap_buffer_new(size_t size) {
    return (GapBuffer){.data = malloc(size),
                       .start_offset = 0,
                       .end_offset = size,
                       .size = size};
}

void gap_buffer_shift_cursor(GapBuffer *gb, size_t dest) {
    size_t gap_len = gb->end_offset - gb->start_offset;
    // TODO: why don't I need a reallocation here?
    // can't move outta bounds!
    dest = (dest < gb->size - gap_len) ? dest : gb->size - gap_len;

    if (dest == gb->start_offset)
        return;
    if (gb->start_offset < dest) {
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
        size_t delta = dest - gb->start_offset;
        memcpy(gb->data + gb->start_offset, gb->data + gb->end_offset, delta);
        gb->start_offset += delta;
        gb->end_offset += delta;
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
        size_t delta = gb->start_offset - dest;
        memcpy(gb->data + gb->end_offset - delta, gb->data + dest, delta);
        gb->start_offset -= delta;
        gb->end_offset -= delta;
    }
}

void gap_buffer_ensure_gap_size(GapBuffer *gb, size_t size) {
    size_t gap_end = gb->end_offset;
    size_t gap_size = gap_end - gb->start_offset;
    if (gap_size < size) {
        size_t total_size = gb->size;
        while (gap_size < size) {
            gap_size += total_size;
            total_size *= 2;
        }
        gb->data = realloc(gb->data, total_size);

        // before:
        // [12]    [7890]
        //         ^
        //         gap_end
        // after:
        //    gb->data + gap_start + gap_size
        //				    v
        // [12]             [7890]
        //         ^-------^ newly allocated
        //    gap_start + gap_size

        memcpy(gb->data + gap_end, gb->data + gb->start_offset + gap_size,
               gb->size - gap_end);

        // update gap_end and size
        gb->end_offset = gb->start_offset + gap_size;
        gb->size = total_size;
    }
}

// insert in the gap buffer
void gap_buffer_insert(GapBuffer *gb, size_t location, char *data,
                       size_t length) {
    gap_buffer_shift_cursor(gb, location);
    gap_buffer_ensure_gap_size(gb, length);
    memcpy(data, gb->data + gb->start_offset + location, length);
    gb->start_offset += length;
}

void gap_buffer_free(GapBuffer *gb) { free(gb->data); }

// buffer related functions
// returns a writable buffer
Buffer buffer_new(void) {
    // this is a damn scratch buffer, it shouldn't need to to be too big
    GapBuffer gb = gap_buffer_new(4096);
    return (Buffer){.gb = &gb};
}

void buffer_free(Buffer *buffer) { free(buffer->gb); }

wchar_t *buffer_get_line(Buffer *buf, int row, size_t *length) {
    size_t count = 0;
    size_t line_count = 0;
    size_t mark = 0;
    bool crosses_gap = false;
    GapBuffer *gb = buf->gb;

    while (line_count < row && count < gb->size) {
        if (gb->data[count] == '\n') {
            line_count++;
            mark = count;
        }
        if (count == gb->start_offset)
            count = gb->end_offset;
        else
            count++;
    }
    if (line_count != row || count == mark)
        // nothing here!
        return nullptr;
    size_t size = count - mark;
    wchar_t *linebuf = malloc(size);
    // don't forget \0!
    // TODO: this almost certainly does not work either
    if (crosses_gap) {
        size_t len = gb->start_offset - mark;
        memcpy(linebuf, gb->data + mark, len);
        memcpy(linebuf + len, gb->data + gb->end_offset, size - len);
    } else {
        memcpy(linebuf, gb->data + mark, size);
    }
    linebuf[size - 1] = '\0';
    *length = size;
    return linebuf;
}

// gets the byte position of the desired line in the gap buffer
void buffer_get_line_indices(Buffer *buf, int row);

#endif
