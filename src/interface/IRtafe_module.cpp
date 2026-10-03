#include "IRtafe_module.hpp"

rtafe_module_status_t IRtafeModule::GetStaticProperties(
    rtafe_module_properties_t *properties) const
{
    (void)properties;
    return kRtafeStatusUnsupported;
}

rtafe_module_status_t IRtafeModule::Init(
    const rtafe_module_properties_t *properties)
{
    return SetProperties(properties);
}

rtafe_module_status_t IRtafeModule::Process(
    const rtafe_process_buf_t *input,
    rtafe_process_buf_t *output)
{
    (void)input;
    (void)output;
    return kRtafeStatusUnsupported;
}

rtafe_module_status_t IRtafeModule::SetParam(
    const rtafe_module_param_t *param)
{
    (void)param;
    return kRtafeStatusUnsupported;
}

rtafe_module_status_t IRtafeModule::GetParam(
    rtafe_module_param_t *param) const
{
    (void)param;
    return kRtafeStatusUnsupported;
}

rtafe_module_status_t IRtafeModule::SetProperties(
    const rtafe_module_properties_t *properties)
{
    (void)properties;
    return kRtafeStatusUnsupported;
}

rtafe_module_status_t IRtafeModule::GetProperties(
    rtafe_module_properties_t *properties) const
{
    (void)properties;
    return kRtafeStatusUnsupported;
}

rtafe_module_status_t IRtafeModule::Reset()
{
    return kRtafeStatusUnsupported;
}

void IRtafeModule::End()
{
}