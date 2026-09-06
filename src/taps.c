#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#include "rflib/rflib.h"

#include <math.h>
#include <stdlib.h>

/*
 * Kaiser beta-from-attenuation formula (1974) 
 * https://en.wikipedia.org/wiki/Kaiser_window
 * https://docs.scipy.org/doc/scipy-1.15.0/reference/generated/scipy.signal.kaiser_beta.html
*/
static float kaiser_beta(float atten_db)
{
    if (atten_db > 50.0f) {
        return 0.1102f * (atten_db - 8.7f);
    } else if (atten_db >= 21.0f) {
        return 0.5842f * powf(atten_db - 21.0f, 0.4f) + 0.07886f * (atten_db - 21.0f);
    }
    return 0.0f;
}

/*
 * standard Kaiser tap-count estimate.
 * trans_bw is normalized (cycles/sample). 
*/
static size_t kaiser_num_taps(float trans_bw, float atten_db)
{
    if (trans_bw <= 0.0f) return 0;
    float n = (atten_db - 7.95f) / (2.285f * 2.0f * (float)M_PI * trans_bw) + 1.0f;
    size_t taps = (size_t)ceilf(n);
    if (taps % 2 == 0) taps += 1;  /* force odd length to symmetric, integer group delay */
    return taps;
}

RFLIB_API size_t
rflib_design_lowpass( float fc,
					  float trans_bw,
					  float atten_db,
					  rflib_complex_t *taps,
					  size_t max_taps)
{
    if (!taps || max_taps == 0) return 0;
    if (fc <= 0.0f || fc >= 0.5f) return 0;
    if (trans_bw <= 0.0f) return 0;

    size_t n = kaiser_num_taps(trans_bw, atten_db);
    if (n == 0 || n > max_taps) return 0;

    float beta = kaiser_beta(atten_db);
    float *win = NULL;
    
    /* Small stack buffer for typical filters; fall back to a heap buffer only
     * for unusually long ones. This is design-time (create-adjacent), not
     * execute(), so an allocation here doesn't violate the hot-path rule. */
    float stack_win[512];
    int heap_used = 0;
    if (n <= 512) {
        win = stack_win;
    } else {
        win = (float *)malloc(n * sizeof(float));
        if (!win) return 0;
        heap_used = 1;
    }

    rflib_window_kaiser(win, n, beta);

    float m = (float)(n - 1) / 2.0f;
    for (size_t i = 0; i < n; i++) {
        float x = (float)i - m;
        float h;
        if (x == 0.0f) {
            h = 2.0f * fc;
        } else {
            h = sinf(2.0f * (float)M_PI * fc * x) / ((float)M_PI * x);
        }
        taps[i].re = h * win[i];
        taps[i].im = 0.0f;
    }

    if (heap_used) free(win);
    return n;
}
