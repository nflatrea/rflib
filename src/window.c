#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#include "rflib/rflib.h"
#include <math.h>

RFLIB_API void rflib_window_hamming(float *w, size_t n)
{
    if (!w || n == 0) return;
    if (n == 1) { w[0] = 1.0f; return; }
    for (size_t i = 0; i < n; i++) {
        w[i] = 0.54f - 0.46f * cosf(2.0f * (float)M_PI * (float)i / (float)(n - 1));
    }
}

RFLIB_API void rflib_window_hann(float *w, size_t n)
{
    if (!w || n == 0) return;
    if (n == 1) { w[0] = 1.0f; return; }
    for (size_t i = 0; i < n; i++) {
        w[i] = 0.5f - 0.5f * cosf(2.0f * (float)M_PI * (float)i / (float)(n - 1));
    }
}

/* 
 * mod bessel function of the first kind, order 0. Series expansion,
 * converges fast for the beta range kaiser windows use (0->20).
*/

static float bessel_i0(float x)
{
    float sum 	= 1.0f;
    float term 	= 1.0f;
    float xh 	= x / 2.0f;

    for (int k = 1; k < 32; k++)
    {
        term *= (xh / (float)k) * (xh / (float)k);
        sum += term;
        if (term < 1e-9f * sum) break;
    }
    return sum;
}

RFLIB_API void
rflib_window_kaiser( float *w, 
					 size_t n,
					 float beta)
{
    if (!w || n == 0) return;
    if (n == 1) { w[0] = 1.0f; return; }
    float denom = bessel_i0(beta);
    float m = (float)(n - 1);
    for (size_t i = 0; i < n; i++)
    {
        float r = (2.0f * (float)i - m) / m;  /* mean [-1;1] */
        float arg = beta * sqrtf(1.0f - r * r);
        w[i] = bessel_i0(arg) / denom;
    }
}
