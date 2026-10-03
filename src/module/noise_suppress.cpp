#include "noise_suppress.hpp"
#ifndef RTAFE_BARE_METAL
#include <cstdio>
#include <cstring>
#include <cmath>
#else
extern "C" {
void *memset(void *destination, int value, unsigned long size);
void *memcpy(void *destination, const void *source, unsigned long size);
float fmaxf(float left, float right);
}
#endif

NoiseSuppress::NoiseSuppress()
    : min_stat_state_{}
{
    Reset();
}

void NoiseSuppress::InitNoiseState(int frame_size)
{
    if (!min_stat_state_.is_initialized || min_stat_state_.last_frame_size != frame_size) {
        memset(min_stat_state_.smooth, 0, sizeof(float) * frame_size);
        min_stat_state_.is_initialized = true;
        min_stat_state_.last_frame_size = frame_size;
    }
}

 rtafe_module_status_t NoiseSuppress::Init(
    const rtafe_module_properties_t *properties)
{
    if (properties == nullptr) {
        return kRtafeStatusInvalidArgument;
    }

    properties_ = *properties;
    return Reset();
}

 rtafe_module_status_t NoiseSuppress::Process(
    const rtafe_process_buf_t *input,
    rtafe_process_buf_t *output)
{
    if (input == nullptr || output == nullptr || input->buffer == nullptr ||
        output->buffer == nullptr || input->buf_size != output->buf_size ||
        input->buf_size != properties_.frame_samples ||
        input->buf_size != MAX_FRAME_SIZE / 2) {
        return kRtafeStatusInvalidBuffer;
    }

    rtafe_sample_t *in_buf =
        static_cast<rtafe_sample_t *>(input->buffer);
    rtafe_sample_t *out_buf =
        static_cast<rtafe_sample_t *>(output->buffer);

    const int hop = static_cast<int>(input->buf_size);
    const int frame_size = hop * 2;
    InitNoiseState(frame_size);

    float windowed[MAX_FRAME_SIZE];
    for (int i = 0; i < hop; ++i) {
        windowed[i] = overlap_in_[i];
        windowed[hop + i] = static_cast<float>(in_buf[i]);
        overlap_in_[i] = static_cast<float>(in_buf[i]);
    }

    hanning_window(windowed, frame_size);

    complex_t Y[MAX_FRAME_SIZE];
    for (int i = 0; i < frame_size; ++i) {
        Y[i].real = windowed[i];
        Y[i].imag = 0.0f;
    }
    fft(Y, frame_size);

    float Pyy[MAX_FRAME_SIZE];
    for (int i = 0; i < frame_size; ++i) {
        float power = Y[i].real * Y[i].real + Y[i].imag * Y[i].imag;
        min_stat_state_.smooth[i] =
            ALPHA_SMOOTH * min_stat_state_.smooth[i] +
            (1.0f - ALPHA_SMOOTH) * power;
        Pyy[i] = min_stat_state_.smooth[i];
    }

    memcpy(min_stat_state_.min_buffer[min_stat_state_.min_buf_idx],
           Pyy, sizeof(float) * frame_size);
    min_stat_state_.min_buf_idx =
        (min_stat_state_.min_buf_idx + 1) % MINSTAT_WINDOW;
    if (min_stat_state_.min_buf_cnt < MINSTAT_WINDOW) {
        ++min_stat_state_.min_buf_cnt;
    }

    float Pnoise[MAX_FRAME_SIZE];
    for (int i = 0; i < frame_size; ++i) {
        float min_value = min_stat_state_.min_buffer[0][i];
        for (int j = 1; j < min_stat_state_.min_buf_cnt; ++j) {
            if (min_stat_state_.min_buffer[j][i] < min_value) {
                min_value = min_stat_state_.min_buffer[j][i];
            }
        }
        Pnoise[i] = min_value * BIAS_CORR;
    }

    complex_t S[MAX_FRAME_SIZE];
    for (int i = 0; i < frame_size; ++i) {
        float power = Y[i].real * Y[i].real + Y[i].imag * Y[i].imag;
        float gain = BETA_FLOOR;
        if (power > 0.0f) {
            gain = fmaxf(BETA_FLOOR, 1.0f - Pnoise[i] / power);
        }
        S[i].real = gain * Y[i].real;
        S[i].imag = gain * Y[i].imag;
    }

    ifft(S, frame_size);

    for (int i = 0; i < hop; ++i) {
        float output_sample = S[i].real + overlap_out_[i];
        out_buf[i] = static_cast<rtafe_sample_t>(output_sample);
        overlap_out_[i] = S[hop + i].real;
    }

    return kRtafeStatusOk;
}

 rtafe_module_status_t NoiseSuppress::SetParam(
    const rtafe_module_param_t *param)
{
    if (param == nullptr) {
        return kRtafeStatusInvalidArgument;
    }

    return kRtafeStatusUnsupported;
}

 rtafe_module_status_t NoiseSuppress::GetParam(
    rtafe_module_param_t *param) const
{
    if (param == nullptr) {
        return kRtafeStatusInvalidArgument;
    }

    return kRtafeStatusUnsupported;
}

 rtafe_module_status_t NoiseSuppress::SetProperties(
    const rtafe_module_properties_t *properties)
{
    if (properties == nullptr) {
        return kRtafeStatusInvalidArgument;
    }

    properties_ = *properties;
    return kRtafeStatusOk;
}

 rtafe_module_status_t NoiseSuppress::GetProperties(
    rtafe_module_properties_t *properties) const
{
    if (properties == nullptr) {
        return kRtafeStatusInvalidArgument;
    }

    *properties = properties_;
    return kRtafeStatusOk;
}

 rtafe_module_status_t NoiseSuppress::Reset()
{
    memset(overlap_in_, 0, sizeof(overlap_in_));
    memset(overlap_out_, 0, sizeof(overlap_out_));
    memset(&min_stat_state_, 0, sizeof(min_stat_state_));
    return kRtafeStatusOk;
}

