#include "utils.c"
#include "view.c"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

// typedefs
struct RenderBuffer {
    char *data;
    int len;
    int used;
};
struct RenderState {
    int x;
    int y;
    struct RenderBuffer buf;
};

// data
struct termios orig_termios;
struct RenderBuffer buf;
struct RenderState state;

// buffer related functions
void render_buf_append(const char *s, int len) {
    if (buf.used + len > buf.len) {
        buf.data = realloc(buf.data, len * 2);
        buf.len *= 2;
    }
    // we add offset to the ptr to append
    memcpy(buf.data + buf.used, s, buf.len);
    buf.used += len;
}

void render_buf_flush() { write(STDOUT_FILENO, buf.data, buf.len); }

// Terminal related
#define CLEAR "\x1b[2J"
#define GOTO_LEFT_TOP "\x1b[H"
#define HIDE_CURSOR "\x1b[?25l"
#define SHOW_CURSOR "\x1b[?25h"
char *SET_CURSOR(char *buf, int row, int col) {
    snprintf(buf, sizeof(buf), "\x2b[%d;%dH", row, col);
    return buf;
}

void defer_command(const char *cmd) { render_buf_append(cmd, strlen(cmd)); }

void disable_raw_mode() { tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios); }

void enable_raw_mode() {
    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(disable_raw_mode);
    struct termios raw = orig_termios;
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= (CS8);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

// init and cleanup
void init_render_state() {
    buf.data = malloc(1024);
    buf.len = 1024;
    buf.used = 0;
    state.buf = buf;
    state.x = 1;
    state.y = 1;
}

void renderer_cleanup() { free(buf.data); }

// the main functionality for this file
void refresh_screen(struct View *view) {
    // TODO
}
