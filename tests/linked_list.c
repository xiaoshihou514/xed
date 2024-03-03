#include "../src/lib/linked_list.h"
#include "criterion/assert.h"
#include "criterion/internal/assert.h"
#include <complex.h>
#include <criterion/criterion.h>

LinkedList llist;

void new_linked_list(void) { llist = llist_new; }

void free_linked_list(void) { llist_free(&llist); }

TestSuite(linked_list, .init = new_linked_list, .fini = free_linked_list);

Test(linked_list, new) {
    cr_expect(llist.head == nullptr);
    cr_expect(llist.tail == nullptr);
    cr_expect(llist.size == 0);
}

Test(linked_list, pushing_two_ints) {
    int first = 42;
    int second = 37;
    llist_push(&llist, &first);
    cr_expect(llist.size == 1);
    llist_push(&llist, &second);
    cr_expect(*(int *)(llist.head->val) == 42);
    cr_expect(*(int *)(llist.tail->val) == 37);
    cr_expect((llist.head->next) == llist.tail);
    cr_expect((llist.tail->next) == nullptr);
    cr_expect(llist.size == 2);
}

Test(linked_list, pushing_many_ints) {
    // creating a linked list with 1..99
    int arr[42];
    for (int i = 0; i < 42; i++) {
        arr[i] = i;
        llist_push(&llist, &arr[i]);
    }
    cr_expect(llist.size == 42);
    // traverse the list and check
    Node *node = llist.head;
    for (int i = 0; i < 42; i++) {
        cr_expect(node != nullptr);
        cr_expect(*(int *)node->val == arr[i]);
        node = node->next;
    }
    cr_expect(node == nullptr);
}

typedef struct {
    int bar;
} Foo;

Test(linked_list, removing_even) {
    // creating a linked list with 1..99
    int arr[42];
    for (int i = 0; i < 42; i++) {
        arr[i] = i;
        llist_push(&llist, &arr[i]);
    }
    // traverse the list and check
    Node *node = llist.head;
    while (node != nullptr) {
        if (*(int *)(node->val) % 2 == 0) {
            llist_remove(&llist, node);
        }
        node = node->next;
    }
    cr_expect(llist.size == 21);
    while (node != nullptr) {
        cr_expect(*(int *)node->val % 2 != 0);
        node = node->next;
    }
}
