#include <stddef.h>

#ifndef _LINKED_LIST_C
#define _LINKED_LIST_C

struct _Node {
    void *val;
    struct _Node *next;
    struct _Node *prev;
};

typedef struct _Node Node;

typedef struct {
    size_t size;
    Node *head;
    Node *tail;
} LinkedList;

/*
 * pushes an element to the end of a non-null linked list
 */
void llist_push(LinkedList llist[static 1], void *element);

/*
 * free a non-null linked list
 */
void llist_free(LinkedList llist[static 1]);

/*
 * remove a node from arbitray linked list and returns the next element
 */
void llist_remove(LinkedList llist[static 1], Node node[static 1]);

/*
 * creates an empty linked list
 */
#define llist_new                                                              \
    (LinkedList) { .size = 0, .head = nullptr, .tail = nullptr }

/*
 * iterate over a linked list with a function
 * you have to provide the function pointer, the type for items in the linked
 * list, and (optionally) extra arguments
 */
#define llist_forall(llist, f, type, ...)                                      \
    for (Node *current = llist.head; current != nullptr;                       \
         current = current->next)                                              \
        f((type *)current->val, ##__VA_ARGS__);

#endif
