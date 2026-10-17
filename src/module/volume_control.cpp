#include "volume_control.hpp"

rtafe_module_status_t VolumeControl::Init(const rtafe_module_properties_t *properties,
                                          const rtafe_module_utils_t *utils)
{
    if (properties == nullptr) {
        return kRtafeStatusInvalidArgument;
    }
    if (utils == nullptr) {
        return kRtafeStatusInvalidArgument;
    }

    properties_ = *properties;
    utils_      = *utils;

    return kRtafeStatusOk;
}

rtafe_module_status_t VolumeControl::Process(const rtafe_process_buf_t *input, rtafe_process_buf_t *output)
{
    if (input == nullptr || output == nullptr || input->buffer == nullptr ||
        output->buffer == nullptr || input->buf_size != output->buf_size ||
        input->buf_size != properties_.frame_samples) {
        return kRtafeStatusInvalidBuffer;
    }

    rtafe_sample_t *in_buf =
        static_cast<rtafe_sample_t *>(input->buffer);
    rtafe_sample_t *out_buf =
        static_cast<rtafe_sample_t *>(output->buffer);

    float cur_gain = in_buf[0];

    utils_.fader->SetRampParams({volume_control_params_.linear_ramp,
                                 volume_control_params_.exp_ramp});
    ramp_buffer_t ramp_buf = utils_.fader->GetRampBuffer(cur_gain, volume_control_params_.gain);

    /*TODO: Cần xử lý ramp buffer, cần phải lưu state gì đó, nếu chưa ramp xong
    thì frame sau phải xử lý tiếp, cần setter để set fader state*/
    for (size_t i = 0; i < input->buf_size; ++i) {
        float in = static_cast<float>(in_buf[i]);
        float out = in * volume_control_params_.gain;

        out_buf[i] = static_cast<rtafe_sample_t>(out);
    }

    return kRtafeStatusOk;
}

rtafe_module_status_t VolumeControl::SetParam(const rtafe_module_param_t *param)
{
    if (param == nullptr || param->data == nullptr) {
        return kRtafeStatusInvalidArgument;
    }
    if (param->size != kVolumeControlModuleParamsSize) {
        return kRtafeStatusInvalidArgument;
    }
    if(param->id != kRtafeParamIdVolumeControl_Gain) {
        return kRtafeStatusInvalidArgument;
    }
    const tByte *data = static_cast<const tByte *>(param->data);

    /*extract params from params payload*/
    volume_control_params_.gain_fixed  = (u16)data[0] | ((u16)data[1] << 8);
    volume_control_params_.gain        = Q8_8_TO_FLOAT(volume_control_params_.gain_fixed);
    volume_control_params_.linear_ramp = MAKE_U16(data[2], data[3]);
    volume_control_params_.exp_ramp    = MAKE_U16(data[4], data[5]);

    /*maybe we not need this...*/
    param_data_[0] = data[0];
    param_data_[1] = data[1];
    param_data_[2] = data[2];
    param_data_[3] = data[3];
    param_data_[4] = data[4];
    param_data_[5] = data[5];
    param_         = *param;
    param_.data    = param_data_;

    FE_LOG("\nVolumeControl::SetParam: gain = %f ramp = %u, exp_ramp = %u\n",
           volume_control_params_.gain,
           volume_control_params_.linear_ramp,
           volume_control_params_.exp_ramp);

    return kRtafeStatusOk;
}
