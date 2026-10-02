# AudioReach adapter

This directory is the integration boundary between the RTAFE C++ core and the Qualcomm AudioReach CAPIv2 module contract.

The adapter is intentionally unimplemented. Future CAPIv2 code belongs here; the DSP modules under `src/module/` and the plugin implementation under `src/plugin/` should remain independent of AudioReach SDK headers.
