#include "IRtafe_module.hpp"

rtafe_module_status_t IRtafeModule::GetStaticProperties(
    rtafe_module_properties_t *properties) const
{
    if (properties == nullptr) {
        return RTAFE_STATUS_INVALID_ARGUMENT;
    }
#ifdef FIXED_POINT
    properties->format = RTAFE_SAMPLE_Q31;
#else
    properties->format = RTAFE_SAMPLE_FLOAT32;
#endif
    return RTAFE_STATUS_OK;
}

rtafe_module_status_t IRtafeModule::Init(
    const rtafe_module_properties_t *properties,
    void *state,
    size_t state_size)
{
    (void)state;
    (void)state_size;
    return SetProperties(properties);
}

rtafe_module_status_t IRtafeModule::Process(
    const rtafe_stream_t *input,
    rtafe_stream_t *output)
{
    if (input == nullptr || output == nullptr || input->buffers == nullptr ||
        output->buffers == nullptr || input->buffer_count != 1 ||
        output->buffer_count != 1 || input->buffers[0] == nullptr ||
        output->buffers[0] == nullptr || input->buffers[0] != output->buffers[0] ||
        input->samples_per_buffer != BLOCK_SIZE ||
        output->samples_per_buffer != BLOCK_SIZE ||
        input->bytes_per_sample != sizeof(sample_t) ||
        output->bytes_per_sample != sizeof(sample_t) ||
        input->format != output->format) {
        return RTAFE_STATUS_INVALID_BUFFER;
    }
    return RTAFE_STATUS_OK;
}

rtafe_module_status_t IRtafeModule::SetParam(
    const rtafe_module_param_t *param)
{
    if (param == nullptr || param->data == nullptr ||
        param->size != sizeof(DSPModuleParams)) {
        return RTAFE_STATUS_INVALID_ARGUMENT;
    }
    return RTAFE_STATUS_UNSUPPORTED;
}

rtafe_module_status_t IRtafeModule::GetParam(
    rtafe_module_param_t *param) const
{
    (void)param;
    return RTAFE_STATUS_UNSUPPORTED;
}

rtafe_module_status_t IRtafeModule::SetProperties(
    const rtafe_module_properties_t *properties)
{
    if (properties == nullptr || properties->frame_samples != BLOCK_SIZE ||
        properties->channel_count != 1) {
        return RTAFE_STATUS_INVALID_ARGUMENT;
    }

    properties_ = *properties;
    return RTAFE_STATUS_OK;
}

rtafe_module_status_t IRtafeModule::GetProperties(
    rtafe_module_properties_t *properties) const
{
    if (properties == nullptr) return RTAFE_STATUS_INVALID_ARGUMENT;
    *properties = properties_;
    return RTAFE_STATUS_OK;
}

rtafe_module_status_t IRtafeModule::Reset()
{
    return RTAFE_STATUS_UNSUPPORTED;
}

void IRtafeModule::End()
{
}