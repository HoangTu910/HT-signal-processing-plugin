#include "fader.hpp"

#include <algorithm>
#include <cmath>

Fader::Fader(BufferManager &buffer_manager)
    : buffer_manager_(buffer_manager), ramp_buffer_{nullptr, 0}
{
    ramp_params_.lin_slope = 100;
    ramp_params_.exp_slope = 0;
    input_gain_  = 1.0f;
    target_gain_ = 1.0f;
    CalculateRampStep();
}

void Fader::SetRampParams(fader_ramp_params_t params)
{
    ramp_params_ = params;
    CalculateRampStep();
}

const ramp_buffer_t Fader::GetRampBuffer(float cur_gain, float target_gain)
{
    input_gain_  = cur_gain;
    target_gain_ = target_gain;
    CalculateRampStep();
}

rtf_error_code_t Fader::CalculateRampStep()
{
    u16   ramp_samples = ramp_params_.lin_slope / 1000.0f * SAMPLING_RATE;
    float gain_step    = (target_gain_ - input_gain_) / static_cast<float>(ramp_samples);
    
    ramp_buffer_.size = ramp_samples;
    ramp_buffer_.buf  = static_cast<float*>(buffer_manager_.Alloc());
    if (!ramp_buffer_.buf) {
        return kErrorMemoryAllocation;
    }

    float current_gain = input_gain_;
    for (u16 i = 0; i < ramp_samples; ++i) {
        ramp_buffer_.buf[i] = current_gain;
        current_gain += gain_step;
    }

    /*TODO: Apply gain smoothing factor*/
    return kOk;
}
