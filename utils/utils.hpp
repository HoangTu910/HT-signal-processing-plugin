#ifndef UTILS_HPP
#define UTILS_HPP

#include <stdint.h>
#include "common/rtafe_types.h"
#ifdef RTAFE_BARE_METAL
#ifdef __cplusplus
extern "C" {
#endif
float roundf(float value);
#ifdef __cplusplus
}
#endif
#else
#include <math.h>
#endif

typedef struct sincos_t
{
    float sin, cos;
} sincos_t;

static inline float fast_sin_poly(float x)
{
    /* https://uli.rocks/p/polynomial-approximation/ */
    float x2 = x * x;
    return x * (0.98786f + x2 * (-0.15527f + x2 * 0.005643f));
}

static inline sincos_t fast_sine_cos(float x)
{
    // Range reduce to [-pi, pi]
    const float inv_two_pi = 0.15915494309189535f; // 1 / (2*pi)
    const float two_pi = 6.283185307179586f;

    float k = roundf(x * inv_two_pi);
    x = x - k * two_pi;

    // Map to [-pi/2, pi/2]
    float sign_sin = 1.0f;
    float sign_cos = 1.0f;

    if (x > M_PI_2) {
        x = M_PI - x;
        sign_cos = -1.0f;
    } else if (x < -M_PI_2) {
        x = -M_PI - x;
        sign_cos = -1.0f;
    }

    float x2 = x * x;

    // Minimax polynomial (degree 5)
    float sin_poly = x * (1.0f + x2 * (-0.16666667f + x2 * 0.0083333310f));
    float cos_poly = 1.0f + x2 * (-0.5f + x2 * 0.041666638f);

    sincos_t out;
    out.sin = sign_sin * sin_poly;
    out.cos = sign_cos * cos_poly;

    return out;
}

#if defined(__has_include)
#  if __has_include("utils/fft.h")
#    include "utils/fft.h"
#  elif __has_include("fft.h")
#    include "fft.h"
#  endif
#else
#  include "fft.h"
#endif

#endif  /* UTILS_HPP */