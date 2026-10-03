#ifndef DC_REMOVAL_HPP
#define DC_REMOVAL_HPP

#include "IRtafe_module.hpp"
#include "utils.hpp"
#include "dsp_block.hpp"
#include "errors_code.hpp"
#include "rtafe_params_id.h"

/** Direct form I from Richard Lyons */
/** Difference equation for Direct Form I
 * y[n] = x[n] - x[n - 1] + alpha * y[n - 1]
 */
const u16 kDCRemovalModuleParamsSize = 2; /*2 bytes*/

typedef struct {
    float x[2];
    float y[2];
} DCRemovalState;

typedef struct {
    float alpha;
    u16   alpha_fixed;
} DCRemovalParams;

class DCRemoval : public IRtafeModule {
public:
    rtafe_module_status_t GetStaticProperties(
        rtafe_module_properties_t *properties) const override;

    rtafe_module_status_t Init(
        const rtafe_module_properties_t *properties) override;

    rtafe_module_status_t SetProperties(
        const rtafe_module_properties_t *properties) override;

    rtafe_module_status_t SetParam(
        const rtafe_module_param_t *param) override;

    rtafe_module_status_t Process(
        const rtafe_process_buf_t *input,
              rtafe_process_buf_t *output) override;

    rtafe_module_status_t GetParam(
        rtafe_module_param_t *param) const override;
    rtafe_module_status_t GetProperties(
        rtafe_module_properties_t *properties) const override;
    rtafe_module_status_t Reset() override;

private:
    rtafe_module_properties_t properties_{};
    rtafe_module_param_t      param_{};
    tByte                     param_data_[kDCRemovalModuleParamsSize]{};
    DCRemovalState            state_{};
    DCRemovalParams           dc_params_{};
};

#endif /* DC_REMOVAL_HPP */