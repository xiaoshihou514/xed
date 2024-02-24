#include "stdio.h"
#include "unistd.h"
#include <stdlib.h>
#include <sys/ioctl.h>

typedef struct {
    int arow;
    int acol;
    int width;
    int height;
} Box;

struct _Node {
    void *val;
    struct _Node *next;
    struct _Node *prev;
};

typedef struct _Node Node;

typedef struct {
    Node *head;
    Node *tail;
} LinkedList;

int term_width;
int term_height;

void llist_push(LinkedList llist[static 1], void *element) {
    Node *new = malloc(sizeof(Node));
    *new = ((Node){.val = element, .prev = llist->tail, .next = nullptr});
    if (llist->head == nullptr) {
        llist->head = new;
        llist->tail = new;
        return;
    }
    llist->tail->next = new;
    llist->tail = new;
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

#define NewLinkedList                                                          \
    { .head = nullptr, .tail = nullptr }

void maybe_draw(int row, int col, const char *thing, bool *occupied) {
    if (!occupied[term_width * row + col - 1]) {
        printf("\x1b[%d;%dH", row, col);
        printf("%s", thing);
        occupied[term_width * row + col - 1] = true;
    }
}

void draw(Box *box, bool *occupied) {
    // goto anchor
    maybe_draw(box->arow, box->acol, "┍", occupied);
    for (int i = 1; i < box->width; i++) {
        maybe_draw(box->arow, box->acol + i, "━", occupied);
    }
    maybe_draw(box->arow, box->acol + box->width, "┑", occupied);

    for (int i = box->arow + 1; i < box->arow + box->height; i++) {
        maybe_draw(i, box->acol, "│", occupied);
        for (int j = box->acol + 1; j < box->acol + box->width - 1; j++) {
            maybe_draw(i, j, " ", occupied);
        }
        maybe_draw(i, box->acol + box->width, "│", occupied);
    }

    maybe_draw(box->arow + box->height, box->acol, "┕", occupied);
    for (int i = 1; i < box->width; i++) {
        maybe_draw(box->arow + box->height, box->acol + i, "━", occupied);
    }
    maybe_draw(box->arow + box->height, box->acol + box->width, "┙", occupied);

    // goto left bottom
    printf("\x1b[%d;%dH", term_height, 1);
}

void panic(const char *reason) {
    write(STDOUT_FILENO, "\x1b[2J", 4);
    write(STDOUT_FILENO, "\x1b[H", 3);

    perror(reason);
    exit(1);
}

int main() {
    struct winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0)
        panic("get terminal dimensions");
    term_height = ws.ws_row;
    term_width = ws.ws_col;
    bool *occupied = malloc((term_height * term_width) * sizeof(bool));

    // clear screen
    printf("\x1b[2J");
    printf("\x1b[H");

    LinkedList windows = NewLinkedList;
    llist_push(&windows, &(Box){
                             .arow = 13,
                             .acol = 15,
                             .width = 10,
                             .height = 3,
                         });
    llist_push(&windows, &(Box){
                             .arow = 12,
                             .acol = 2,
                             .width = 17,
                             .height = 6,
                         });
    llist_push(&windows, &(Box){
                             .arow = 11,
                             .acol = 10,
                             .width = 22,
                             .height = 10,
                         });
    Node *node = windows.head;
    while (node) {
        draw((Box *)(node->val), occupied);
        node = node->next;
    }
    free(occupied);
    llist_free(&windows);
    return 0;
}
