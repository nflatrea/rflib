#include "rflib/rflib.h"

#include <stdlib.h>
#include <string.h>

struct rflib_fir_s
{
    rflib_complex_t *taps;     	/* copy caller's taps for us  */
    size_t           num_taps;
    rflib_complex_t *hist;     	/* ring buffer, size num_taps */
    size_t           hist_pos;	/* next write index into hist */
};

RFLIB_API rflib_fir_t
rflib_fir_create( const rflib_complex_t *taps,
				  size_t num_taps)
{
    if (!taps || num_taps == 0) return NULL;

    struct rflib_fir_s *f = (struct rflib_fir_s *)calloc(1, sizeof(*f));
    if (!f) return NULL;

    f->taps = (rflib_complex_t *)malloc(num_taps * sizeof(rflib_complex_t));
    f->hist = (rflib_complex_t *)calloc(num_taps, sizeof(rflib_complex_t));
    if (!f->taps || !f->hist) {
        free(f->taps);
        free(f->hist);
        free(f);
        return NULL;
    }

    memcpy(f->taps, taps, num_taps * sizeof(rflib_complex_t));
    f->num_taps = num_taps;
    f->hist_pos = 0;
    return f;
}

RFLIB_API rflib_status_t
rflib_fir_execute( rflib_fir_t f,
				   const rflib_complex_t *in,
				   rflib_complex_t *out,
				   size_t len)
{
    if (!f || !in || !out) return RFLIB_ERR_NULL_PTR;

    size_t nt = f->num_taps;
    for (size_t i = 0; i < len; i++) {
        f->hist[f->hist_pos] = in[i];

        rflib_complex_t acc = {0.0f, 0.0f};
        size_t idx = f->hist_pos;
        for (size_t k = 0; k < nt; k++) {
            const rflib_complex_t *s = &f->hist[idx];
            const rflib_complex_t *h = &f->taps[k];
            acc.re += s->re * h->re - s->im * h->im;
            acc.im += s->re * h->im + s->im * h->re;
            idx = (idx == 0) ? nt - 1 : idx - 1;
        }
        out[i] = acc;

        f->hist_pos = (f->hist_pos + 1 == nt) ? 0 : f->hist_pos + 1;
    }
    return RFLIB_OK;
}

RFLIB_API void rflib_fir_reset(rflib_fir_t f)
{
    if (!f) return;
    memset(f->hist, 0, f->num_taps * sizeof(rflib_complex_t));
    f->hist_pos = 0;
}

RFLIB_API void rflib_fir_destroy(rflib_fir_t f)
{
    if (!f) return;
    free(f->taps);
    free(f->hist);
    free(f);
}
