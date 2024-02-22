#include "globals.c"
#include "objs/resizable_buffer.c"
#include "objs/window.c"
#include "utils.c"
#include <stdlib.h>
#include <termios.h>

// data
struct termios default_termios;
ResizableBuffer canvas;
bool *render_table;

// reset terminal behaviour
void disable_raw_mode() {
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &default_termios) == -1)
        panic("disable raw mode");
}

// tells the terminal how we want to get the input
void enable_raw_mode() {
    if (tcgetattr(STDIN_FILENO, &default_termios) == -1)
        panic("enable raw mode");

    atexit(disable_raw_mode);

    // change a bunch of flags
    struct termios raw = default_termios;
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= (CS8);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
        panic("enable raw mode");

    // TODO: enable mouse
    // printf("\x1b[?1049h\x1b[0m\x1b[2J\x1b[?1003h\x1b[?1015h\x1b[?1006h\x1b[?25l");
}

void renderer_init() {
    canvas = rbuf_new();
    render_table = malloc(term_height * term_width);
}

void renderer_cleanup() {
    rbuf_free(&canvas);
    free(render_table);
}

// draw the screen according to context
void refresh_screen() {
    Node *node = focused_view.win_stack.head;
    while (node) {
        Window *win = node->val;
        win_render(win, &canvas, render_table, term_height, term_width);
        node = node->next;
    }
    rbuf_flush(&canvas);
}
