#include "globals.c"
#include "key.c"
#include "renderer.c"
#include <stdbool.h>

int main(int argc, char *argv[]) {
    init_globals(argc, argv);
    enable_raw_mode();
    renderer_init();
    while (!SHOULD_EXIT) {
        process_keys();
        refresh_screen();
    }
    renderer_cleanup();
    free_all_buffers();
    disable_raw_mode();
    return 0;
}
