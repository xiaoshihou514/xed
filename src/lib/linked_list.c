#include "linked_list.h"
#include <stdlib.h>

void llist_push(LinkedList llist[static 1], void *element) {
    Node *new = malloc(sizeof(Node));
    *new = ((Node){.val = element, .prev = llist->tail, .next = nullptr});
    if (llist->head == nullptr) {
        llist->head = new;
        llist->tail = new;
    } else {
        llist->tail->next = new;
        llist->tail = new;
    }
    llist->size++;
}

void llist_free(LinkedList llist[static 1]) {
    Node *node = llist->head;
    while (node) {
        Node *temp = node;
        node = node->next;
        free(temp);
    }
    llist->head = nullptr;
    llist->tail = nullptr;
}

void llist_remove(LinkedList llist[static 1], Node node[static 1]) {
    llist->size--;
    if (node->prev != nullptr)
        node->prev->next = node->next;
    if (node->next != nullptr)
        node->next->prev = node->prev;
}
