#include "objs/linked_list.c"
#include "objs/view.c"
#include "utils.c"
#include <stdbool.h>
#include <sys/ioctl.h>

#ifndef _GLOBALS_H
#define _GLOBALS_H

bool SHOULD_EXIT = false;

struct LinkedList views;
struct View focused_view;

int term_height;
int term_width;

void init_globals(int argc, char *argv[]) {
    struct winsize ws;

    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) {
        panic("get terminal dimensions");
    }
    term_width = ws.ws_col;
    term_height = ws.ws_row;

    if (argc == 0) {
        // a scratch buffer with statusline
        struct View initial_view = view_new(term_width, term_height);
        views = llist_new(&initial_view);
    } else {
        // TODO: parse args
    }
}

// TODO: clean up after ourselves
void free_all_buffers();

#endif
