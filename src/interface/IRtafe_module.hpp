#ifndef IRTAFE_MODULE_HPP
#define IRTAFE_MODULE_HPP

#include "utils.hpp"
#include "dsp_block.hpp"
#include "errors_code.hpp"

typedef enum rtafe_module_status {
    RTAFE_STATUS_OK = 0,
    RTAFE_STATUS_INVALID_ARGUMENT = 1,
    RTAFE_STATUS_OUT_OF_MEMORY = 2,
    RTAFE_STATUS_UNSUPPORTED = 3,
    RTAFE_STATUS_INVALID_STATE = 4,
    RTAFE_STATUS_INVALID_BUFFER = 5
} rtafe_module_status_t;

typedef enum rtafe_sample_format {
    RTAFE_SAMPLE_Q15 = 0,
    RTAFE_SAMPLE_Q31 = 1,
    RTAFE_SAMPLE_FLOAT32 = 2
} rtafe_sample_format_t;

typedef struct rtafe_stream {
    void**                buffers;
    uint32_t              buffer_count;
    uint32_t              samples_per_buffer;
    uint32_t              bytes_per_sample;
    rtafe_sample_format_t format;
} rtafe_stream_t;

typedef struct rtafe_module_properties {
    uint32_t              sample_rate;
    uint32_t              frame_samples;
    uint32_t              channel_count;
    uint32_t              latency_samples;
    rtafe_sample_format_t format;
} rtafe_module_properties_t;

typedef struct rtafe_module_param {
    uint32_t    id;
    const void *data;
    size_t      size;
} rtafe_module_param_t;

typedef struct rtafe_module rtafe_module_t;

class IRtafeModule {
public:
    virtual ~IRtafeModule() = default;

    /** Get the static properties of the module.
     * @param properties The properties to fill.
     * @return The status of the operation.
     */
    virtual rtafe_module_status_t GetStaticProperties(
        rtafe_module_properties_t *properties) const;

    /**
     * Initialize the module with the given properties and state.
     * @param properties The properties to initialize the module with.
     * @param state The state to initialize the module with.
     * @param state_size The size of the state.
     * @return The status of the operation.
     */
    virtual rtafe_module_status_t Init(
        const rtafe_module_properties_t *properties,
        void *state,
        size_t state_size);

    /**
     * Process the input stream and produce the output stream.
     * @param input The input stream to process.
     * @param output The output stream to produce.
     * @return The status of the operation.
     */
    virtual rtafe_module_status_t Process(
        const rtafe_stream_t *input,
        rtafe_stream_t *output);

    /**
     * Set a parameter of the module.
     * @param param The parameter to set.
     * @return The status of the operation.
     */
    virtual rtafe_module_status_t SetParam(
        const rtafe_module_param_t *param);
    
    /**
     * Get a parameter of the module.
     * @param param The parameter to get.
     * @return The status of the operation.
     */
    virtual rtafe_module_status_t GetParam(
        rtafe_module_param_t *param) const;

    /**
     * Set the properties of the module.
     * @param properties The properties to set.
     * @return The status of the operation.
     */
    virtual rtafe_module_status_t SetProperties(
        const rtafe_module_properties_t *properties);

    /**
     * Get the properties of the module.
     * @param properties The properties to get.
     * @return The status of the operation.
     */
    virtual rtafe_module_status_t GetProperties(
        rtafe_module_properties_t *properties) const;

    /**
     * Reset the module to its initial state.
     * @return The status of the operation.
     */
        virtual rtafe_module_status_t Reset();

    /**
     * End the module and release any resources.
     */
        virtual void End();

    virtual HtspErrRet SetParams(const DSPModuleParams *params,
                                 u16 param_count) = 0;
    virtual void ProcessBlock(DSPBlock *dsp_block) = 0;
    virtual void ProcessBlockFixed(DSPBlock *dsp_block) = 0;

    rtafe_module_properties_t properties_{};
};

#endif /* IRTAFE_MODULE_HPP */