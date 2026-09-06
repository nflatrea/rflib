#ifndef RFLIB_H
#define RFLIB_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_WIN32) && defined(RFLIB_SHARED)
#	ifdef RFLIB_BUILDING
#		define RFLIB_API __declspec(dllexport)
#	else
#		define RFLIB_API __declspec(dllimport)
#	endif
#else
#	define RFLIB_API
#endif

#define RFLIB_ABI_VERSION 4

typedef struct { float re, im; } rflib_complex_t;

typedef enum {

    RFLIB_OK              =  0,
    RFLIB_ERR_NULL_PTR    = -1,
    RFLIB_ERR_INVALID_ARG = -2,
    RFLIB_ERR_IO          = -3,
    RFLIB_ERR_NOMEM       = -4

} rflib_status_t;

RFLIB_API int rflib_abi_version(void);

// math (src/math.c)

RFLIB_API float rflib_math_cabs(rflib_complex_t a);
RFLIB_API float rflib_math_carg(rflib_complex_t a);

RFLIB_API rflib_complex_t
rflib_math_cmul( rflib_complex_t a, 
				 rflib_complex_t b);

RFLIB_API void  
rflib_math_scale( const rflib_complex_t *in, 
				  rflib_complex_t *out,
				  size_t len, 
				  float s);

RFLIB_API void
rflib_math_add( const rflib_complex_t *a,
				const rflib_complex_t *b,
                rflib_complex_t *out,
                size_t len);
                
RFLIB_API rflib_complex_t
rflib_math_dot( const rflib_complex_t *a,
				const rflib_complex_t *b,
				size_t len);

// window (src/window.c)

RFLIB_API void rflib_window_hamming(float *w, size_t n);
RFLIB_API void rflib_window_hann(float *w, size_t n);
RFLIB_API void rflib_window_kaiser(float *w, size_t n, float beta);

/*
 * IQ file I/O (src/file.c) 
 * Kept separate from util on purpose: this is the only part an 
 * embedded build would ever want to compile out later.
*/

typedef enum {

    RFLIB_FMT_CF32 = 0, /* interleaved float32 I/Q 				*/
    RFLIB_FMT_CU8  = 1, /* interleaved uint8 I/Q, offset-binary */
    RFLIB_FMT_CS16 = 2  /* interleaved int16 I/Q 				*/

} rflib_iq_format_t ;


RFLIB_API size_t
rflib_file_read_iq( const char *path,
					rflib_iq_format_t fmt,
					rflib_complex_t *out,
					size_t max_len);
					
RFLIB_API rflib_status_t
rflib_file_write_iq( const char *path,
					 rflib_iq_format_t fmt,
					 const rflib_complex_t *in,
					 size_t len);

/*
 * taps (src/taps.c)
 * Windowed-sinc + Kaiser only (Parks-McClellan not needed for now)
 * fc, trans_bw are normalized to sample rate 0 < fc < 0.5
 * Returns the number of taps written (0 on error).
*/
 
RFLIB_API size_t 
rflib_taps_lowpass( float fc,
					float trans_bw,
					float atten_db,
					rflib_complex_t *taps,
					size_t max_taps);

/* 
 * FIR filter, complex (src/fir.c)
 * Opaque handle, lifecycle:
 * create
 * 	   \____ execute*
 * 			      \____ destroy.
 * execute() performs zero allocations.
 */

typedef struct rflib_fir_s *rflib_fir_t;

RFLIB_API rflib_fir_t 
rflib_fir_create( const rflib_complex_t *taps,
				  size_t num_taps);

RFLIB_API rflib_status_t
rflib_fir_execute( rflib_fir_t f,
				  const rflib_complex_t *in,
				  rflib_complex_t *out,
				  size_t len);
				  
RFLIB_API void rflib_fir_reset(rflib_fir_t f);
RFLIB_API void rflib_fir_destroy(rflib_fir_t f);				

#endif
