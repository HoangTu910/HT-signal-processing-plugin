#include <cstdio>
#include <cctype>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include "wav_file_mgr.hpp"
#include "rtafe_plugin.hpp"
#include "utils.hpp"

struct ParsedParam {
    rtafe_param_set_t param{};
    std::vector<uint8_t> payload;
};

struct FrameConfig {
    uint32_t start_frame;
    std::string filename;
    std::vector<ParsedParam> params;
};

static bool ParseModuleName(const std::string& name,
                            u16& module_index,
                            u32& param_id)
{
    if (name == "kRtafeDCRemoval") {
        module_index = kRtafeDCRemoval;
        param_id = kRtafeParamIdDCRemoval_Alpha;
        return true;
    }

    if (name == "kRtafePreEmphasis") {
        module_index = kRtafePreEmphasis;
        param_id = kRtafeParamIdPreEmphasis_Alpha;
        return true;
    }

    if (name == "kRtafeNoiseSuppress") {
        module_index = kRtafeNoiseSuppress;
        param_id = kRtafeParamIdNoiseSuppress_Alpha;
        return true;
    }

    return false;
}

static bool EncodeQ8_8(float value, std::vector<uint8_t>& payload)
{
    if (value < -128.0f || value > 127.99609375f) {
        return false;
    }

    const int32_t raw =
        static_cast<int32_t>(value * 256.0f + (value >= 0.0f ? 0.5f : -0.5f));

    if (raw < INT16_MIN || raw > INT16_MAX) {
        return false;
    }

    const uint16_t encoded = static_cast<uint16_t>(
        static_cast<int16_t>(raw));

    payload.resize(2);
    payload[0] = static_cast<uint8_t>(encoded & 0xFFu);
    payload[1] = static_cast<uint8_t>((encoded >> 8) & 0xFFu);
    return true;
}

static bool ParseParameterFile(const std::string& filename,
                               std::vector<ParsedParam>& params)
{
    FILE* file = std::fopen(filename.c_str(), "r");
    if (file == nullptr) {
        std::printf("Failed to open parameter file: %s\n", filename.c_str());
        return false;
    }

    char line[256]{};
    u16 current_module = 0;
    u32 current_param_id = 0;
    bool module_selected = false;

    while (std::fgets(line, sizeof(line), file) != nullptr) {
        std::string text(line);

        const std::size_t comment = text.find('#');
        if (comment != std::string::npos) {
            text.erase(comment);
        }

        while (!text.empty() &&
               std::isspace(static_cast<unsigned char>(text.back()))) {
            text.pop_back();
        }

        std::size_t first = 0;
        while (first < text.size() &&
               std::isspace(static_cast<unsigned char>(text[first]))) {
            ++first;
        }

        text = text.substr(first);

        if (text.empty()) {
            continue;
        }

        if (text[0] == ':') {
            const std::size_t end = text.find_first_of(" \t\r\n");
            const std::string module_name =
                text.substr(1, end == std::string::npos
                                   ? std::string::npos
                                   : end - 1);

            module_selected = ParseModuleName(
                module_name, current_module, current_param_id);

            if (!module_selected) {
                std::printf("Unknown module '%s' in %s\n",
                            module_name.c_str(), filename.c_str());
                std::fclose(file);
                return false;
            }

            continue;
        }

        if (!module_selected) {
            std::printf("Parameter appears before module in %s\n",
                        filename.c_str());
            std::fclose(file);
            return false;
        }

        const std::size_t equal = text.find('=');
        if (equal == std::string::npos) {
            continue;
        }

        const std::string key = text.substr(0, equal);
        if (key.find("alpha") == std::string::npos) {
            std::printf("Unsupported parameter '%s' in %s\n",
                        key.c_str(), filename.c_str());
            std::fclose(file);
            return false;
        }

        char* end_ptr = nullptr;
        const float value = std::strtof(text.c_str() + equal + 1,
                                        &end_ptr);

        if (end_ptr == text.c_str() + equal + 1) {
            std::printf("Invalid parameter value in %s\n", filename.c_str());
            std::fclose(file);
            return false;
        }

        ParsedParam parsed{};
        parsed.param.module_index = current_module;
        parsed.param.param_id = current_param_id;

        if (!EncodeQ8_8(value, parsed.payload)) {
            std::printf("Q8.8 value out of range in %s\n", filename.c_str());
            std::fclose(file);
            return false;
        }

        parsed.param.param_payload = parsed.payload.data();
        parsed.param.size = parsed.payload.size();
        params.push_back(std::move(parsed));
    }

    std::fclose(file);
    return true;
}

static bool ReadParamFromConfFile(
    const char* main_config_filename,
    uint32_t frame_index,
    std::vector<ParsedParam>& params)
{
    params.clear();

    FILE* file = std::fopen(main_config_filename, "r");
    if (file == nullptr) {
        std::printf("Failed to open main config: %s\n",
                    main_config_filename);
        return false;
    }

    std::string selected_config;
    bool has_config_entries = false;
    char line[256]{};

    while (std::fgets(line, sizeof(line), file) != nullptr) {
        unsigned int config_frame = 0;
        char config_name[128]{};

        if (std::sscanf(line,
                        " :ConfigAtFrame(%u): %127[^;];",
                        &config_frame,
                        config_name) != 2 &&
            std::sscanf(line,
                        ":ConfigAtFrame(%u): %127[^;];",
                        &config_frame,
                        config_name) != 2) {
            continue;
        }

        has_config_entries = true;
        if (config_frame == frame_index) {
            selected_config = config_name;
            break;
        }
    }

    std::fclose(file);

    if (selected_config.empty()) {
        if (!has_config_entries) {
            std::printf("No config entries found in %s\n",
                        main_config_filename);
            return false;
        }

        return false;
    }

    const std::string main_path(main_config_filename);
    const std::size_t slash = main_path.find_last_of("/\\");

    const std::string config_path =
        slash == std::string::npos
            ? selected_config
            : main_path.substr(0, slash + 1) + selected_config;

    return ParseParameterFile(config_path, params);
}

static bool processWavFile(const char *inputFile, const char *outputFile)
{
    const u16 frameSize = 512;

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

    /*PLUGIN TEST*/
    RTAFE_MainPlugin* rtafe_plugin = new RTAFE_MainPlugin(config);

    std::vector<ParsedParam> params;
    uint32_t frame_index = 0;

    while (wav.bHasNextFrame()) {
        sample_t** channels = wav.ppReadFrame();

        if (!channels) {
            return false;
        }

        if (ReadParamFromConfFile("conf/main_conf.conf", frame_index, params)) {
            for (ParsedParam& parsed : params) {
                parsed.param.param_payload = parsed.payload.data();
                if (rtafe_plugin->SetParam(parsed.param) != kOk) {
                    std::printf("Failed to set parameter\n");
                    return false;
                }
                printf("\nSet parameter for module %u, param ID %u",
                        parsed.param.module_index,
                        parsed.param.param_id);
            }
            printf("\n");
        }

        rtafe_plugin->SetProcessFrame({
            channels,
            numChannels
        });

    #ifdef FIXED_POINT
        if (rtafe_plugin->ProcessFixed() != kOk) {
    #else
        if (rtafe_plugin->Process() != kOk) {
    #endif
            std::printf("DSP processing error\n");
            return false;
        }
        printf(".");
        wav.vWriteFrame(channels, frameSize);
        ++frame_index;
    }
    printf("\n");

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