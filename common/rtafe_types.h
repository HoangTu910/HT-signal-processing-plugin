#ifndef RTAFE_TYPES_H
#define RTAFE_TYPES_H

#include <stdint.h>
#include <math.h>
#include <stdio.h>

typedef uint8_t  u8;
typedef uint32_t u32;
typedef uint16_t u16;
typedef int32_t  s32;
typedef int16_t  s16;
typedef int8_t   s8;
typedef uint64_t u64;
typedef int64_t  s64;
typedef s32      tFixed;
typedef u8       tByte;
typedef float    tFloat;

#define FLOAT_TO_Q15(x) ((s16)((x) * 32768.0f))
#define FLOAT_TO_Q31(x) ((s32)((x) * 2147483648.0f))
#define FLOAT_TO_Q2_14(x) ((s32)((x) * 16384.0f))
#define Q2_14_TO_FLOAT(x) ((float)(x) / 16384.0f)

#define Q8_8_TO_FLOAT(x) ((float)(x) / 256.0f)
#define FLOAT_TO_Q8_8(x) ((s16)((x) * 256.0f))

#define Q2_14_SHIFT 14

#define CLIP_S16(x) ((x) > INT16_MAX ? INT16_MAX : ((x) < INT16_MIN ? INT16_MIN : (s16)(x)))
#define CLIP_S32(x) ((x) > INT32_MAX ? INT32_MAX : ((x) < INT32_MIN ? INT32_MIN : (s32)(x)))
#define SAMPLING_RATE (48000.0)

#define M_PI		3.14159265358979323846	/* pi */
#define M_PI_2		1.57079632679489661923	/* pi/2 */

#define FE_LOG(fmt, ...) printf(fmt, ##__VA_ARGS__)
#define FE_WARN(fmt, ...) fprintf(stderr, "WARN: " fmt, ##__VA_ARGS__)
#define FE_ERROR(fmt, ...) fprintf(stderr, "ERROR: " fmt, ##__VA_ARGS__)

#define SIZE_OF_DSP_SAMPLE sizeof(sample_t)

#define DC_MODULE_PARAMS_COUNT 1
#define PRE_EMPHASIS_MODULE_PARAMS_COUNT 1
#define NOISE_SUPPRESS_MODULE_PARAMS_COUNT 1

#define LO_BYTE(x) ((x) & 0xFF)
#define HI_BYTE(x) ((x) >> 8 & 0xFF)
#define MAKE_U16(high_byte, low_byte) (((high_byte) << 8) | ((low_byte) & 0xFF))

#ifdef FIXED_POINT
typedef tFixed rtafe_sample_t;
#else
typedef tFloat rtafe_sample_t;
#endif
typedef rtafe_sample_t sample_t;

typedef struct rtafe_process_buf {
    void*  buffer;
    size_t buf_size;
} rtafe_process_buf_t;

enum ChannelId : u16 {
    kMono,
    kLeftFront,
    kRightFront,
    kLeftRear,
    kRightRear,
    kCenter
};

#endif /* RTAFE_TYPES_H */