#ifndef RTAFE_MODULE_CONTRACT_H
#define RTAFE_MODULE_CONTRACT_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Stable statuses for the RTAFE boundary. The AudioReach adapter maps these
 * values to the SDK's capi_v2_err_t values. */
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

/* One logical input or output port. Planar buffers are used deliberately so
 * the same contract can map to AudioReach stream buffers. */
typedef struct rtafe_stream {
    void **buffers;
    uint32_t buffer_count;
    uint32_t samples_per_buffer;
    uint32_t bytes_per_sample;
    rtafe_sample_format_t format;
} rtafe_stream_t;

typedef struct rtafe_module_properties {
    uint32_t sample_rate;
    uint32_t frame_samples;
    uint32_t channel_count;
    uint32_t latency_samples;
    rtafe_sample_format_t format;
} rtafe_module_properties_t;

typedef struct rtafe_module_param {
    uint32_t id;
    const void *data;
    size_t size;
} rtafe_module_param_t;

typedef struct rtafe_module rtafe_module_t;

typedef struct rtafe_module_vtable {
    rtafe_module_status_t (*get_static_properties)(
        rtafe_module_properties_t *properties);
    rtafe_module_status_t (*init)(
        rtafe_module_t *module,
        const rtafe_module_properties_t *properties,
        void *state,
        size_t state_size);
    rtafe_module_status_t (*process)(
        rtafe_module_t *module,
        const rtafe_stream_t *input,
        rtafe_stream_t *output);
    rtafe_module_status_t (*set_param)(
        rtafe_module_t *module,
        const rtafe_module_param_t *param);
    rtafe_module_status_t (*get_param)(
        const rtafe_module_t *module,
        rtafe_module_param_t *param);
    rtafe_module_status_t (*set_properties)(
        rtafe_module_t *module,
        const rtafe_module_properties_t *properties);
    rtafe_module_status_t (*get_properties)(
        const rtafe_module_t *module,
        rtafe_module_properties_t *properties);
    rtafe_module_status_t (*reset)(rtafe_module_t *module);
    void (*end)(rtafe_module_t *module);
} rtafe_module_vtable_t;

struct rtafe_module {
    const rtafe_module_vtable_t *vtable;
    void *context;
};

#ifdef __cplusplus
}

/* C++ module contract used by the internal DSP implementation. The C structs
 * above remain the stable boundary for the future AudioReach adapter. */
class IRtafeModule {
public:
    virtual ~IRtafeModule() = default;

    virtual rtafe_module_status_t GetStaticProperties(
        rtafe_module_properties_t *properties) const = 0;
    virtual rtafe_module_status_t Init(
        const rtafe_module_properties_t *properties,
        void *state,
        size_t state_size) = 0;
    virtual rtafe_module_status_t Process(
        const rtafe_stream_t *input,
        rtafe_stream_t *output) = 0;
    virtual rtafe_module_status_t SetParam(
        const rtafe_module_param_t *param) = 0;
    virtual rtafe_module_status_t GetParam(
        rtafe_module_param_t *param) const = 0;
    virtual rtafe_module_status_t SetProperties(
        const rtafe_module_properties_t *properties) = 0;
    virtual rtafe_module_status_t GetProperties(
        rtafe_module_properties_t *properties) const = 0;
    virtual rtafe_module_status_t Reset() = 0;
    virtual void End() = 0;
};
#endif

#endif /* RTAFE_MODULE_CONTRACT_H */
