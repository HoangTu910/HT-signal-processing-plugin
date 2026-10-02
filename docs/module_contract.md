# RTAFE module contract

`src/module/module_contract.h` is the project-level module boundary for new modules. It is C-compatible and intentionally independent of the Qualcomm AudioReach SDK.

The contract follows the CAPIv2 lifecycle:

```text
get_static_properties -> init -> set_properties -> process
                                      ^             |
                                      |             v
                                   set_param      reset/end
```

## Module rules

- A module must not own input or output buffers.
- `process()` receives planar streams and must validate format, channel count, sample count, and byte width before touching data.
- Persistent algorithm state belongs in the caller-provided `state` region from `init()`; no runtime allocation is allowed for the bare-metal target.
- Parameter IDs and payload layouts must be documented per module. Do not pass untyped positional float arrays as the public contract.
- `reset()` clears algorithm history without changing static properties.
- `end()` releases only resources owned by the module. It must tolerate a null or already-ended context.
- Static properties describe capabilities; current properties describe the configured instance.

## AudioReach mapping

The future adapter in `src/audio_reach/` maps this contract to the AudioReach CAPIv2 SDK:

| RTAFE contract | AudioReach CAPIv2 role |
| --- | --- |
| `get_static_properties` | static properties query |
| `init` | module initialization |
| `process` | stream processing |
| `set_param` / `get_param` | parameter operations |
| `set_properties` / `get_properties` | media and port properties |
| `reset` | state reset or framework-specific reset parameter |
| `end` | module teardown |

The existing classes derived from `IDSPModule` now inherit the C++ `IRtafeModule` interface declared in the same header. The base class provides a compatibility bridge for the current `DSPBlock` implementation; new modules should implement the lifecycle contract directly where possible.
