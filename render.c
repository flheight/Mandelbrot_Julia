#include "headers.h"

static void __frametime_millis_ensure(Uint64 *prev_ticks, const Uint64 millis) {
    Uint64 current_ticks = SDL_GetTicks64();
    Uint64 dticks = current_ticks - *prev_ticks;
    if(dticks >= millis) {
        *prev_ticks = current_ticks;
        return;
    }
    SDL_Delay(millis - dticks);
    *prev_ticks = SDL_GetTicks64();
}

void render_loop(window *win1, window *win2, int maxiter, int cmax) {
	Uint32 *grad = gradient(cmax);
    if(!grad)
        return;

    SDL_Event event;
    Uint64 ticks = SDL_GetTicks();

    render_mandelbrot(win1, grad, maxiter, cmax);

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
                        if(SDL_GetMouseFocus() != win1->win)
                            break;
                    while(event.type != SDL_MOUSEBUTTONUP) {
                        SDL_PollEvent(&event);
                        SDL_GetMouseState(&posx, &posy);
                        cx = win1->xmin + (win1->xmax - win1->xmin) * posx / (win1->w - 1);
                        cy = win1->ymax - (win1->ymax - win1->ymin) * posy / (win1->h - 1);
                        render_julia(win2, cx, cy, grad, maxiter, cmax);     
                    }
                }
                break;
            case SDL_MOUSEWHEEL:
                if(event.wheel.y == 0)
                    break;
            SDL_GetMouseState(&posx, &posy);
            if(SDL_GetMouseFocus() == win1->win) {
                double x = win1->xmin + (win1->xmax - win1->xmin) * posx / (win1->w - 1);
                double y = win1->ymax - (win1->ymax - win1->ymin) * posy / (win1->h - 1);
                if(event.wheel.y > 0)
                    rescale(win1, x, y, scalefactor);
                else 
                    rescale(win1, x, y, 1 / scalefactor);
                render_mandelbrot(win1, grad, maxiter, cmax);
            }
            else {
                double x = win2->xmin + (win2->xmax - win2->xmin) * posx / (win2->w - 1);
                double y = win2->ymax - (win2->ymax - win2->ymin) * posy / (win2->h - 1);
                if(event.wheel.y > 0)
                    rescale(win2, x, y, scalefactor);
                else 
                    rescale(win2, x, y, 1 / scalefactor);
                render_julia(win2, cx, cy, grad, maxiter, cmax);
            }            
        }
        
        __frametime_millis_ensure(&ticks, frametime);
    }

    free(grad);
}

void rescale(window *win, double x, double y, double s) {
    win->xmin = x - (x - win->xmin) * s;
    win->xmax = x + (win->xmax - x) * s;
    win->ymin = y - (y - win->ymin) * s;
    win->ymax = y + (win->ymax - y) * s;
}