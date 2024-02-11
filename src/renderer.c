#include "typedefs.h"
#include "utils.c"
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

// typedefs
struct RenderBuffer {
    char *data;
    int len;
    int used;
};
struct RenderState {
    int term_height;
    int term_width;
    int cursor_row;
    int cursor_col;
    struct RenderBuffer buf;
};

// data
struct termios orig_termios;
struct RenderBuffer buf;
struct RenderState state;

// buffer related functions
void render_buf_append(const char *s) {
    int len = strlen(s);
    if (buf.used + len > buf.len) {
        buf.data = realloc(buf.data, len * 2);
        buf.len *= 2;
    }
    // we add offset to the ptr to append
    memcpy(buf.data + buf.used, s, buf.len);
    buf.used += len;
}

void render_buf_flush() {
    write(STDOUT_FILENO, buf.data, buf.used);
    buf.used = 0;
}

// Terminal related
#define CLEAR "\x1b[2J"
#define GOTO_LEFT_TOP "\x1b[H"
#define HIDE_CURSOR "\x1b[?25l"
#define SHOW_CURSOR "\x1b[?25h"
/// PRE: buf size is 16
char *SET_CURSOR(char *buf, int row, int col) {
    snprintf(buf, 16, "\x1b[%d;%dH", row, col);
    return buf;
}

void defer_command(const char *cmd) { render_buf_append(cmd); }

void disable_raw_mode() { tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios); }

void enable_raw_mode() {
    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(disable_raw_mode);
    struct termios raw = orig_termios;
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= (CS8);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

// init and cleanup
void renderer_init() {
    buf.data = malloc(1024);
    buf.len = 1024;
    buf.used = 0;
    state.buf = buf;
    state.cursor_row = 1;
    state.cursor_col = 1;
    // get terminal dimensions
    struct winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1) {
        panic("getting terminal dimensions");
    } else {
        state.term_width = ws.ws_col;
        state.term_height = ws.ws_row;
    }
}

void renderer_cleanup() { free(buf.data); }

// window related functions
bool window_has_content_on_line(struct Window *win, int line) {
    return win->anchor_row <= line && win->anchor_row + win->height >= line;
}

void window_render_line(struct Window *win, int row) {
    // TODO
    // for now just render something random
    render_buf_append("~");
}

// the main functionality for this file
void refresh_screen(struct View *view) {
    // clear the screen and hide cursor
    defer_command(HIDE_CURSOR);
    defer_command(CLEAR);

    // render line after line
    struct LinkedList windows = view->windows;
    // look, I promise these are pointers to windows
    struct Node *win = (struct Node *)windows.head;
    for (int row = 1; row <= state.term_height; row++) {
        while (win != NULL) {
            if (window_has_content_on_line(win->element, row)) {
                window_render_line(win->element, row);
            }
            win = win->next;
        }
        render_buf_append("\r\n");
    }

    // calculate cursor pos
    if (view->focused != NULL) {
        struct Window *focused = view->focused;
        state.cursor_row =
            focused->anchor_row + focused->cursor_row - focused->rowoff;
        state.cursor_col =
            focused->anchor_col + focused->cursor_col - focused->rowoff;
    }
    // render cursor pos
    char cursor_cmd[16];
    SET_CURSOR(cursor_cmd, state.cursor_row, state.cursor_col);
    defer_command(cursor_cmd);
    defer_command(SHOW_CURSOR);
    render_buf_flush();
}
