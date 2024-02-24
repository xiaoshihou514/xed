#include "linked_list.c"
#include "window.c"

#ifndef _VIEW_C
#define _VIEW_C

typedef struct {
    LinkedList win_stack;
    Window *focused;
} View;

// TODO
View *view_new(int term_width, int term_height) {
    Window win = scratch_win(term_width, term_height);
    // TODO: llist_new(&win) is not working
    View *view = (View *)malloc(sizeof(View));
    view->win_stack = llist_new(&win);
    view->focused = &win;
    return view;
}

void view_free(View *view) {
    LinkedList wins = view->win_stack;
    Node *n2 = wins.head;
    while (n2) {
        Window *win = n2->val;
        window_free(win);
        n2 = n2->next;
    }
}

#endif
