#include "globals.c"
#include "utils.c"
#include <stdbool.h>
#include <unistd.h>

bool process_input() {
    char c;
    if (read(STDIN_FILENO, &c, 1) == -1) {
        panic("read");
    } else {
        switch (c) {
        case 'q':
            return true;
        case 'h':
            view.focused->cursor_col--;
            break;
        case 'l':
            view.focused->cursor_col++;
            break;
        case 'j':
            view.focused->cursor_row++;
            break;
        case 'k':
            view.focused->cursor_row--;
            break;
        }
    }
    return false;
}
