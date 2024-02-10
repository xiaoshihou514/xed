#include "key.c"
#include "renderer.c"

int main(int argc, char *argv[]) {
    enable_raw_mode();
    init_render_state();
    while (1) {
        refresh_screen();
        process_input();
    }
    renderer_cleanup();
    disable_raw_mode();
}
