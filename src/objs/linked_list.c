#include <stddef.h>
#include <stdlib.h>

#ifndef _LINKED_LIST_C
#define _LINKED_LIST_C

struct Node {
    void *val;
    struct Node *prev;
    struct Node *next;
};

struct LinkedList {
    struct Node *head;
    struct Node *tail;
};

struct LinkedList llist_new(void *initial_val) {
    struct Node node = {initial_val, NULL, NULL};
    return (struct LinkedList){&node, &node};
}

void llist_push(void *val, struct LinkedList *llist) {
    struct Node node = {val, llist->tail, NULL};
    llist->tail->next = &node;
    llist->tail = &node;
}

void llist_remove(struct Node *node) {
    struct Node *prev = node->prev;
    struct Node *next = node->next;
    prev->next = next;
}

#endif
