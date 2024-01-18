#include "headers.h"
#include "SDL_thread.h"
#include <stdlib.h>
#include <immintrin.h>
#include <math.h>

typedef struct {
  window *win;
  Uint32 *pixels;
  Uint32 *grad;
  int start;
  int step;
  int maxiter;
  int cmax;
} data_mandelbrot;

static data_mandelbrot *__create_data_mandelbrot(window *win, Uint32 *pixels, Uint32 *grad, int start, int step, int maxiter, int cmax) {
  data_mandelbrot *data = (data_mandelbrot*) malloc(sizeof(data_mandelbrot));
  if(!data)
    return NULL;
  *data = (data_mandelbrot) {win, pixels, grad, start, step, maxiter, cmax};
  return data;
}

static void __load_data_mandelbrot(void *data, window **win, Uint32 **pixels, Uint32 **grad, int *start, int *step, int *maxiter, int *cmax) {
  *win = ((data_mandelbrot*) data)->win;
  *pixels = ((data_mandelbrot*) data)->pixels;
  *grad = ((data_mandelbrot*) data)->grad;
  *start = ((data_mandelbrot*) data)->start;
  *step = ((data_mandelbrot*) data)->step;
  *maxiter = ((data_mandelbrot*) data)->maxiter;
  *cmax = ((data_mandelbrot*) data)->cmax;
}

static int thread_mandelbrot(void *data) {
  window *win = NULL;
  Uint32 *pixels = NULL;
  Uint32 *grad = NULL;
  int start;
  int step;
  int maxiter;
  int cmax;

  __load_data_mandelbrot(data, &win, &pixels, &grad, &start, &step, &maxiter, &cmax);

  double *cx = (double*) malloc(4 * sizeof(double));
  double *cy = (double*) malloc(4 * sizeof(double));
  if(!cx || !cy)
    return 0;

  union {
    unsigned int full;
    unsigned char bytes[4];
  } mask;

  double dx = (win->xmax - win->xmin) / (win->w - 1);
  double dy = (win->ymax - win->ymin) / (win->h - 1);

  __m256d _two = _mm256_set1_pd(2.0);

  Uint32 *iter = pixels + 4 * start;
  for(int i = 4 * start; i < win->w * win->h; i += 4 * step, iter += 4 * step) {
    mask.full = 0xffffffff;

    for(int j = 0; j < 4; j++) {
      cx[j] = win->xmin + ((i + j) % win->w) * dx;
      cy[j] = win->ymax - ((i + j) / win->w) * dy;
    }

    __m256d _cx = _mm256_loadu_pd(cx);
    __m256d _cy = _mm256_loadu_pd(cy);
    __m256d _x = _mm256_setzero_pd();
    __m256d _y = _mm256_setzero_pd();
    __m256d _x2;
    __m256d _y2;
    __m256d _z2;

    int idx = 0;
    while((mask.full > 0) && (idx < maxiter)) {
      _x2 = _mm256_mul_pd(_x, _x);
      _y2 = _mm256_fmsub_pd(_y, _y, _cx);
      _y = _mm256_mul_pd(_two, _y);
      _y = _mm256_fmadd_pd(_x, _y, _cy);
      _x = _mm256_sub_pd(_x2, _y2);

      _z2 = _mm256_add_pd(_x2, _y2);
      double *z2 = (double*) &_z2;

      for(int j = 0; (j < 4) && (i + j < win->w * win->h); j++) {
        if(!mask.bytes[j])
          continue;
        if(z2[j] >= bailout) {
          mask.bytes[j] = 0;
          double nu = idx + 1 - log2(log(z2[j]));
          nu = nu < 0 ? 0 : nu;
          *(iter + j) = grad[(int) (cmax * (nu / maxiter))];
        }
      }

      idx++;
    }

    for(int j = 0; (j < 4) && (i + j < win->w * win->h); j++) {
      if(!mask.bytes[j])
        continue;
      *(iter + j) = grad[cmax - 1];
    }
  }

  free(cx);
  free(cy);

  return 0;
}

void render_mandelbrot(window *win, Uint32 *grad, int maxiter, int cmax) {
  Uint32 *pixels;
  int pitch;
  SDL_LockTexture(win->tex, NULL, (void**) &pixels, &pitch);
  
  SDL_Thread **threads;
  threads = malloc(nthreads * sizeof(SDL_Thread*));
  if(!threads)
    return;
  data_mandelbrot **datas;
  datas = malloc(nthreads * sizeof(data_mandelbrot*));
  if(!datas)
    return;

  for(int i = 0; i < nthreads; i++) {
    datas[i] = __create_data_mandelbrot(win, pixels, grad, i, nthreads, maxiter, cmax);
    threads[i] = SDL_CreateThread(thread_mandelbrot, (char*) &i, (void*) datas[i]);
  }

  for(int i = 0; i < nthreads; i++) {
    SDL_WaitThread(threads[i], NULL);  
    free(datas[i]);
  }

  free(threads);
  free(datas);

  SDL_UnlockTexture(win->tex);
  SDL_RenderCopy(win->ren, win->tex, NULL, NULL);
  SDL_RenderPresent(win->ren);
}

