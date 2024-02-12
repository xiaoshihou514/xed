#include "globals.c"
#include "utils.c"
#include <stdbool.h>
#include <unistd.h>

void handle_normal_mode_bindings(char c) {
    switch (c) {
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
    case 'i':
        mode = 1;
        break;
    }
}

void handle_insert_mode_bindings(char c) {
    switch (c) {
        // TODO
    }
}

bool process_input() {
    char c;
    if (read(STDIN_FILENO, &c, 1) == -1) {
        panic("read");
    } else {
        switch (mode) {
        case 0:
            // HACK
            if (c == 'q')
                return true;
            handle_normal_mode_bindings(c);
            break;
        case 1:
            handle_insert_mode_bindings(c);
            break;
        }
    }
    return false;
}
