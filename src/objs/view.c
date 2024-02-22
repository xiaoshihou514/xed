#include "linked_list.c"
#include "window.c"

#ifndef _VIEW_C
#define _VIEW_C

typedef struct {
    LinkedList win_stack;
    Window *focused;
} View;

// TODO
View view_new(int term_width, int term_height) {
    Window win = scratch_win(term_width, term_height);
    View view = {llist_new(&win), &win};
    return view;
}

#endif
