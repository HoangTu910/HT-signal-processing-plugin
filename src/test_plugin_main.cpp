#include "wav_file_mgr.hpp"
#include "rtafe_plugin.hpp"
#include "utils.hpp"

#include <cstdio>

static bool processWavFile(const char *inputFile, const char *outputFile)
{
    const u16 frameSize = BLOCK_SIZE;

    WavFileMgr wav;
    if (!wav.bOpenRead(inputFile, frameSize)) {
        std::printf("Failed to open input WAV file\n");
        return false;
    }

    const u16 numChannels = wav.uGetChannels();
    if (numChannels != 2 && numChannels != 4) {
        std::printf("Unsupported channel count: %u; plugin requires 2 or 4\n",
                    numChannels);
        wav.vCloseRead();
        return false;
    }

    if (!wav.bOpenWrite(outputFile)) {
        std::printf("Failed to open output WAV file\n");
        wav.vCloseRead();
        return false;
    }

    buffer_pool_config_t buffer_pool_config{};
    buffer_pool_config.block_size = 768;
    buffer_pool_config.num_blocks = 128;

    rtafe_plugin_config_t config{};
    config.sampling_rate      = wav.uGetSampleRate();
    config.frame_samples      = frameSize;
    config.channel_count      = numChannels;
    config.hop_samples        = frameSize; // no overlap
    config.format             = kRtafeSampleQ15; // or kRtafeSample
    config.buffer_pool_config = buffer_pool_config;

    RTAFE_MainPlugin rtafe_plugin(config);

    while (wav.bHasNextFrame()) {
        sample_t **channels = wav.ppReadFrame();
        if (!channels) {
            std::printf("Failed to read WAV frame\n");
            wav.vCloseWrite();
            wav.vCloseRead();
            return false;
        }
        
        rtf_error_code_t ret;

        #ifdef FIXED_POINT
        ret = rtafe_plugin.ProcessFixed(channels, numChannels);
        #else
        ret = rtafe_plugin.Process(channels, numChannels);
        #endif
        if (ret != kOk) {
            std::printf("DSP processing error: %d\n", ret);
            wav.vCloseWrite();
            wav.vCloseRead();
            return false;
        }

        wav.vWriteFrame(channels, frameSize);
    }

    wav.vCloseWrite();
    wav.vCloseRead();
    return true;
}

int main(int argc, char *argv[])
{
    if (argc < 3) {
        std::printf("Usage: %s input.wav output.wav\n", argv[0]);
        return 1;
    }

    return processWavFile(argv[1], argv[2]) ? 0 : 1;
}