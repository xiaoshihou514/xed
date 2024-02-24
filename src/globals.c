#include "objs/linked_list.c"
#include "objs/view.c"
#include "utils.c"
#include <sys/ioctl.h>

#ifndef _GLOBALS_H
#define _GLOBALS_H

bool SHOULD_EXIT = false;

LinkedList views;
View focused_view;

int term_height;
int term_width;

void init_globals(int argc, char *argv[]) {
    // get terminal dimensions
    struct winsize ws;

    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0)
        panic("get terminal dimensions");

    term_width = ws.ws_col;
    term_height = ws.ws_row;

    if (argc == 1) {
        // no arguments given
        // just create a scratch buffer
        focused_view = *view_new(term_width, term_height);
        views = llist_new(&focused_view);
    } else {
        // TODO: parse args
    }
}

void free_all_views(void) {
    Node *n1 = views.head;
    while (n1) {
        View *view = n1->val;
        view_free(view);
        n1 = n1->next;
    }
}

#endif
