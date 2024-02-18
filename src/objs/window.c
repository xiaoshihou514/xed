#include "buffer.c"
#include "line_segment.c"
#include "linked_list.c"
#include "resizable_buffer.c"
#include <stdbool.h>
#include <stdio.h>

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
    // TODO: statuscol
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

void win_render(struct Window *win, struct ResizableBuffer *rbuf,
                bool *occupied, int term_height, int term_width) {
    int row_start = win->anchor_row;
    int row_end = win->anchor_row + win->height;
    int col_start = win->anchor_col;
    int col_end = win->anchor_col + win->width;

    // draw borders if it's not anchored at the top row
    if (row_start > 0) {
        // first move the cursor
        rbuf_write_move_cursor_command(rbuf, row_start, col_start);
        // check if we can draw the "┎"
        if (!occupied[term_width * row_start + col_start - 1]) {
            occupied[term_width * row_start + col_start - 1] = true;
            rbuf_append(rbuf, "┎");
        }
        bool needs_repostion = false;
        // draw the "─"s
        for (int col = col_start; col < col_end; col++) {
            if (occupied[row_start * term_width + col - 1]) {
                // can't draw here, next draw needs to repostion cursor
                needs_repostion = true;
            } else {
                // can draw here
                if (needs_repostion) {
                    rbuf_write_move_cursor_command(rbuf, row_start, col);
                    // we have done the repositioning, next draw don't need to
                    // do it againt
                    needs_repostion = false;
                }
                occupied[row_start * term_width + col - 1] = true;
                rbuf_append(rbuf, "─");
            }
        }
        // check if we can draw the "┒"
        if (col_end < term_width &&
            !occupied[row_start * term_width + col_end - 1]) {
            if (needs_repostion)
                rbuf_write_move_cursor_command(rbuf, row_start, col_end);
            rbuf_append(rbuf, "┒");
            occupied[row_start * term_width + col_end - 1] = true;
        }
        row_start++;
    }

    // (maybe) draw the vertical border and the buffer contents
    for (int row = row_start; row < row_end; row++) {
        rbuf_write_move_cursor_command(rbuf, row, win->anchor_col);
        bool needs_reposition = false;
        // left column border if anchor col is not 0
        if (row_start > 0) {
            if (occupied[row * term_width + col_start - 1]) {
                needs_reposition = true;
            } else {
                occupied[row * term_width + col_start - 1] = true;
                rbuf_append(rbuf, "│");
            }
        }
        // TODO: draw buffer contents
        for (int col = col_start + 1; col <= col_end - 1; col++) {
            // if (occupied[row * term_width + col])
        }
        // right column border
        if (row_end < term_width && !occupied[row * term_width + col_end - 1]) {
            occupied[row * term_width + col_end - 1] = true;
            if (needs_reposition)
                rbuf_write_move_cursor_command(rbuf, row, col_end);
            rbuf_append(rbuf, "│");
        }
    }

    // maybe draw bottom border
    if (row_end < term_height) {
        // first move the cursor
        rbuf_write_move_cursor_command(rbuf, row_end, col_start);
        // check if we can draw the "┖"
        if (!occupied[row_end * term_width + col_start - 1]) {
            occupied[row_end * term_width + col_start - 1] = true;
            rbuf_append(rbuf, "┖");
        }
        bool needs_repostion = false;
        // draw the "─"s
        for (int col = col_start; col < col_end; col++) {
            if (occupied[row_end * term_width + col - 1]) {
                // can't draw here, next draw needs to repostion cursor
                needs_repostion = true;
            } else {
                // can draw here
                if (needs_repostion) {
                    rbuf_write_move_cursor_command(rbuf, row_start, col);
                    needs_repostion = false;
                }
                occupied[row_end * term_width + col - 1] = true;
                rbuf_append(rbuf, "─");
            }
        }
        // check if we can draw the "┚"
        if (col_end < term_width &&
            !occupied[row_end * term_width + col_end - 1]) {
            if (needs_repostion)
                rbuf_write_move_cursor_command(rbuf, row_end, col_end);
            rbuf_append(rbuf, "┚");
        }
    }
}

char *win_get_display_char(struct Window *win, int row, int col);

#endif
