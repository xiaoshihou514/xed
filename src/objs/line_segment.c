#include "linked_list.c"

#ifndef _LINE_SEGMENT_C
#define _LINE_SEGMENT_C

struct LineSeg {
    int start;
    int end;
};

struct LinkedList line_segs_new(int width) {
    return llist_new(&(struct LineSeg){1, width});
}

#endif
