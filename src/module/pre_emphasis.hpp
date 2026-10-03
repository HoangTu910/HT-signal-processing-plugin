#ifndef PRE_EMPHASIS_HPP
#define PRE_EMPHASIS_HPP

#include "IRtafe_module.hpp"
#include "rtafe_params_id.h"

const u16 kPreEmphasisModuleParamsSize = 2;

struct PreEmState {
    float x[2];
    float y[2];
};

struct PreEmParams {
    float alpha;
    s16   alpha_fixed;
};

class PreEmphasis : public IRtafeModule {
public:
    PreEmphasis(float pre_emphasis_factor = 0.97f);
    ~PreEmphasis() override = default;

    rtafe_module_status_t Init(
        const rtafe_module_properties_t *properties) override;
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
    PreEmState state_{};
    rtafe_module_param_t param_{};
    tByte param_data_[kPreEmphasisModuleParamsSize]{};
    PreEmParams pre_em_params_{};
};

#endif /* PRE_EMPHASIS_HPP */