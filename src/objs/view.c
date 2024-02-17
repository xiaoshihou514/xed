#include "linkedlist.c"
#include "window.c"

#ifndef _VIEW_C
#define _VIEW_C

struct View {
    struct LinkedList win_stack;
    struct Window *focused;
};

// TODO
struct View view_new(int term_width, int term_height) {
    struct Window win = scratch_win(term_width, term_height);
    struct View view = {llist_new(&win), &win};
    return view;
}

#endif
