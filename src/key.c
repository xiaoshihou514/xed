#include "utils.c"
#include <stdbool.h>
#include <unistd.h>

bool process_input() {
    char c;
    if (read(STDIN_FILENO, &c, 1) == 1) {
        panic("read");
    } else {
        if (c == 'q')
            return true;
    }
    return false;
}
