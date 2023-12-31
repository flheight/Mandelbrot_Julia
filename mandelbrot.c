#include "headers.h"
#include "SDL_thread.h"
#include <stdlib.h>
#include <immintrin.h>
#include <math.h>

typedef struct {
  window *ww;
  Uint32 *pixels;
  Uint32 *grad;
  int start;
  int step;
  int maxiter;
  int cmax;
} data_mandelbrot;

static data_mandelbrot *create_data_mandelbrot(window *ww, Uint32 *pixels, Uint32 *grad, int start, int step, int maxiter, int cmax) {
  data_mandelbrot *data = (data_mandelbrot*) malloc(sizeof(data_mandelbrot));
  if(!data)
    return NULL;
  *data = (data_mandelbrot) {ww, pixels, grad, start, step, maxiter, cmax};
  return data;
}

static void load_data_mandelbrot(void *data, window **ww, Uint32 **pixels, Uint32 **grad, int *start, int *step, int *maxiter, int *cmax) {
  *ww = ((data_mandelbrot*) data)->ww;
  *pixels = ((data_mandelbrot*) data)->pixels;
  *grad = ((data_mandelbrot*) data)->grad;
  *start = ((data_mandelbrot*) data)->start;
  *step = ((data_mandelbrot*) data)->step;
  *maxiter = ((data_mandelbrot*) data)->maxiter;
  *cmax = ((data_mandelbrot*) data)->cmax;
}

static int thread_mandelbrot(void *data) {
  window *ww = NULL;
  Uint32 *pixels = NULL;
  Uint32 *grad = NULL;
  int start;
  int step;
  int maxiter;
  int cmax;

  load_data_mandelbrot(data, &ww, &pixels, &grad, &start, &step, &maxiter, &cmax);

  double *cx = (double*) malloc(4 * sizeof(double));
  double *cy = (double*) malloc(4 * sizeof(double));
  if(!cx || !cy)
    return 0;

  char mask;
  double dx = (ww->xmax - ww->xmin) / (ww->w - 1);
  double dy = (ww->ymax - ww->ymin) / (ww->h - 1);

  __m256d _two = _mm256_set1_pd(2.0);

  Uint32 *iter = pixels + 4 * start;
  for(int i = 4 * start; i < ww->w * ww->h; i += 4 * step, iter += 4 * step) {
    mask = 0xf;

    for(int j = 0; j < 4; j++) {
      cx[j] = ww->xmin + ((i + j) % ww->w) * dx;
      cy[j] = ww->ymax - ((i + j) / ww->w) * dy;
    }

    __m256d _cx = _mm256_loadu_pd(cx);
    __m256d _cy = _mm256_loadu_pd(cy);
    __m256d _x = _mm256_setzero_pd();
    __m256d _y = _mm256_setzero_pd();
    __m256d _x2;
    __m256d _y2;
    __m256d _z2;

    int idx = 0;
    while((mask > 0) && (idx < maxiter)) {
      _x2 = _mm256_mul_pd(_x, _x);
      _y2 = _mm256_fmsub_pd(_y, _y, _cx);
      _y = _mm256_mul_pd(_two, _y);
      _y = _mm256_fmadd_pd(_x, _y, _cy);
      _x = _mm256_sub_pd(_x2, _y2);

      _z2 = _mm256_add_pd(_x2, _y2);
      double *z2 = (double*) &_z2;

      for(int j = 0; (j < 4) && (i + j < ww->w * ww->h); j++) {
        if(!((mask >> j) & 1))
          continue;
        if(z2[j] >= bailout) {
          mask ^= (1 << j);
          double nu = idx + 1 - log2(log(z2[j]));
          nu = nu < 0 ? 0 : nu;
          *(iter + j) = grad[(int) (cmax * (nu / maxiter))];
        }
      }

      idx++;
    }

    for(int j = 0; (j < 4) && (i + j < ww->w * ww->h); j++) {
      if(!((mask >> j) & 1))
        continue;
      *(iter + j) = grad[cmax - 1];
    }
  }

  free(cx);
  free(cy);

  return 0;
}

void render_mandelbrot(window *ww, Uint32 *grad, int maxiter, int cmax) {
  Uint32 *pixels;
  int pitch;
  SDL_LockTexture(ww->tex, NULL, (void**) &pixels, &pitch);
  
  SDL_Thread **threads;
  threads = malloc(nthreads * sizeof(SDL_Thread*));
  if(!threads)
    return;
  data_mandelbrot **datas;
  datas = malloc(nthreads * sizeof(data_mandelbrot*));
  if(!datas)
    return;

  for(int i = 0; i < nthreads; i++) {
    datas[i] = create_data_mandelbrot(ww, pixels, grad, i, nthreads, maxiter, cmax);
    threads[i] = SDL_CreateThread(thread_mandelbrot, (char*) &i, (void*) datas[i]);
  }

  for(int i = 0; i < nthreads; i++) {
    SDL_WaitThread(threads[i], NULL);  
    free(datas[i]);
  }

  free(threads);
  free(datas);

  SDL_UnlockTexture(ww->tex);
  SDL_RenderCopy(ww->ren, ww->tex, NULL, NULL);
  SDL_RenderPresent(ww->ren);
}

