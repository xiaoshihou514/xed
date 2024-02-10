#include "buffer.c"

struct Window {
    int width;
    int height;
    int anchor_row;
    int anchor_col;
    struct Buffer buf;
};

// represents a view for the renderer to render
struct View {
    // a list of windows
    struct Window (*windows)[];
    // a statusline or a command prompt
    char *status_line; // nullable
    char *cmdline;     // nullable
};
