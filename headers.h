#include <SDL.h>

#define bailout 0xf000000000000000
#define scalefactor .9
#define nthreads 32

typedef struct {
	SDL_Window *win;
	SDL_Renderer *ren;
	SDL_Texture *tex;
	int w; 
	int h;
	double xmin;
	double xmax;
	double ymin;
	double ymax;
} window;

Uint32 *gradient(int cmax);
void render_mandelbrot(window *ww, Uint32 *grad, int maxiter, int cmax);
void render_julia(window *ww, double cx, double cy, Uint32 *grad, int maxiter, int cmax);
void render_loop(window *ww1, window *ww2, int maxiter, int cmax);
void rescale(window *ww, double x, double y, double s);