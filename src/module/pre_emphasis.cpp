#include "pre_emphasis.hpp"

rtafe_module_status_t PreEmphasis::Init(
    const rtafe_module_properties_t *properties)
{
    if (properties == nullptr) {
        return kRtafeStatusInvalidArgument;
    }

    properties_ = *properties;
    return Reset();
}

rtafe_module_status_t PreEmphasis::Process(
    const rtafe_process_buf_t *input,
    rtafe_process_buf_t *output)
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

    for (size_t i = 0; i < input->buf_size; ++i) {
        float in = static_cast<float>(in_buf[i]);
        float out = in - pre_em_params_.alpha * state_.x[0];

        state_.x[0] = in;
        out_buf[i] = static_cast<rtafe_sample_t>(out);
    }

    return kRtafeStatusOk;
}

 rtafe_module_status_t PreEmphasis::SetParam(
    const rtafe_module_param_t *param)
{
    if (param == nullptr || param->data == nullptr ||
        param->size != kPreEmphasisModuleParamsSize ||
        param->id != kRtafeParamIdPreEmphasis_Alpha) {
        return kRtafeStatusInvalidArgument;
    }

    const tByte *data = static_cast<const tByte *>(param->data);
    pre_em_params_.alpha_fixed = static_cast<s16>(static_cast<u16>(data[0]) |
                                    (static_cast<u16>(data[1]) << 8));
    pre_em_params_.alpha = Q8_8_TO_FLOAT(pre_em_params_.alpha_fixed);
    param_data_[0] = data[0];
    param_data_[1] = data[1];
    param_ = *param;
    param_.data = param_data_;
    return kRtafeStatusOk;
}

 rtafe_module_status_t PreEmphasis::GetParam(
    rtafe_module_param_t *param) const
{
    if (param == nullptr || param->id != kRtafeParamIdPreEmphasis_Alpha) {
        return kRtafeStatusInvalidArgument;
    }

    *param = param_;
    return kRtafeStatusOk;
}

 rtafe_module_status_t PreEmphasis::SetProperties(
    const rtafe_module_properties_t *properties)
{
    if (properties == nullptr) {
        return kRtafeStatusInvalidArgument;
    }

    properties_ = *properties;
    return kRtafeStatusOk;
}

 rtafe_module_status_t PreEmphasis::GetProperties(
    rtafe_module_properties_t *properties) const
{
    if (properties == nullptr) {
        return kRtafeStatusInvalidArgument;
    }

    *properties = properties_;
    return kRtafeStatusOk;
}

 rtafe_module_status_t PreEmphasis::Reset()
{
    state_ = {};
    return kRtafeStatusOk;
}