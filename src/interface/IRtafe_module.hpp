#ifndef IRTAFE_MODULE_HPP
#define IRTAFE_MODULE_HPP

#include "utils.hpp"
#include "dsp_block.hpp"
#include "errors_code.hpp"

typedef enum rtafe_module_status {
    kRtafeStatusOk = 0,
    kRtafeStatusInvalidArgument,
    kRtafeStatusOutOfMemory,
    kRtafeStatusUnsupported,
    kRtafeStatusInvalidState,
    kRtafeStatusInvalidBuffer
} rtafe_module_status_t;

typedef enum rtafe_sample_format {
    kRtafeSampleQ15 = 0,
    kRtafeSampleQ31,
    kRtafeSampleQ8_8,
    kRtafeSampleFloat32
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
    uint32_t              frame_samples; /** Number of samples in each frame */
    uint32_t              channel_count;
    uint32_t              latency_samples;
    rtafe_sample_format_t format;
} rtafe_module_properties_t;

/* parameter is an array of parameters, each element is one byte */
typedef struct rtafe_module_param {
    uint32_t    id;
    const void *data;
    size_t      size;
} rtafe_module_param_t;

typedef struct rtafe_process_buf {
    void*  buffer;
    size_t buf_size;
} rtafe_process_buf_t;

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
        const rtafe_module_properties_t *properties);

    /**
     * Process the input stream and produce the output stream.
     * @param input The input stream to process.
     * @param output The output stream to produce.
     * @return The status of the operation.
     */
    virtual rtafe_module_status_t Process(
        const rtafe_process_buf_t *input,
              rtafe_process_buf_t *output);

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

    rtafe_module_properties_t properties_{};
};

#endif /* IRTAFE_MODULE_HPP */