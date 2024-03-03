#include "lib/linked_list.h"
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>
#include <wchar.h>

// data
int term_width;
int term_height;
bool *occupied;
wchar_t *grid;

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

void occupy(int row, int col) {
    occupied[term_width * (row - 1) + col - 1] = true;
}

void grid_put_wchar(int row, int col, const wchar_t thing) {
    grid[term_width * (row - 1) + col - 1] = thing;
}

wchar_t grid_get_wchar(int row, int col) {
    return grid[(row - 1) * term_width + col - 1];
}

void maybe_draw(int row, int col, const wchar_t thing) {
    if (row < 1 || row >= term_height)
        return;
    if (col < 1 || col >= term_width)
        return;
    if (!occupied_at(row, col, occupied)) {
        occupy(row, col);
        grid_put_wchar(row, col, thing);
    }
}

void grid_render(Box box[static 1]) {
    // goto anchor
    // TODO: handle resize
    int arow = box->arow;
    int acol = box->acol;
    int height = box->height;
    int width = box->width;
    maybe_draw(arow, acol, L'┍');
    for (int i = 1; i < width; i++) {
        maybe_draw(arow, acol + i, L'━');
    }
    maybe_draw(arow, acol + width, L'┑');

    for (int i = arow + 1; i < arow + height; i++) {
        maybe_draw(i, acol, L'│');
        for (int j = acol + 1; j < acol + width; j++) {
            maybe_draw(i, j, L' ');
        }
        maybe_draw(i, acol + width, L'│');
    }

    maybe_draw(arow + height, acol, L'┕');
    for (int i = 1; i < width; i++) {
        maybe_draw(arow + height, acol + i, L'━');
    }
    maybe_draw(arow + height, acol + width, L'┙');

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

void resize_box(Box *b, double h_factor, double w_factor) {
    b->width = (int)(b->width * w_factor);
    b->height = (int)(b->height * h_factor);
}

// main
int main() {
    setlocale(LC_ALL, "C.UTF-8");
    struct winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0)
        panic("get terminal dimensions");
    term_height = ws.ws_row + 1;
    term_width = ws.ws_col + 1;
    int grid_num = (term_height * term_width);
    occupied = malloc(grid_num * sizeof(bool));
    grid = malloc(grid_num * sizeof(wchar_t));

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
    llist_forall(windows, grid_render, Box);

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

    // main loop
    char c;
    bool quit = false;
    while (!quit) {
        // handle resize
        if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0)
            panic("get terminal dimensions");
        if (term_height != ws.ws_row + 1 || term_width != ws.ws_col) {
            term_height = ws.ws_row + 1;
            term_width = ws.ws_col + 1;
            int grid_num = term_height * term_width;
            occupied = malloc(grid_num * sizeof(bool));
            grid = malloc(grid_num * sizeof(wchar_t));
        } else {
            memset(occupied, 0, term_height * term_width * sizeof(bool));
            memset(grid, 0, term_height * term_width * sizeof(wchar_t));
        }
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
        // clear screen
        printf("\x1b[2J");
        printf("\x1b[H");
        llist_forall(windows, grid_render, Box);
        for (int row = 1; row < term_height; row++) {
            for (int col = 1; col < term_width; col++) {
                wchar_t wc = grid_get_wchar(row, col);
                if (wc != L'\0')
                    printf("%lc", wc);
            }
            printf("\r\n");
        }
    }
    free(occupied);
    llist_free(&windows);
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &original) == -1)
        panic("tcsetattr");
    return 0;
}
