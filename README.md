# HistogramViewer – PixInsight module

Native PCL/C++ port of `Histogram.js`: a non-destructive histogram and statistics
viewer (separate RGB curves, linear/log axes, zoom/range, percentiles, clipping
statistics). Based on the histogram view of Seti Astro Suite Pro (SASpro) by
Franklin Marek. GPL-3.0.

The process is called **HistogramViewer** (category *Tricx*); `Histogram` is
already taken by the PJSR core object of the same name. Parameter ids are the
same as in the script (`logX`, `logY`, `zoom`, `pan`, `stats`, `showRed`,
`showGreen`, `showBlue`).

## Build

Prebuilt (unsigned) modules for all platforms are produced by the
[Build workflow](.github/workflows/build.yml) and can be downloaded as
workflow artifacts from the *Actions* tab:

| Artifact | Platform |
|---|---|
| `HistogramViewer-macosx-arm64` | macOS, Apple Silicon |
| `HistogramViewer-macosx-x64` | macOS, Intel (cross-compiled on Apple Silicon) |
| `HistogramViewer-linux-x64` | Linux x86_64 (built on Ubuntu 22.04, GCC 12) |
| `HistogramViewer-windows-x64` | Windows x64 (Visual Studio 2022) |

The workflow builds against the public PCL repository
(https://gitlab.com/pixinsight/PCL, `master` by default; another ref can be
chosen when starting it manually).

### Local build

```sh
./build.sh                 # native architecture
./build.sh --arch=x64      # macOS: Intel build on Apple Silicon
./build.sh clean
```

- **macOS:** Xcode.
- **Linux:** GCC ≥ 12 and make.
- **Windows:** run from Git Bash with Visual Studio 2022 (MSBuild) in `PATH`.

PCL is taken from `build/PCL`. If that does not exist, the headers and sources
are copied from the local PixInsight installation (`--pi=<dir>` to override
the default location). The PCL libraries are built once per target into
`build/PCL/lib/<platform>-<arch>`, the module ends up in
`bin/<platform>-<arch>/`.

The public PCL repository lacks the Windows project of the PCL library itself;
[windows/vc17/PCL.vcxproj](windows/vc17/PCL.vcxproj) replaces it.

## Install

1. Sign the module (PixInsight only loads signed modules):
   `PixInsight --sign-module-file=bin/macosx-arm64/HistogramViewer-pxm.dylib --xssk-file=<key.xssk> --xssk-password=<password>`
2. In PixInsight: *Process › Modules › Install Modules…*, select the dylib.
3. Open it via *Process › Tricx › HistogramViewer*.

## Usage

- The viewer follows the active image while *Track View* is on; any image or
  preview can also be selected in the *Image* list.
- Drag the triangle in the control bar to create a process icon with the
  current display settings. Executing that icon on an image (or globally)
  opens the viewer for it; the image is never modified.
