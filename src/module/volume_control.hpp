#ifndef VOLUME_CONTROL_HPP
#define VOLUME_CONTROL_HPP

#include "IRtafe_module.hpp"
#include "common/rtafe_params_id.h"
#include "fader.hpp"

const u16 kVolumeControlModuleParamsSize = 6; /*6 bytes*/
// | Gain(2 bytes Q8.8) | LinearRamp(2 bytes) | ExpRamp(2 byte) |

struct VolumeControlParams {
    float gain;
    s16   gain_fixed;
    u16   linear_ramp;
    u16   exp_ramp;
};

class VolumeControl : public IRtafeModule {
public:
    rtafe_module_status_t Init(
        const rtafe_module_properties_t *properties,
        const rtafe_module_utils_t *utils) override;
    rtafe_module_status_t Process(
        const rtafe_process_buf_t *input,
        rtafe_process_buf_t *output) override;
    rtafe_module_status_t SetParam(
        const rtafe_module_param_t *param) override;
    rtafe_module_status_t GetParam(
        rtafe_module_param_t *param) const override;
    rtafe_module_status_t SetProperties(
        const rtafe_module_properties_t *properties) override;
    rtafe_module_status_t GetProperties(
        rtafe_module_properties_t *properties) const override;
    rtafe_module_status_t Reset() override;

private:
    VolumeControlParams       volume_control_params_{};
    rtafe_module_param_t      param_{};
    tByte                     param_data_[kVolumeControlModuleParamsSize]{};
    rtafe_module_properties_t properties_{};
    rtafe_module_utils_t      utils_{};
};

#endif /* VOLUME_CONTROL_HPP */