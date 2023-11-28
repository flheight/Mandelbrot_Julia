#include "headers.h"

SDL_Window *win1;
SDL_Window *win2;

int main(int argc, char *argv[]) {
    SDL_Renderer *renderer1;
    SDL_Renderer *renderer2;
    SDL_Event events;
    int x;
    int y;
    int w;
    int h;
    int ret = 1;

    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "error : %s\n", SDL_GetError());
        goto err;
    }

    x = 60;
    y = 60;
    renderer1 = NULL;
    renderer2 = NULL;

    /********************************************/

    w = 750;
    h = 750;

    /********************************************/

    win1 = SDL_CreateWindow("Mandelbrot fractal", x, y, w, h, SDL_WINDOW_SHOWN);
    if(!win1) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "error : %s\n", SDL_GetError());
        goto sdl_quit;
    }

    renderer1 = SDL_CreateRenderer(win1, -1, SDL_RENDERER_ACCELERATED);
    if(!renderer1) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "error : %s\n", SDL_GetError());
        goto destroy_window1;
    }

    win2 = SDL_CreateWindow("Julia fractal", x, y, w, h, SDL_WINDOW_SHOWN);
    if(!win2) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "error : %s\n", SDL_GetError());
        goto sdl_quit;
    }

    renderer2 = SDL_CreateRenderer(win2, -1, SDL_RENDERER_ACCELERATED);
    if(!renderer2) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "error : %s\n", SDL_GetError());
        goto destroy_window2;
    }

    /********************************************/

    window ww1 = (window) {win1, w, h, -2, 1, -1.5, 1.5};
    window ww2 = (window) {win2, w, h, -2, 2, -2, 2};

    render_loop(events, renderer1, renderer2, &ww1, &ww2, 200, 768);

    /********************************************/

    ret = 0;

    SDL_DestroyRenderer(renderer1);
    SDL_DestroyRenderer(renderer2);

    destroy_window2:
        SDL_DestroyWindow(win2);
    destroy_window1:
        SDL_DestroyWindow(win1);
    sdl_quit:
        SDL_Quit();
    err:
        return ret;

    (void) argc;
    (void) argv;
}