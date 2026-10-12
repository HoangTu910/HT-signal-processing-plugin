#ifndef RTAFE_PLUGIN_HPP
#define RTAFE_PLUGIN_HPP

#include "IRtafe_module.hpp"
#include "common/rtafe_types.h"

typedef struct BufferPoolConfig {
    u16   block_size;
    u16   num_blocks;
} buffer_pool_config_t;

typedef struct PluginConfig {
    buffer_pool_config_t  buffer_pool_config;
    u32                   sampling_rate;
    u32                   frame_samples;
    u32                   channel_count;
    u32                   hop_samples;
    rtafe_sample_format_t format;
} rtafe_plugin_config_t;

struct ChannelContext {
    ChannelId channel_id;
    u16       module_count;
};

class RTAFE_MainPlugin {
public:
    RTAFE_MainPlugin(rtafe_plugin_config_t config);
    ~RTAFE_MainPlugin();

    rtf_error_code_t Init(const rtafe_plugin_config_t& config);
    rtf_error_code_t SetParam(ChannelId ch, u16 module_index, u32 param_id,
                        const void* data, size_t size);
    rtf_error_code_t Process(sample_t** in_buf, u16 num_channels);
    rtf_error_code_t ProcessFixed(sample_t** in_buf, u16 num_channels);
    rtf_error_code_t Reset();
    void Destroy();

private:
    rtafe_plugin_config_t config_{};
    uint16_t      channel_count_{0};
    BufferManager buffer_manager_;
};

#endif /* RTAFE_PLUGIN_HPP */