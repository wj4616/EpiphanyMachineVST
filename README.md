# Epiphany Machine VST

**A creative ambient glitch reverb VST3 plugin built with JUCE + CMake**

> Texture-aware diffusion, granular pitch drift, dual-band filtering, and 34 factory presets with smooth morphing — all in a 950×580 cyberpunk GUI.

[![Platform: Linux](https://img.shields.io/badge/Platform-Linux-blue.svg)]()
[![Format: VST3](https://img.shields.io/badge/Format-VST3-ff3060.svg)]()
[![JUCE 7](https://img.shields.io/badge/JUCE-7.0.12-b44aff.svg)](https://juce.com)

---

### [epiphanymachineart.com](https://epiphanymachineart.com)

---

[User Manual & Sound Design Guide — v2.0](https://github.com/wj4616/EpiphanyMachineVST/blob/main/epiphany-machine-VST-v2-manual-full.md)

By the way, you want to download the Epiphany Machine.so file and place in correct location on your system. Full source code is provided. sha512 checksum hash provided at bottom.

## What Is It?

Epiphany Machine is a creative reverb and glitch processor for ambient, experimental, and cinematic music production. It uses a **texture-body detection system** to split your input signal into transient texture and sustained body components, routing each through independently tunable DSP chains.

The result: reverbs that breathe, glitch architectures that respond to your playing dynamics, and evolving filter modulation that reacts to the signal in real time.

---

## Features

### Signal Path
```
INPUT >> DETECT >> GLITCH >> DIFFUSE >> FILTER >> OUTPUT
```

| Stage | What It Does |
|---|---|
| **Detect** | Dual envelope follower splits signal into texture (transients) and body (sustain) |
| **Glitch** | Granular buffer scrambler with density and chaos controls |
| **Glitch Focus** | Routes glitch engine toward body, texture, or a blend of both |
| **Diffuse** | 4-line feedback delay network (FDN) reverb with sine LFO modulation per line |
| **Pitch Drift** | Dual-head granular pitch shifter with Hann window — subtle detuning on the reverb tail |
| **Stereo Widener** | Mid/side matrix width control |
| **Dual Filter** | Texture (A) and Body (B) independent filter bands with LFO modulation |
| **Mix** | Dry/wet blend with cosine interpolation |

### DSP Highlights
- **Cytomic TPT State Variable Filter** — stable at all frequencies, click-free during LFO modulation
- **4-line FDN reverb** with Hadamard matrix mixing and prime delay lengths (1087, 1283, 1637, 1933 samples at 48kHz)
- **Dual envelope follower** — fast (5ms attack / 50ms release) minus slow (100ms attack / 500ms release) = texture signal
- **Granular pitch drifter** with precomputed Hann window, auto-bypassed at zero drift
- **Denormal protection** via `juce::ScopedNoDenormals` + DC offset in FDN feedback path
- **Soft clip** on output and freeze feedback path: `x / (1 + |x|)`

### Freeze Mode
Holds the reverb tail indefinitely. Extra soft-clipping in the feedback path prevents runaway energy when DRIFT and FEEDBACK are both active on a frozen tail. Freeze state is smoothed over ~42ms to avoid clicks.

### Dual Filter
Two independent filter bands — **Texture (A)** and **Body (B)** — each with:
- LP / HP / BP mode selector
- Cutoff (20Hz–20kHz)
- Resonance
- LFO rate + depth (Texture band only)
- Filter position: **OFF / WET / FINAL** — insert the filter pre-mix or post-mix

### 34 Factory Presets with Smooth Morphing
Presets are organized across 6 sonic categories:

| Category | Presets |
|---|---|
| Deep Ambient | Void Meditation, Eternal Hall, Cathedral of Dust, Weightless, Frozen Lake |
| Shimmer | Phantom Drift, Descending Angel, Fifth Dimension, Dissonant Cloud, Tritone Abyss, Solar Flare |
| Glitch Ambient | Neural Storm, Broken Transmission, Scattered Memories, Stutter Prayer, Insect Choir, Living Texture |
| Deep & Dark | Deep Submersion, Tectonic Drone, Obsidian Mirror, Gravity Well, Midnight Fog |
| Crystalline & Metallic | Crystal Lattice, Bell Garden, Ice Fracture, Singing Metal, Prism |
| Experimental | Event Horizon, Ghost in the Machine, Parallel Universe, Temporal Collapse, The Long Exhale, Machine Awakening |

**Morph system features:**
- Adjustable morph time (0.1s – 30s)
- Log-space interpolation for frequencies
- Cosine interpolation for mix
- User knob gestures during morph automatically exclude that parameter from the transition

---

## Parameters (25 Total)

| ID | Name | Range | Description |
|---|---|---|---|
| `sensitivity` | Sensitivity | 0–1 | Texture detector threshold |
| `glitchFocus` | Glitch Focus | 0–1 | 0=body, 1=texture routing to glitch engine |
| `density` | Density | 0–1 | Glitch engine playback density |
| `chaos` | Chaos | 0–1 | Glitch randomization amount |
| `decay` | Decay | 0.1–20s | FDN reverb decay time |
| `damping` | Damping | 0–1 | High-frequency rolloff in reverb |
| `width` | Width | 0–1 | Stereo width (mid/side) |
| `drift` | Drift | 0–24st | Granular pitch drift amount |
| `feedback` | Feedback | 0–0.98 | FDN feedback gain |
| `mix` | Mix | 0–1 | Dry/wet blend |
| `morph` | Morph | 0–1 | Preset morph position |
| `morphTime` | Morph Time | 0.1–30s | Duration of preset transitions |
| `bypass` | Bypass | on/off | Master bypass (smoothed) |
| `freeze` | Freeze | on/off | Freeze reverb tail |
| `filterPos` | Filter Position | Off/Wet/Final | Where dual filter is inserted |
| `texFilterType` | Texture Filter | LP/HP/BP | Texture band filter mode |
| `texFilterCutoff` | Tex Cutoff | 20Hz–20kHz | Texture band cutoff |
| `texFilterReso` | Tex Reso | 0.1–4.0 | Texture band resonance |
| `texLfoRate` | LFO Rate | 0.01–8Hz | Texture band LFO speed |
| `texLfoDepth` | LFO Depth | 0–1 | Texture band LFO depth |
| `bodyFilterType` | Body Filter | LP/HP/BP | Body band filter mode |
| `bodyFilterCutoff` | Body Cutoff | 20Hz–20kHz | Body band cutoff |
| `bodyFilterReso` | Body Reso | 0.1–4.0 | Body band resonance |
| `preset` | Preset | 34 options | Factory preset selector |

---

## Building From Source

### Requirements
- Linux (tested on Linux Mint)
- CMake 3.22+
- GCC or Clang with C++17 support
- JUCE 7 (fetched automatically via CMake FetchContent)

### Build
```bash
git clone https://github.com/wj4616/EpiphanyMachineVST.git
cd EpiphanyMachineVST
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --target EpiphanyMachine_VST3
```

The plugin is automatically installed to `~/.vst3/` after a successful build.

### DAW Setup (REAPER on Linux)
1. Options → Preferences → Plug-ins → VST
2. Add `~/.vst3` to the VST path
3. Re-scan plugins
4. Add **Epiphany Machine** to an FX chain

---

## Project Structure

```
EpiphanyMachineVST/
├── CMakeLists.txt
└── src/
    ├── PluginProcessor.h / .cpp     # APVTS, signal routing, processBlock
    ├── PluginEditor.h / .cpp        # 950x580 GUI, 5-column layout
    ├── DSP/
    │   ├── GlitchEngine             # Granular buffer scrambler
    │   ├── DiffusionNetwork         # 4-line FDN reverb
    │   ├── PitchDrifter             # Dual-head granular pitch shifter
    │   ├── StereoWidener            # Mid/side matrix
    │   ├── TextureBodyDetector      # Dual envelope follower
    │   └── DualFilter               # Cytomic TPT SVF, dual band
    ├── GUI/
    │   ├── CustomLookAndFeel        # Cyberpunk colour palette + knob drawing
    │   ├── MorphKnob                # Animated morph ring indicator
    │   └── PresetBar                # Preset nav + morph time control
    └── Presets/
        ├── PresetData.h             # 34 factory preset structs
        └── PresetManager            # Morph engine, gesture exclusion
```

---

## GUI

950×580 pixel interface with a cyberpunk colour scheme (.png image in repository):

| Colour | Hex | Used For |
|---|---|---|
| Background | `#0d0a1a` | Main background |
| Surface | `#1a1430` | Column panels |
| Neon purple | `#b44aff` | Knobs, borders, headers |
| Gold | `#f0c040` | Value labels, morph ring, link |
| Danger red | `#ff3060` | Bypass button active state |
| Filter blue | `#6a3aff` | Dual filter section accent |

The MORPH knob uses an animated colour ring that transitions neon-secondary → neon-primary → gold as presets blend.

---

## Links

- **Website:** [epiphanymachineart.com](https://epiphanymachineart.com)
- **GitHub:** [github.com/wj4616/EpiphanyMachineVST](https://github.com/wj4616/EpiphanyMachineVST)

---

## Technical References

- [Cytomic TPT SVF filter design](https://cytomic.com/files/dsp/SvfLinearTrapOptimised2.pdf) — Andy Simper's optimised state variable filter
- [JUCE framework](https://juce.com)
- Feedback delay networks: Jot & Chaigne, "Digital Delay Networks for Designing Artificial Reverberators" (1991)

Epiphany Machine.so sha512sum 8e72d4616e0bc9577be5fc60ce846755c2f8c989a22d654af5d08a1e3ba707fb5c683fd24c935fff78d2aea8828a865f74a69669d73e7e4e37d0fc4ac5480a09 
