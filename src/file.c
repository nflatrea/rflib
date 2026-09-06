#include "rflib/rflib.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

static float clampf(float v, float lo, float hi)
{
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

RFLIB_API size_t
rflib_file_read_iq( const char *path,
					rflib_iq_format_t fmt,
					rflib_complex_t *out,
					size_t max_len)				
{
    if (!path || !out || max_len == 0) return 0;

    FILE *fp = fopen(path, "rb");
    if (!fp) return 0;

    size_t n = 0;

    switch (fmt)
    {
    case RFLIB_FMT_CF32: {
        float buf[2];
        while (n < max_len && fread(buf, sizeof(float), 2, fp) == 2) {
            out[n].re = buf[0];
            out[n].im = buf[1];
            n++;
        }
        break;
    }
    case RFLIB_FMT_CU8: {
        uint8_t buf[2];
        while (n < max_len && fread(buf, sizeof(uint8_t), 2, fp) == 2) {
            out[n].re = ((float)buf[0] - 127.5f) / 127.5f;
            out[n].im = ((float)buf[1] - 127.5f) / 127.5f;
            n++;
        }
        break;
    }
    case RFLIB_FMT_CS16: {
        int16_t buf[2];
        while (n < max_len && fread(buf, sizeof(int16_t), 2, fp) == 2) {
            out[n].re = (float)buf[0] / 32768.0f;
            out[n].im = (float)buf[1] / 32768.0f;
            n++;
        }
        break;
    }
    default:
        fclose(fp);
        return 0;
    }

    fclose(fp);
    return n;
}

RFLIB_API rflib_status_t 
rflib_file_write_iq( const char *path,
					 rflib_iq_format_t fmt,
					 const rflib_complex_t *in,
					 size_t len)
{
    if (!path || !in) return RFLIB_ERR_NULL_PTR;

    FILE *fp = fopen(path, "wb");
    if (!fp) return RFLIB_ERR_IO;

    rflib_status_t status = RFLIB_OK;

    switch (fmt)
    {
    case RFLIB_FMT_CF32: {
        for (size_t i = 0; i < len; i++) {
            float buf[2] = { in[i].re, in[i].im };
            if (fwrite(buf, sizeof(float), 2, fp) != 2) { status = RFLIB_ERR_IO; break; }
        }
        break;
    }
    case RFLIB_FMT_CU8: {
        for (size_t i = 0; i < len; i++) {
            float re = in[i].re, im = in[i].im;
            re = clampf(re, -1.0f, 1.0f);
            im = clampf(im, -1.0f, 1.0f);
            uint8_t buf[2] = {
                (uint8_t)(re * 127.5f + 127.5f),
                (uint8_t)(im * 127.5f + 127.5f)
            };
            if (fwrite(buf, sizeof(uint8_t), 2, fp) != 2) { status = RFLIB_ERR_IO; break; }
        }
        break;
    }
    case RFLIB_FMT_CS16: {
        for (size_t i = 0; i < len; i++) {
            float re = in[i].re, im = in[i].im;
            re = clampf(re, -1.0f, 1.0f);
            im = clampf(im, -1.0f, 1.0f);
            int16_t buf[2] = { (int16_t)(re * 32767.0f), (int16_t)(im * 32767.0f) };
            if (fwrite(buf, sizeof(int16_t), 2, fp) != 2) { status = RFLIB_ERR_IO; break; }
        }
        break;
    }
    default:
        status = RFLIB_ERR_INVALID_ARG;
        break;
    }

    fclose(fp);
    return status;
}
