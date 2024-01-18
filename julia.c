#include "headers.h"
#include "SDL_thread.h"
#include <stdlib.h>
#include <immintrin.h>
#include <math.h>

typedef struct {
  window *win;
  Uint32 *pixels;
  Uint32 *grad;
  double cx;
  double cy;
  int start;
  int step;
  int maxiter;
  int cmax;
} data_julia;

static data_julia *__create_data_julia(window *win, Uint32 *pixels, Uint32 *grad, double cx, double cy, int start, int step, int maxiter, int cmax) {
  data_julia *data = (data_julia*) malloc(sizeof(data_julia));
  if(!data)
    return NULL;
  *data = (data_julia) {win, pixels, grad, cx, cy, start, step, maxiter, cmax};
  return data;
}

static void __load_data_julia(void *data, window **win, Uint32 **pixels, Uint32 **grad, double *cx, double *cy, int *start, int *step, int *maxiter, int *cmax) {
  *win = ((data_julia*) data)->win;
  *pixels = ((data_julia*) data)->pixels;
  *grad = ((data_julia*) data)->grad;
  *cx = ((data_julia*) data)->cx;
  *cy = ((data_julia*) data)->cy;
  *start = ((data_julia*) data)->start;
  *step = ((data_julia*) data)->step;
  *maxiter = ((data_julia*) data)->maxiter;
  *cmax = ((data_julia*) data)->cmax;
}


int thread_julia(void *data) {
  window *win = NULL;
  Uint32 *pixels = NULL;
  Uint32 *grad = NULL;
  double cx;
  double cy;
  int start;
  int step;
  int maxiter;
  int cmax;

  __load_data_julia(data, &win, &pixels, &grad, &cx, &cy, &start, &step, &maxiter, &cmax);

  double *x = (double*) malloc(4 * sizeof(double));
  double *y = (double*) malloc(4 * sizeof(double));
  if(!x || !y)
    return 0;

  union {
    unsigned int full;
    unsigned char bytes[4];
  } mask;

  double dx = (win->xmax - win->xmin) / (win->w - 1);
  double dy = (win->ymax - win->ymin) / (win->h - 1);

  __m256d _cx = _mm256_set1_pd(cx);
  __m256d _cy = _mm256_set1_pd(cy);
  __m256d _two = _mm256_set1_pd(2.0);

  Uint32 *iter = pixels + 4 * start;
  for(int i = 4 * start; i < win->w * win->h; i += 4 * step, iter += 4 * step) {
    mask.full = 0xffffffff;

    for(int j = 0; j < 4; j++) {
      x[j] = win->xmin + ((i + j) % win->w) * dx;
      y[j] = win->ymax - ((i + j) / win->w) * dy;
    }

    __m256d _x = _mm256_loadu_pd(x);
    __m256d _y = _mm256_loadu_pd(y);
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

  free(x);
  free(y);

  return 0;
}

void render_julia(window *win, double cx, double cy, Uint32 *grad, int maxiter, int cmax) {
  Uint32 *pixels;
  int pitch;
  SDL_LockTexture(win->tex, NULL, (void**) &pixels, &pitch);

  SDL_Thread **threads;
  threads = malloc(nthreads * sizeof(SDL_Thread*));
  if(!threads)
    return;
  data_julia **datas;
  datas = malloc(nthreads * sizeof(data_julia*));
  if(!datas)
    return;

  for(int i = 0; i < nthreads; i++) {
    datas[i] = __create_data_julia(win, pixels, grad, cx, cy, i, nthreads, maxiter, cmax);
    threads[i] = SDL_CreateThread(thread_julia, (char*) &i, (void*) datas[i]);
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
