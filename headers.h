#include <SDL.h>

#define bailout (1 << 16)
#define scalefactor .9
#define nthreads 32

typedef struct {
	SDL_Window *win;
	int w; 
	int h;
	double xmin;
	double xmax;
	double ymin;
	double ymax;
} window;

typedef struct {
	window *ww;
	Uint32 *pixels;
	Uint32 *grad;
	int start;
	int step;
	int maxiter;
	int cmax;
} data_mandelbrot;

typedef struct {
	window *ww;
	Uint32 *pixels;
	Uint32 *grad;
	double cx;
	double cy;
	int start;
	int step;
	int maxiter;
	int cmax;
} data_julia;

extern SDL_Renderer *renderer;
Uint32 *gradient(int cmax);
void render_mandelbrot(SDL_Renderer *renderer, window *ww, Uint32 *grad, int maxiter, int cmax);
void render_julia(SDL_Renderer *renderer, window *ww, double cx, double cy, Uint32 *grad, int maxiter, int cmax);
void render_loop(SDL_Event events, SDL_Renderer *renderer1, SDL_Renderer *renderer2, window *ww1, window *ww2, int maxiter, int cmax);
void rescale(window *ww, double x, double y, double s);
data_mandelbrot *create_data_mandelbrot(window *ww, Uint32 *pixels, Uint32 *grad, int start, int step, int maxiter, int cmax);
void load_data_mandelbrot(void *data, window **ww, Uint32 **pixels, Uint32 **grad, int *start, int *step, int *maxiter, int *cmax);
int thread_mandelbrot(void *data);
data_julia *create_data_julia(window *ww, Uint32 *pixels, Uint32 *grad, double cx, double cy, int start, int step, int maxiter, int cmax);
void load_data_julia(void *data, window **ww, Uint32 **pixels, Uint32 **grad, double *cx, double *cy, int *start, int *step, int *maxiter, int *cmax);
int thread_julia(void *data);