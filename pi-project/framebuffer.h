char *framebuffer_init(int x, int y, int depth);
int framebuffer_clear_screen(int color);
int framebuffer_hline(int color, int x0, int x1, int y);
int framebuffer_vline(int color, int y0, int y1, int x);
int framebuffer_putpixel(int color, int x, int y);
int framebuffer_push(void);
int framebuffer_ready(void);
int framebuffer_gradient(void);
