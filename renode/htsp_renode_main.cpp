#include "htsp_plugin.hpp"

#include <new>
#include <stdint.h>

struct RenodeResult {
    uint32_t magic;
    uint32_t status;
    int32_t left_checksum;
    int32_t right_checksum;
    int32_t left_first;
    int32_t right_first;
};

volatile RenodeResult renode_result
    __attribute__((section(".results"), used)) = {};

static uint32_t plugin_storage[(sizeof(HTSPPlugin) + sizeof(uint32_t) - 1) /
                               sizeof(uint32_t)];
static sample_t left_input[BLOCK_SIZE];
static sample_t right_input[BLOCK_SIZE];
static sample_t *input_channels[2] = {left_input, right_input};

void operator delete(void *pointer) noexcept
{
    (void)pointer;
}

void operator delete(void *pointer, unsigned int size) noexcept
{
    (void)pointer;
    (void)size;
}

void operator delete[](void *pointer) noexcept
{
    (void)pointer;
}

void operator delete[](void *pointer, unsigned int size) noexcept
{
    (void)pointer;
    (void)size;
}

extern "C" void *memset(void *destination, int value, unsigned long size)
{
    uint8_t *bytes = static_cast<uint8_t *>(destination);
    for (unsigned long index = 0; index < size; ++index) {
        bytes[index] = static_cast<uint8_t>(value);
    }
    return destination;
}

extern "C" void *memcpy(void *destination, const void *source, unsigned long size)
{
    uint8_t *destination_bytes = static_cast<uint8_t *>(destination);
    const uint8_t *source_bytes = static_cast<const uint8_t *>(source);
    for (unsigned long index = 0; index < size; ++index) {
        destination_bytes[index] = source_bytes[index];
    }
    return destination;
}

static int32_t checksum(const sample_t *samples)
{
    int32_t sum = 0;
    for (uint16_t index = 0; index < BLOCK_SIZE; ++index) {
        sum += samples[index];
    }
    return sum;
}

extern "C" int main()
{
    for (uint16_t index = 0; index < BLOCK_SIZE; ++index) {
        left_input[index] = static_cast<sample_t>((index * 97) - 12000);
        right_input[index] = static_cast<sample_t>(12000 - (index * 53));
    }

    HTSPPlugin *plugin = new (plugin_storage) HTSPPlugin();
    HtspErrRet status = plugin->SetParams();
    if (status == kOk) {
        status = plugin->ProcessFixed(input_channels, 2);
    }

    renode_result.magic = 0x52544146U;
    renode_result.status = static_cast<uint32_t>(status);
    renode_result.left_checksum = checksum(left_input);
    renode_result.right_checksum = checksum(right_input);
    renode_result.left_first = left_input[0];
    renode_result.right_first = right_input[0];

    for (;;) {
        __asm volatile("wfi");
    }
}
