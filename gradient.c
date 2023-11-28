#include "headers.h"

Uint32 *gradient(int cmax) {
    Uint32 *grad = (Uint32*) malloc(cmax * sizeof(Uint32));
    if(!grad)
        return NULL;

    for(int i = 0; i < cmax / 3; i++) {
        char c = (i * 255) / (cmax / 3);
        grad[i] = (0xff << 24) | (c << 16);
        grad[cmax / 3 + i] = (0xff << 24) | (0xff << 16) | (c << 8);
        grad[2 * (cmax / 3) + i] = (0xff << 24) | (0xff << 16) | (0xff << 8) | c;
    }  

    for(int i = 3 * (cmax / 3); i < cmax; i++)
        grad[i] = (0xff << 24) | (0xff << 16) | (0xff << 8) | 0xff;
    
    return grad;
}