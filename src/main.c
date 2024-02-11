#include "globals.c"
#include "key.c"
#include "renderer.c"
#include <stdbool.h>

int main(int argc, char *argv[]) {
    enable_raw_mode();
    renderer_init();
    bool quit = false;
    while (!quit) {
        refresh_screen(&view);
        quit = process_input();
    }
    printf("freed\n");
    renderer_cleanup();
    disable_raw_mode();
}
