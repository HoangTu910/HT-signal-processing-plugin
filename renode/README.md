# Renode Cortex-M3 integration

The RTAFE C++ sources are packaged as a static library first. The Renode firmware only contains startup code and a small harness that links that library; it does not compile the DSP modules as part of the firmware target.

The firmware runs the fixed-point library on a small Cortex-M3 Renode platform. It processes one deterministic 256-sample block on two channels and stores its result at `0x2007FF00`.

## Build the library

For normal host development:

```bash
./lib/build.sh --arch=host --type=FIXED_POINT
```

This creates `lib/build/host/librtafe.a`. A shared object can be added for Linux integration later, but a `.so` cannot be loaded directly by a microcontroller. The Cortex-M3 target must use a target-built `.a` archive.

## Prerequisites

On Fedora, install the ARM toolchain with:

```bash
sudo dnf install arm-none-eabi-gcc-cs arm-none-eabi-newlib
```

Install Renode from its Linux portable archive, then add its directory to `PATH`:

```bash
mkdir -p "$HOME/.local/opt/renode"
curl -L https://builds.renode.io/renode-latest.linux-portable.tar.gz \
	| tar -xz -C "$HOME/.local/opt/renode" --strip-components=1
export PATH="$HOME/.local/opt/renode:$PATH"
```

The build script does not use the repository's host `CC` or `CXX` variables:

```bash
export CC_RENODE=/path/to/arm-none-eabi-gcc
export CXX_RENODE=/path/to/arm-none-eabi-g++
```

## Build and run

From the repository root:

```bash
./renode/build.sh
renode --console renode/run.resc
```

`renode/build.sh` internally runs `./lib/build.sh --arch=cortex-m3 --type=FIXED_POINT`, then links `lib/build/cortex-m3/librtafe.a` with the startup code and harness.

The Renode script prints six words from the result record:

1. magic (`0x52544146`)
2. `HtspErrRet` status (`0` means `kOk`)
3. left-channel checksum
4. right-channel checksum
5. first left output sample
6. first right output sample

The platform is intentionally small and deterministic. To move to a real MCU model, keep the firmware ELF and linker memory map aligned with that board, then replace `lm3s6965.repl` and the corresponding flash/RAM origins in `renode.ld`.
