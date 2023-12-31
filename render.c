#include "headers.h"

void render_loop(window *ww1, window *ww2, int maxiter, int cmax) {
	Uint32 *grad = gradient(cmax);
    if(!grad)
        return;

    SDL_Event event;

    render_mandelbrot(ww1, grad, maxiter, cmax);

    Uint32 ticks = SDL_GetTicks();
    int running = 1;
    double cx = 0;
    double cy = 0;
    int posx;
    int posy;

    while(running) {
        SDL_PollEvent(&event);
        switch(event.type) {
            case SDL_QUIT:
            running = 0;
            break;
            case SDL_KEYDOWN:
            switch(event.key.keysym.sym) {
                case 'q':
                running = 0;
                break;
            }
            break; 
            case SDL_MOUSEBUTTONDOWN:
                switch(event.button.button) {
                    case SDL_BUTTON_LEFT:
                        if(SDL_GetMouseFocus() != ww1->win)
                            break;
                    while(event.type != SDL_MOUSEBUTTONUP) {
                        SDL_PollEvent(&event);
                        SDL_GetMouseState(&posx, &posy);
                        cx = ww1->xmin + (ww1->xmax - ww1->xmin) * posx / (ww1->w - 1);
                        cy = ww1->ymax - (ww1->ymax - ww1->ymin) * posy / (ww1->h - 1);
                        render_julia(ww2, cx, cy, grad, maxiter, cmax);     
                    }
                }
                break;
            case SDL_MOUSEWHEEL:
                if(event.wheel.y == 0)
                    break;
            SDL_GetMouseState(&posx, &posy);
            if(SDL_GetMouseFocus() == ww1->win) {
                double x = ww1->xmin + (ww1->xmax - ww1->xmin) * posx / (ww1->w - 1);
                double y = ww1->ymax - (ww1->ymax - ww1->ymin) * posy / (ww1->h - 1);
                if(event.wheel.y > 0)
                    rescale(ww1, x, y, scalefactor);
                else 
                    rescale(ww1, x, y, 1 / scalefactor);
                render_mandelbrot(ww1, grad, maxiter, cmax);
            }
            else {
                double x = ww2->xmin + (ww2->xmax - ww2->xmin) * posx / (ww2->w - 1);
                double y = ww2->ymax - (ww2->ymax - ww2->ymin) * posy / (ww2->h - 1);
                if(event.wheel.y > 0)
                    rescale(ww2, x, y, scalefactor);
                else 
                    rescale(ww2, x, y, 1 / scalefactor);
                render_julia(ww2, cx, cy, grad, maxiter, cmax);
            }            
        }
    Uint32 c = SDL_GetTicks();
    Uint32 t = c - ticks;
    if(t >= 5) {
        ticks = c;
        continue;
    }
    SDL_Delay(5 - t);
    ticks = SDL_GetTicks();
    }

    free(grad);
}

void rescale(window *ww, double x, double y, double s) {
    ww->xmin = x - (x - ww->xmin) * s;
    ww->xmax = x + (ww->xmax - x) * s;
    ww->ymin = y - (y - ww->ymin) * s;
    ww->ymax = y + (ww->ymax - y) * s;
}