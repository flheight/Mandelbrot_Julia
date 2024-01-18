#include "headers.h"

int main(int argc, char *argv[]) {
    int x;
    int y;
    int w;
    int h;
    int ret = 1;

    if(SDL_Init(SDL_INIT_VIDEO) < 0) 
        goto err;

    x = 60;
    y = 60;

    /********************************************/

    w = 750;
    h = 750;

    /********************************************/

    SDL_Window *window1 = NULL;
    window1 = SDL_CreateWindow("Mandelbrot fractal", x, y, w, h, SDL_WINDOW_SHOWN);
    if(!window1)
        goto sdl_quit;

    SDL_Renderer *renderer1 = NULL;
    renderer1 = SDL_CreateRenderer(window1, -1, SDL_RENDERER_ACCELERATED);
    if(!renderer1)
        goto destroy_window1;

    SDL_Window *window2 = NULL;
    window2 = SDL_CreateWindow("Julia fractal", x, y, w, h, SDL_WINDOW_SHOWN);
    if(!window2)
        goto sdl_quit;

    SDL_Renderer *renderer2 = NULL;
    renderer2 = SDL_CreateRenderer(window2, -1, SDL_RENDERER_ACCELERATED);
    if(!renderer2)
        goto destroy_window2;

    /********************************************/

    window win1 = (window) {window1, renderer1, NULL, w, h, -2, 1, -1.5, 1.5};
    window win2 = (window) {window2, renderer2, NULL, w, h, -2, 2, -2, 2};

    win1.tex = SDL_CreateTexture(win1.ren, SDL_PIXELFORMAT_BGRA32, SDL_TEXTUREACCESS_STREAMING, win1.w, win1.h);
    win2.tex = SDL_CreateTexture(win2.ren, SDL_PIXELFORMAT_BGRA32, SDL_TEXTUREACCESS_STREAMING, win2.w, win2.h);

    render_loop(&win1, &win2, 200, 768);

    /********************************************/

    ret = 0;

    SDL_DestroyRenderer(renderer1);
    SDL_DestroyRenderer(renderer2);

    destroy_window2:
        SDL_DestroyWindow(window2);
    destroy_window1:
        SDL_DestroyWindow(window1);
    sdl_quit:
        SDL_Quit();
    err:
        if(ret)
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "error : %s\n", SDL_GetError());
        return ret;

    (void) argc;
    (void) argv;
}