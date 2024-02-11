#include "globals.c"
#include "key.c"
#include "renderer.c"

int main(int argc, char *argv[]) {
    enable_raw_mode();
    renderer_init();
    while (1) {
        refresh_screen(&view);
        process_input();
    }
    renderer_cleanup();
    disable_raw_mode();
}
