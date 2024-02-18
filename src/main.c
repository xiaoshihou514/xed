#include "globals.c"
#include "key.c"
#include "renderer.c"
#include <stdbool.h>

int main(int argc, char *argv[]) {
    init_globals(argc, argv);
    enable_raw_mode();
    while (!SHOULD_EXIT) {
        process_keys();
        refresh_screen();
    }
    disable_raw_mode();
    free_all_buffers();
    return 0;
}
