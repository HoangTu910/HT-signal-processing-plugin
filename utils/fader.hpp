#ifndef FADER_HPP
#define FADER_HPP

#include "common/rtafe_types.h"
#include "utils.hpp"
#include "errors_code.hpp"
#include "buffer_manager.hpp"

typedef struct FaderRampParams {
    u16 lin_slope; /* total ramp time in ms */
    u16 exp_slope; /* dB/s */
} fader_ramp_params_t;

typedef struct RampStepBuffer {
    float* buf;
    size_t size;
} ramp_buffer_t;

class Fader {
public:
    Fader(BufferManager &buffer_manager);
    ~Fader() = default;

    void                SetRampParams(fader_ramp_params_t params);
    const ramp_buffer_t GetRampBuffer(float cur_gain, float target_gain);

private:
    rtf_error_code_t CalculateRampStep();

    ramp_buffer_t ramp_buffer_{};
    fader_ramp_params_t ramp_params_{};
    float input_gain_ = 1.0f;
    float target_gain_ = 1.0f;
    ramp_buffer_t ramp_buffer_;

    BufferManager &buffer_manager_;
};

#endif /* FADER_HPP */