#include "../wcswidth.c"
#include "buffer.c"
#include "linked_list.c"
#include "resizable_buffer.c"
#include <stddef.h>
#include <stdlib.h>
#include <wchar.h>

#ifndef _WINDOW_C
#define _WINDOW_C

typedef struct {
    Buffer *buffer;
    int anchor_row;
    int anchor_col;
    int height;
    int width;
    int rowoff;
    int coloff;
    int cursor_row;
    int cursor_col;
    // TODO: statuscol
} Window;

Window scratch_win(int term_width, int term_height) {
    Buffer buffer = buffer_new();
    Window *win = malloc(sizeof(Window));
    *win = (Window){
        .buffer = &buffer,
        .anchor_row = 1,
        .anchor_col = 1,
        .height = term_height,
        .width = term_width,
        .rowoff = 0,
        .coloff = 0,
        .cursor_row = 5,
        .cursor_col = 5,
    };
    return *win;
}

void window_free(Window *win) { buffer_free(win->buffer); }

void win_render(Window *win, ResizableBuffer *canvas, bool *occupied,
                int term_height, int term_width) {
    int row_start = win->anchor_row;
    int row_end = win->anchor_row + win->height;
    int col_start = win->anchor_col;
    int col_end = win->anchor_col + win->width;

    // draw borders if it's not anchored at the top row
    if (row_start > 0) {
        // first move the cursor
        rbuf_write_move_cursor_command(canvas, row_start, col_start);
        // check if we can draw the "┎"
        if (!occupied[term_width * row_start + col_start - 1]) {
            occupied[term_width * row_start + col_start - 1] = true;
            rbuf_append(canvas, "┎", 1);
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
                    rbuf_write_move_cursor_command(canvas, row_start, col);
                    // we have done the repositioning, next draw don't need to
                    // do it againt
                    needs_repostion = false;
                }
                occupied[row_start * term_width + col - 1] = true;
                rbuf_append(canvas, "─", 1);
            }
        }
        // check if we can draw the "┒"
        if (col_end < term_width &&
            !occupied[row_start * term_width + col_end - 1]) {
            if (needs_repostion)
                rbuf_write_move_cursor_command(canvas, row_start, col_end);
            rbuf_append(canvas, "┒", 1);
            occupied[row_start * term_width + col_end - 1] = true;
        }
        row_start++;
    }

    // (maybe) draw the vertical border and the buffer contents
    for (int row = row_start; row < row_end; row++) {
        rbuf_write_move_cursor_command(canvas, row, win->anchor_col);
        bool needs_reposition = false;

        // left column border if anchor col is not 0
        if (row_start > 0) {
            if (occupied[row * term_width + col_start - 1]) {
                needs_reposition = true;
            } else {
                occupied[row * term_width + col_start - 1] = true;
                rbuf_append(canvas, "│", 1);
            }
        }
        size_t length;
        wchar_t *line = buffer_get_line(win->buffer,
                                        win->rowoff + win->cursor_row, &length);
        // nothing for us to render
        if (line == nullptr)
            continue;
        for (int col = col_start + 1; col <= col_end - 1; col++) {
            if (occupied[row * term_width + col])
                needs_reposition = true;
            else {
                if (needs_reposition)
                    rbuf_write_move_cursor_command(canvas, row, col);
                // TODO: draw buffer contents
                if (col + win->coloff > length) {
                    // no contents, we draw a space
                    occupied[row * term_width + col - 1] = true;
                    rbuf_append(canvas, " ", 1);
                } else {
                    // there is content, check the width of the character
                    wchar_t wc = line[col - col_start];
                    int width = mk_wcwidth(wc);
                    occupied[row * term_width + col - 1] = true;
                    // if it's a CJK char, we want to peek ahead, but let's deal
                    // with the easy case first
                    if (width == 1) {
                        // TODO: this may or may not work for non-ascii
                        rbuf_append(canvas, (const char *)&wc, sizeof(char));
                    } else {
                        // CJK char
                        if (occupied[row * term_width + col])
                            rbuf_append(canvas, " ", 1);
                        else {
                            // TODO: this most certainly does not work
                            occupied[row * term_width + col] = true;
                            rbuf_append(canvas, (const char *)&wc,
                                        sizeof(wchar_t));
                        }
                    }
                }
            }
        }
        free(line);
        // right column border
        if (row_end < term_width && !occupied[row * term_width + col_end - 1]) {
            occupied[row * term_width + col_end - 1] = true;
            if (needs_reposition)
                rbuf_write_move_cursor_command(canvas, row, col_end);
            rbuf_append(canvas, "│", 1);
        }
    }

    // maybe draw bottom border
    if (row_end < term_height) {
        // first move the cursor
        rbuf_write_move_cursor_command(canvas, row_end, col_start);
        // check if we can draw the "┖"
        if (!occupied[row_end * term_width + col_start - 1]) {
            occupied[row_end * term_width + col_start - 1] = true;
            rbuf_append(canvas, "┖", 1);
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
                    rbuf_write_move_cursor_command(canvas, row_start, col);
                    needs_repostion = false;
                }
                occupied[row_end * term_width + col - 1] = true;
                rbuf_append(canvas, "─", 1);
            }
        }
        // check if we can draw the "┚"
        if (col_end < term_width &&
            !occupied[row_end * term_width + col_end - 1]) {
            if (needs_repostion)
                rbuf_write_move_cursor_command(canvas, row_end, col_end);
            rbuf_append(canvas, "┚", 1);
        }
    }
}

char *win_get_display_char(Window *win, int row, int col);

#endif
