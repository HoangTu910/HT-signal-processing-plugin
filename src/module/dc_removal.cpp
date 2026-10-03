#include "dc_removal.hpp"
/* dc_removal.c */

/** Difference equation for Direct Form I
 * y[n] = x[n] - x[n - 1] + alpha * y[n - 1]
 */

rtafe_module_status_t DCRemoval::Process(const rtafe_process_buf_t *input,
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
        rtafe_sample_t in  = static_cast<float>(in_buf[i]);
        rtafe_sample_t out = in - state_.x[0] +
                    dc_params_.alpha * state_.y[0];

        state_.x[0] = in;
        state_.y[0] = out;
        out_buf[i] = static_cast<rtafe_sample_t>(out);
    }

    return kRtafeStatusOk;
}

rtafe_module_status_t DCRemoval::GetParam(rtafe_module_param_t *param) const
{
    if (param == nullptr) {
        return kRtafeStatusInvalidArgument;
    }
    if(param->id != kRtafeParamIdDCRemoval_Alpha) {
        return kRtafeStatusInvalidArgument;
    }
    *param = param_;

    return kRtafeStatusOk;
}

rtafe_module_status_t DCRemoval::Init(
    const rtafe_module_properties_t *properties)
{
    if (properties == nullptr) {
        return kRtafeStatusInvalidArgument;
    }

    properties_ = *properties;
    return Reset();
}

rtafe_module_status_t DCRemoval::SetProperties(const rtafe_module_properties_t *properties)
{
    if (properties == nullptr) {
        return kRtafeStatusInvalidArgument;
    }
    properties_ = *properties;

    return kRtafeStatusOk;
}

rtafe_module_status_t DCRemoval::GetStaticProperties(rtafe_module_properties_t *properties) const
{
    /*unused*/
    (void)properties;
    return kRtafeStatusUnsupported;
}

rtafe_module_status_t DCRemoval::GetProperties(
    rtafe_module_properties_t *properties) const
{
    if (properties == nullptr) {
        return kRtafeStatusInvalidArgument;
    }

    *properties = properties_;
    return kRtafeStatusOk;
}

rtafe_module_status_t DCRemoval::Reset()
{
    state_ = {};
    return kRtafeStatusOk;
}

rtafe_module_status_t DCRemoval::SetParam(const rtafe_module_param_t *param)
{
    if (param == nullptr || param->data == nullptr) {
        return kRtafeStatusInvalidArgument;
    }
    if (param->size != kDCRemovalModuleParamsSize) {
        return kRtafeStatusInvalidArgument;
    }
    if(param->id != kRtafeParamIdDCRemoval_Alpha) {
        return kRtafeStatusInvalidArgument;
    }
    const tByte *data = static_cast<const tByte *>(param->data);

    /*extract params from params payload*/
    u16 alpha_fixed = (u16)data[0] | ((u16)data[1] << 8);

    dc_params_.alpha_fixed = alpha_fixed;
    dc_params_.alpha       = Q8_8_TO_FLOAT(alpha_fixed);
    param_data_[0] = data[0];
    param_data_[1] = data[1];
    param_ = *param;
    param_.data = param_data_;

    return kRtafeStatusOk;
}
