#include "lib/linked_list.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

// data
int term_width;
int term_height;

typedef struct {
    int arow;
    int acol;
    int width;
    int height;
} Box;

// draw
bool occupied_at(int row, int col, bool occupied[static 1]) {
    // row and col are 1, 1 indexed
    return occupied[term_width * (row - 1) + col - 1];
}

void occupy(int row, int col, bool occupied[static 1]) {
    occupied[term_width * (row - 1) + col - 1] = true;
}

void maybe_draw(int row, int col, const char thing[static 1],
                bool occupied[static 1]) {
    if (row < 1 || row >= term_height)
        return;
    if (col < 1 || col >= term_width)
        return;
    if (!occupied_at(row, col, occupied)) {
        printf("\x1b[%d;%dH", row, col);
        printf("%s", thing);
        occupy(row, col, occupied);
    }
}

void draw(Box box[static 1], bool occupied[static 1]) {
    // goto anchor
    maybe_draw(box->arow, box->acol, "┍", occupied);
    for (int i = 1; i < box->width; i++) {
        maybe_draw(box->arow, box->acol + i, "━", occupied);
    }
    maybe_draw(box->arow, box->acol + box->width, "┑", occupied);

    for (int i = box->arow + 1; i < box->arow + box->height; i++) {
        maybe_draw(i, box->acol, "│", occupied);
        for (int j = box->acol + 1; j < box->acol + box->width; j++) {
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

// util

void panic(const char *reason) {
    write(STDOUT_FILENO, "\x1b[2J", 4);
    write(STDOUT_FILENO, "\x1b[H", 3);

    perror(reason);
    exit(1);
}

// main
int main() {
    struct winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0)
        panic("get terminal dimensions");
    term_height = ws.ws_row + 1;
    term_width = ws.ws_col;
    bool *occupied = malloc((term_height * term_width) * sizeof(bool));

    // clear screen
    printf("\x1b[2J");
    printf("\x1b[H");

    // hide cursor
    printf("\x1b[?25l");

    LinkedList windows = llist_new;
    llist_push(&windows,
               &(Box){.arow = 12, .acol = 2, .width = 34, .height = 12});
    llist_push(&windows, &(Box){
                             .arow = 11,
                             .acol = 10,
                             .width = 44,
                             .height = 20,
                         });
    llist_push(&windows, &(Box){
                             .arow = 0,
                             .acol = 0,
                             .width = term_width,
                             .height = term_height,
                         });
    llist_forall(windows, draw, Box, occupied);

    // enable raw mode
    struct termios original;
    if (tcgetattr(STDIN_FILENO, &original))
        panic("tcgetattr");

    struct termios raw = original;
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= (CS8);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
        panic("tcsetattr");

    char c;
    bool quit = false;
    while (!quit) {
        c = ' ';
        read(STDIN_FILENO, &c, 1);
        switch (c) {
        case 'k':
            ((Box *)(windows.head->val))->arow--;
            break;
        case 'j':
            ((Box *)(windows.head->val))->arow++;
            break;
        case 'l':
            ((Box *)(windows.head->val))->acol++;
            break;
        case 'h':
            ((Box *)(windows.head->val))->acol--;
            break;
        case 'q':
            quit = true;
            break;
        default:
            break;
        }
        printf("\x1b[2J");
        printf("\x1b[H");
        occupied = malloc((term_height * term_width) * sizeof(bool));
        llist_forall(windows, draw, Box, occupied);
    }
    free(occupied);
    llist_free(&windows);
    return 0;
}
