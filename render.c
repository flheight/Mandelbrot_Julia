#include "headers.h"

void render_loop(SDL_Event events, SDL_Renderer *renderer1, SDL_Renderer *renderer2, window *ww1, window *ww2, int maxiter, int cmax) {
	Uint32 *grad = gradient(cmax);
    if(!grad)
        return;

    render_mandelbrot(renderer1, ww1, grad, maxiter, cmax);

    int running = 1;
    double cx = 0;
    double cy = 0;
    int posx;
    int posy;

    while(running) {
        while(SDL_PollEvent(&events)) {
            switch(events.type) {
                case SDL_QUIT:
                running = 0;
                break;
                case SDL_KEYDOWN:
                switch(events.key.keysym.sym) {
                    case 'q':
                    running = 0;
                    break;
                }
                break; 
                case SDL_MOUSEBUTTONDOWN:
                switch(events.button.button) {
                    case SDL_BUTTON_LEFT:
                    if(SDL_GetMouseFocus() != ww1->win)
                      break;
                  while(events.type != SDL_MOUSEBUTTONUP) {
                      SDL_PollEvent(&events);
                      SDL_GetMouseState(&posx, &posy);
                      cx = ww1->xmin + (ww1->xmax - ww1->xmin) * posx / (ww1->w - 1);
                      cy = ww1->ymax - (ww1->ymax - ww1->ymin) * posy / (ww1->h - 1);
                      render_julia(renderer2, ww2, cx, cy, grad, maxiter, cmax);     
                  }
                  break;
              }
              break;
              case SDL_MOUSEWHEEL:
              if(events.wheel.y == 0)
                break;
            SDL_GetMouseState(&posx, &posy);
            if(SDL_GetMouseFocus() == ww1->win) {
                double x = ww1->xmin + (ww1->xmax - ww1->xmin) * posx / (ww1->w - 1);
                double y = ww1->ymax - (ww1->ymax - ww1->ymin) * posy / (ww1->h - 1);
                if(events.wheel.y > 0)
                    rescale(ww1, x, y, scalefactor);
                else 
                    rescale(ww1, x, y, 1 / scalefactor);
                render_mandelbrot(renderer1, ww1, grad, maxiter, cmax);
            }
            else {
                double x = ww2->xmin + (ww2->xmax - ww2->xmin) * posx / (ww2->w - 1);
                double y = ww2->ymax - (ww2->ymax - ww2->ymin) * posy / (ww2->h - 1);
                if(events.wheel.y > 0)
                    rescale(ww2, x, y, scalefactor);
                else 
                    rescale(ww2, x, y, 1 / scalefactor);
                render_julia(renderer2, ww2, cx, cy, grad, maxiter, cmax);
            }            
            break;
        }
    }
}

free(grad);
}

void rescale(window *ww, double x, double y, double s) {
    ww->xmin = x - (x - ww->xmin) * s;
    ww->xmax = x + (ww->xmax - x) * s;
    ww->ymin = y - (y - ww->ymin) * s;
    ww->ymax = y + (ww->ymax - y) * s;
}