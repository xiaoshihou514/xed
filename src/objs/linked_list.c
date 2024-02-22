#include <stddef.h>
#include <stdlib.h>

#ifndef _LINKED_LIST_C
#define _LINKED_LIST_C

struct _Node {
    void *val;
    struct _Node *prev;
    struct _Node *next;
};
typedef struct _Node Node;

typedef struct {
    Node *head;
    Node *tail;
} LinkedList;

LinkedList llist_new(void *initial_val) {
    Node node = {initial_val, nullptr, nullptr};
    return (LinkedList){&node, &node};
}

void llist_push(void *val, LinkedList *llist) {
    Node *node = (Node *)malloc(sizeof(Node));
    node->val = val;
    node->prev = llist->tail;
    node->next = nullptr;
    llist->tail->next = node;
    llist->tail = node;
}

void llist_remove(Node *node) {
    Node *prev = node->prev;
    Node *next = node->next;
    prev->next = next;
}

void llist_free(LinkedList *llist) {
    Node *current = llist->head;
    while (current) {
        Node *next = current->next;
        free(current);
        current = next;
    }
    llist->head = llist->tail = nullptr;
}

#endif
