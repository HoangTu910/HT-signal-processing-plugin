#include "rtafe_plugin.hpp"

rtf_error_code_t RTAFE_MainPlugin::Init(const rtafe_plugin_config_t &config)
{
    buffer_manager_.InitMemoryPool(config.buffer_pool_config.block_size,
                                   config.buffer_pool_config.num_blocks);

    /**init more */
}
