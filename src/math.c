#include "rflib/rflib.h"
#include <math.h>

RFLIB_API float rflib_math_cabs(rflib_complex_t a){
    return sqrtf(a.re * a.re + a.im * a.im);
}

RFLIB_API float rflib_math_carg(rflib_complex_t a) {
    return atan2f(a.im, a.re);
}


RFLIB_API rflib_complex_t 
rflib_math_cmul( rflib_complex_t a,
				 rflib_complex_t b)
{
    rflib_complex_t r;
    r.re = a.re * b.re - a.im * b.im;
    r.im = a.re * b.im + a.im * b.re;
    return r;
}

RFLIB_API void 
rflib_math_scale( const rflib_complex_t *in,
				  rflib_complex_t *out,
                  size_t len, 
                  float s)
{
    if (!in || !out) return;
    for (size_t i = 0; i < len; i++)
    {
        out[i].re = in[i].re * s;
        out[i].im = in[i].im * s;
    }
}

RFLIB_API void 
rflib_math_add( const rflib_complex_t *a,
				const rflib_complex_t *b,
				rflib_complex_t *out,
				size_t len)
{
    if (!a || !b || !out) return;
    for (size_t i = 0; i < len; i++)
    {
        out[i].re = a[i].re + b[i].re;
        out[i].im = a[i].im + b[i].im;
    }
}

RFLIB_API rflib_complex_t
rflib_math_dot( const rflib_complex_t *a,
				const rflib_complex_t *b,
				size_t len)
{
    rflib_complex_t acc = {0.0f, 0.0f};
    if (!a || !b) return acc;
    for (size_t i = 0; i < len; i++) {
        /* a[i] * conj(b[i]) std for correlation/dot */
        acc.re += a[i].re * b[i].re + a[i].im * b[i].im;
        acc.im += a[i].im * b[i].re - a[i].re * b[i].im;
    }
    return acc;
}
