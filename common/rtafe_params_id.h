#ifndef RTAFE_PARAMS_ID_H
#define RTAFE_PARAMS_ID_H

enum rtafe_module_param_id {
    kRtafeParamIdDCRemoval_Alpha = 0,
    kRtafeParamIdPreEmphasis_Alpha,
    kRtafeParamIdNoiseSuppress_Alpha,
    kRtafeParamIdVolumeControl_Gain,
};

enum rtafe_module_id {
    kRtafeDCRemoval = 0,
    kRtafePreEmphasis,
    kRtafeNoiseSuppress,
    kRtafeVolumeControl,
};

#endif