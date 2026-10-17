#include "rtafe_plugin.hpp"

RTAFE_MainPlugin::RTAFE_MainPlugin(rtafe_plugin_config_t config)
{
    Init(config);
}

rtf_error_code_t RTAFE_MainPlugin::Init(const rtafe_plugin_config_t &config)
{
    config_ = config;
    buffer_manager_.InitMemoryPool(config.buffer_pool_config.block_size,
                                   config.buffer_pool_config.num_blocks);

    /**init more */
    module_[kRtafeDCRemoval]     = new DCRemoval();
    module_[kRtafePreEmphasis]   = new PreEmphasis();
    module_[kRtafeNoiseSuppress] = new NoiseSuppress();

    rtafe_module_properties_t properties{};
    properties.sample_rate     = config_.sampling_rate;
    properties.frame_samples   = config_.frame_samples;
    properties.channel_count   = config_.channel_count;
    properties.latency_samples = config_.hop_samples;
    properties.format          = config_.format;

    /*just use same properties for all modules*/
    module_[kRtafeDCRemoval]->Init(&properties);
    module_[kRtafePreEmphasis]->Init(&properties);
    module_[kRtafeNoiseSuppress]->Init(&properties);

    return kOk;
}

rtf_error_code_t RTAFE_MainPlugin::SetParam(rtafe_param_set_t param)
{
    switch(param.module_index) {
        case kRtafeDCRemoval: {
            const rtafe_module_param_t module_param{
                param.param_id, param.param_payload, param.size};
            if (module_[kRtafeDCRemoval]->SetParam(&module_param) !=
                kRtafeStatusOk) {
                return kErrorInvalidModuleParam;
            }
            break;
        }
        case kRtafePreEmphasis: {
            const rtafe_module_param_t module_param{
                param.param_id, param.param_payload, param.size};
            if (module_[kRtafePreEmphasis]->SetParam(&module_param) !=
                kRtafeStatusOk) {
                return kErrorInvalidModuleParam;
            }
            break;
        }
        case kRtafeNoiseSuppress:
            return kErrorInvalidModuleParam;
        default:
            return kErrorInvalidModuleParam;
    }
    return kOk;
}

rtf_error_code_t RTAFE_MainPlugin::SetProcessFrame(rtafe_process_frame_t in_frame)
{
    process_frame_ = in_frame;
    return kOk;
}

rtf_error_code_t RTAFE_MainPlugin::Process()
{
    rtafe_process_buf_t input_buf{};
    rtafe_process_buf_t output_buf{};

    input_buf.buffer   = process_frame_.data[0];
    input_buf.buf_size = config_.frame_samples;

    output_buf.buffer = process_frame_.data[0];
    output_buf.buf_size = config_.frame_samples;

    for (u16 ch = 0; ch < config_.channel_count; ++ch) {
        input_buf.buffer  = process_frame_.data[ch];
        output_buf.buffer = process_frame_.data[ch];

        for (u16 modIdx = 0; modIdx < kRtafeMaxModule; ++modIdx) {
            if (module_[modIdx]) {
                if (module_[modIdx]->Process(&input_buf, &output_buf) !=
                    kRtafeStatusOk) {
                    return kErrorProcessing;
                }
                input_buf.buffer = output_buf.buffer;
            }
        }
    }

    return kOk;
}

rtf_error_code_t RTAFE_MainPlugin::ProcessFixed()
{
    return Process();
}
