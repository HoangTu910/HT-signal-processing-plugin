#ifndef HTSP_ERRORS_CODE_HPP
#define HTSP_ERRORS_CODE_HPP

typedef enum RTAFEErrorCode {
    kOk = 0,
    kErrorInvalidModuleParam,
    kErrorNullModuleParam,
    kErrorNullListOfModules,
    kErrorFileIO,
    kErrorUnsupportedFormat,
    kErrorMemoryAllocation,
    kErrorProcessing,
    kErrorInitAudioInputFailed,
    kErrorInitAudioOutputFailed,
    kErrorInvalidChannelCount,
} rtf_error_code_t;

#endif /* HTSP_ERRORS_CODE_HPP */   