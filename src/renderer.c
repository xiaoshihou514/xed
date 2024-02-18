#include "globals.c"
#include "objs/line_segment.c"
#include "objs/resizable_buffer.c"
#include "objs/window.c"
#include "utils.c"
#include <termios.h>

// data
struct termios default_termios;
struct ResizableBuffer canvas;

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

// draw the screen according to context
void refresh_screen() {
    struct Node *node = focused_view.win_stack.head;
    for (int line = 1; line <= term_height; line++) {
        struct LinkedList segs = line_segs_new(term_width);
        while (node != NULL) {
            struct Window *win = node->val;
            window_render_segments(win, &canvas, line, &segs);
        }
    }
}
