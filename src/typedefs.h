#ifndef TYPEDEF_H
#define TYPEDEF_H

struct LinkedList {
    struct Node *head;
    struct Node *last;
};

struct Node {
    void *element;
    void *next;
    void *prev;
};

struct View {
    // TODO: window stack, since windows could be on top of each other
    // a list of windows... for now
    struct LinkedList windows;
    struct Window *focused;
};

struct Buffer {
    // TODO
};

struct Window {
    int anchor_row;        // relative pos in a view
    int anchor_col;        // relative pos in a view
    int width;             // window width
    int height;            // window height
    int cursor_row;        // line number within a buffer
    int cursor_col;        // col number within a buffer
    int rowoff;            // row offset within a buffer
    int coloff;            // col offset within a buffer
    struct Buffer *buffer; // backing buffer
};

#endif
