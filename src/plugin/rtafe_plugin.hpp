#ifndef RTAFE_PLUGIN_HPP
#define RTAFE_PLUGIN_HPP

#include <cstddef>
#include <cstdint>

#include "IRtafe_module.hpp"
#include "common/rtafe_types.h"
#include "common/rtafe_params_id.h"
#include "buffer/buffer_manager.hpp"
#include "module/dc_removal.hpp"
#include "module/pre_emphasis.hpp"
#include "module/noise_suppress.hpp"

/* Keep the public header usable by both static and shared-library consumers. */
#if defined(_WIN32) && defined(RTAFE_PLUGIN_BUILD_SHARED)
#  if defined(RTAFE_PLUGIN_EXPORTS)
#    define RTAFE_PLUGIN_API __declspec(dllexport)
#  else
#    define RTAFE_PLUGIN_API __declspec(dllimport)
#  endif
#elif defined(__GNUC__) || defined(__clang__)
#  define RTAFE_PLUGIN_API __attribute__((visibility("default")))
#else
#  define RTAFE_PLUGIN_API
#endif

constexpr u16 kRtafePluginMaxChannels = 4;

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

typedef struct ParamConfig {
    u16   module_index;
    u32   param_id;
    const void *param_payload;
    size_t size;
} rtafe_param_set_t;

typedef struct ProcessFrame {
    sample_t** data;
    u16        num_channels;
} rtafe_process_frame_t;

struct ChannelContext {
    ChannelId channel_id;
    u16       module_count;
};

class RTAFE_PLUGIN_API RTAFE_MainPlugin {
public:
    explicit RTAFE_MainPlugin(rtafe_plugin_config_t config);
    ~RTAFE_MainPlugin();

    RTAFE_MainPlugin(const RTAFE_MainPlugin&) = delete;
    RTAFE_MainPlugin& operator=(const RTAFE_MainPlugin&) = delete;
    RTAFE_MainPlugin(RTAFE_MainPlugin&&) = delete;
    RTAFE_MainPlugin& operator=(RTAFE_MainPlugin&&) = delete;

    /**
     * Validate a configuration before constructing or initializing a plugin.
     *
     * This only checks the public contract. Module-specific constraints are
     * reported by Init() and the processing methods.
     */
    static bool IsValidConfig(const rtafe_plugin_config_t& config) noexcept
    {
        const bool valid_format =
            config.format == kRtafeSampleQ15 ||
            config.format == kRtafeSampleQ31 ||
            config.format == kRtafeSampleQ8_8 ||
            config.format == kRtafeSampleFloat32;

        return config.buffer_pool_config.block_size != 0 &&
               config.buffer_pool_config.num_blocks != 0 &&
               config.sampling_rate != 0 &&
               config.frame_samples != 0 &&
               config.hop_samples != 0 &&
               config.hop_samples <= config.frame_samples &&
               config.channel_count != 0 &&
               config.channel_count <= kRtafePluginMaxChannels &&
               valid_format;
    }

    /** Initialize or reconfigure the plugin. */
    rtf_error_code_t Init(const rtafe_plugin_config_t& config);

    /** Return the active configuration without transferring ownership. */
    const rtafe_plugin_config_t& GetConfig() const noexcept { return config_; }

    /** Return the number of configured input channels. */
    u32 GetChannelCount() const noexcept { return config_.channel_count; }

    /** Return the number of samples in each frame. */
    u32 GetFrameSamples() const noexcept { return config_.frame_samples; }

    /** Return the number of samples in each frame. */
    u32 GetHopSamples() const noexcept { return config_.hop_samples; }

    rtf_error_code_t SetParam(rtafe_param_set_t param);
    rtf_error_code_t SetProcessFrame(rtafe_process_frame_t in_frame);
    rtf_error_code_t Process();
    rtf_error_code_t ProcessFixed();
    rtf_error_code_t Reset();
    void Destroy();

private:
    rtafe_plugin_config_t config_{};
    IRtafeModule* module_[kRtafeMaxModule];
    BufferManager buffer_manager_;
    rtafe_process_frame_t process_frame_;
};

#endif /* RTAFE_PLUGIN_HPP */