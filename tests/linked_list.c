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
}

Test(linked_list, adding_ints) {
    int first = 42;
    int second = 37;
    llist_push(&llist, &first);
    llist_push(&llist, &second);
    cr_expect(*(int *)(llist.head->val) == 42);
    cr_expect(*(int *)(llist.tail->val) == 37);
    cr_expect((llist.head->next) == llist.tail);
    cr_expect((llist.tail->next) == nullptr);
}
