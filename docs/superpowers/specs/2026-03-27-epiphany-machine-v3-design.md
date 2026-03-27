# Epiphany Machine VST v3 Design Spec

**Date:** 2026-03-27
**Status:** Approved — awaiting implementation plan
**Goal:** Transform Epiphany Machine from a subtle ambient reverb into a psychedelic sound design instrument capable of everything from gentle textures to harsh, wild transformations.

---

## Problem Statement

The v2 plugin's effect is not noticeable enough. Multiple layers of conservative gain staging compound to suppress the signal:

1. **-6dB FDN output** — only 2 of 4 delay lines summed per channel
2. **0.95 decay multiplier** — caps effective decay below parameter value
3. **8kHz fixed feedback LPF** — rolls off all high-frequency content in the feedback loop
4. **StereoWidener soft clip** — `x / (1 + |x|)` on every sample compresses anything above 0.7
5. **Double freeze soft clip** — extra compression during freeze mode
6. **Tame filter Q range** — max Q ~30, no self-oscillation possible
7. **Conservative glitch engine** — grains limited to 10ms-500ms, chaos gated below 0.7

The plugin needs to serve as a primary modulation source in a synth's effect signal path, producing psychedelic sounds that are weird and interesting but tolerable by human listeners.

---

## Design Principles

1. **Every knob should have a noticeable range** — subtle at minimum, extreme at maximum
2. **Drive is opt-in intensity** — without DRIVE, the signal shouldn't run hot
3. **One limiter at the end** — `tanh()` output limiting replaces multiple internal soft clips
4. **Parameters do double duty** — minimize knob count, maximize capability
5. **Quantum physics preset theme** — 18 curated presets showcasing the full range

---

## Signal Flow

```
Input
  |
  v
TextureBodyDetector (dual envelope follower, two instances)
  |
  v
Glitch Focus (routes texture, body, or blend to glitch engine)
  |
  v
GlitchEngine (1ms-1000ms grains, adaptive envelopes, DRIFT pitch warp on grains)
  |
  v
Glitch wet/dry blend (density/chaos determine how much glitch is present)
  |
  v
(+feedback return) ──────────────────────────────────┐
  |                                                   |
  v                                                   |
8-line FDN (Hadamard matrix, per-line stereo panning) |
  |                                                   |
  v                                                   |
Feedback send:                                        |
  PitchDrifter (±24st, dual-head granular)            |
  → DualFilter [if filterPos == "Feedback"]           |
  → Wavefolder (DRIVE parameter)                      |
  → soft clip (safety only)                           |
  → back to FDN ─────────────────────────────────────┘
  |
  v
DualFilter [if filterPos == "Wet" or "Final"]
  |
  v
MIX (dry/wet cosine interpolation)
  |
  v
tanh() output limiter
  |
  v
Output
```

### Key Routing Changes from v2

- **Feedback loop now contains:** PitchDrifter → optional DualFilter → Wavefolder → soft clip
- **DualFilter gains "Feedback" position:** filter inside the feedback loop creates resonant, self-exciting behavior that accumulates over time
- **Single output limiter:** replaces 4+ internal soft clips
- **StereoWidener removed** as separate module — replaced by per-line FDN panning with independent LFOs
- **WIDTH parameter** now controls per-line panning depth in the FDN directly

---

## Gain Staging Overhaul

### Removals

| What | Where | Why Remove |
|---|---|---|
| `-6dB FDN output` | DiffusionNetwork output sum | Was only using 2 of 4 lines per channel |
| `0.95 decay multiplier` | `decayParam * 0.95f` | Caps user's decay below what they set |
| `8kHz feedback LPF` | PluginProcessor feedback path | Removes all brightness from feedback |
| `StereoWidener soft clip` | `x / (1 + abs(x))` per sample | Compresses everything above 0.7 |
| `Double freeze soft clip` | Extra clip during freeze | Unnecessary with tanh output |

### New Values

| Parameter | v2 | v3 |
|---|---|---|
| FDN output normalization | `(line0 + line1) * 0.5` (2 lines) | `sum_all_8_lines * (1 / sqrt(8))` |
| Decay range | 0.1–20s, `* 0.95` | 0.0–20s, no multiplier |
| Feedback cap | 0.98 | 0.95 (lower cap, but louder actual signal) |
| Filter Q range | 0.5–30 | 0.5–50 (self-oscillating above ~40) |
| DRIFT range | ±12st | ±24st |
| Grain length | 10ms–500ms | 1ms–1000ms |
| LFO filter depth | ±1 octave | ±3 octaves |
| Damping range | 0–0.9 | 0–1.0 |
| LFO modulation depth (FDN) | 3–6 samples | 10–30 samples |

### Safety

- **Wavefolder output normalization:** `sin(x * foldAmount * PI) / foldAmount` crossfaded with dry signal by drive amount — true bypass at drive=0, adds harmonic complexity without volume increase at higher drive
- **Feedback soft clip:** single `x / (1 + |x|)` at the end of the feedback send chain (safety net only)
- **Output limiter:** `tanh()` on final output — the one and only loudness limiter
- **DRIVE at 0:** wavefolder is bypassed, signal passes through clean

---

## DSP Components

### 1. TextureBodyDetector (unchanged from v2)

Two instances of dual envelope follower.

- Fast envelope: 5ms attack / 50ms release
- Slow envelope: 100ms attack / 500ms release
- Texture = fast - slow (transient detection)
- Body = slow envelope (sustain detection)
- `sensitivityGain = 1.0f + sensitivity * 7.0f` (1x–8x), clamped to [0,1]

### 2. GlitchEngine (redesigned)

**Buffer:** 131072 samples (~2.7s at 48kHz) — unchanged

**Grain length range:** 1ms–1000ms (was 10ms–500ms)
- Density 0.0: grains 500ms–1000ms (long tape-loop textures)
- Density 1.0: grains 1ms–50ms (micro-granular clouds)

**Adaptive grain envelopes** (based on grain length):
- `> 50ms`: Hann window (smooth, musical)
- `10ms–50ms`: Tukey window (alpha=0.3, flatter sustain)
- `< 10ms`: Trapezoid (~0.5ms linear attack/release, maximum content)

**Chaos behavior** (full range, no 0.7 gate):
- Chaos 0.0: sequential playback from recent buffer
- Chaos 0.3: occasional random position jumps
- Chaos 0.5: 20% reverse grains, random positions
- Chaos 0.7: 40% reverse, 20% half-speed
- Chaos 1.0: 60% reverse, 40% half-speed, 20% double-speed, random from full buffer

**DRIFT pitch warp on grains:**
- Each grain gets a random pitch shift within ±DRIFT semitones
- Implemented via playback rate modification: `rate = pow(2, randomDrift / 12.0)`
- This is IN ADDITION to the PitchDrifter in the feedback loop
- At DRIFT=0, grains play at original pitch

**Silence handling:** Remove the -80dBFS silence gate that clears the buffer. Instead, let the buffer accumulate — the glitch engine should work with whatever is in the buffer, including fading tails.

### 3. DiffusionNetwork (8-line FDN)

**Upgrade from 4 to 8 delay lines.**

**Delay lengths** (prime numbers, samples at 48kHz):
```
{ 1087, 1283, 1637, 1933, 2311, 2719, 3109, 3511 }
```

**8x8 Hadamard matrix** for delay line mixing (normalized by `1/sqrt(8)`).

**Per-line stereo panning:**
- Each delay line has an independent pan position modulated by a slow LFO
- LFO rates (Hz): `{ 0.07, 0.11, 0.15, 0.19, 0.23, 0.29, 0.31, 0.37 }`
- Pan depth controlled by WIDTH parameter (0 = mono center, 1 = full L/R spread)
- Base pan positions alternate L/R: `{ -0.8, 0.8, -0.4, 0.4, -0.6, 0.6, -0.2, 0.2 }`
- This replaces the separate StereoWidener module entirely

**FDN modulation depth:** 10–30 samples per line (was 3–6), controlled by existing sine LFOs per line.

**Damping:** Full 0–1.0 range (was 0–0.9). Implemented as one-pole lowpass per line in the feedback path.

**Decay:** Direct parameter value used (no `* 0.95` multiplier). Range 0.0–20s.

**Output:** Sum all 8 lines with `1/sqrt(8)` normalization, applying per-line pan positions.

**Buffer size:** 4096 samples per line (sufficient for longest delay at 3511).

### 4. PitchDrifter (in feedback loop)

- Dual-head granular pitch shifter with Hann window crossfade
- Buffer: 4096 samples, grain size: 2048 samples
- Range: ±24st (was ±12st)
- Lives in the feedback loop — pitch shift accumulates over iterations
- At DRIFT=0, bypassed entirely (pass-through)

### 5. Wavefolder (new — in feedback loop)

**Formula:**
```cpp
float foldAmount = 1.0f + drive * 4.0f;
float folded = std::sin(input * foldAmount * PI) / foldAmount;
float output = input + drive * (folded - input);  // crossfade: dry at drive=0, full fold at drive=1
```

Where `foldAmount = 1.0 + drive * 4.0` (drive parameter range 0–1).

**Behavior:**
- At drive=0: output = input (true bypass — crossfade produces clean passthrough)
- At drive=0.25: `foldAmount=2.0`, 25% blend of folded signal — subtle harmonic color
- At drive=0.5: `foldAmount=3.0`, 50% blend — noticeable harmonic addition
- At drive=1.0: `foldAmount=5.0`, 100% folded signal — aggressive folding, rich harmonics, volume-normalized by `/foldAmount`

**Key property:** The crossfade ensures drive=0 is a true bypass (no coloring). Division by `foldAmount` normalizes the folded signal's volume. Drive is opt-in — at 0, the feedback path is completely clean.

**Position in chain:** After PitchDrifter, before feedback soft clip. This means the wavefolder processes already-pitch-shifted material, and its output feeds back into the FDN — harmonics accumulate over feedback iterations.

### 6. DualFilter (enhanced)

**Cytomic TPT State Variable Filter** — same topology, expanded range.

**Q range:** 0.5–50 (was 0.5–30). At Q > ~40, the filter self-oscillates, becoming a sine oscillator at the cutoff frequency.

**New `filterPos` option — "Feedback":**
- Off: filter bypassed
- Wet: filter applied to wet signal before mix
- Final: filter applied to mixed signal
- **Feedback: filter inserted in the feedback loop** (after PitchDrifter, before Wavefolder)

**Filter in feedback loop behavior:**
- The filter processes material on every feedback iteration
- Resonance accumulates — even moderate Q values create pronounced peaks after several iterations
- Self-oscillation (high Q) + feedback creates tonal, pitched reverb tails
- Combined with Wavefolder: filter shapes the harmonic content that gets folded

**LFO depth:** ±3 octaves (was ±1 octave). Texture band LFO only.

**Both bands** (Texture A, Body B) retain independent LP/HP/BP mode, cutoff, and resonance.

### 7. Output Limiter

Single `tanh()` applied to the final stereo output.

```cpp
outL = std::tanh(outL);
outR = std::tanh(outR);
```

This replaces all internal soft clips. `tanh` provides smooth saturation that preserves signal character while preventing digital clipping.

---

## Parameters (25 Total)

| # | ID | Name | Range | Default | Description |
|---|---|---|---|---|---|
| 1 | `sensitivity` | Sensitivity | 0–1 | 0.5 | Texture detector threshold |
| 2 | `glitchFocus` | Glitch Focus | 0–1 | 0.5 | 0=body, 1=texture routing to glitch engine |
| 3 | `density` | Density | 0–1 | 0.3 | Glitch grain density (long→micro) |
| 4 | `chaos` | Chaos | 0–1 | 0.2 | Glitch randomization (sequential→wild) |
| 5 | `decay` | Decay | 0.0–20s | 2.0 | FDN reverb decay time |
| 6 | `damping` | Damping | 0–1 | 0.3 | High-frequency rolloff in reverb |
| 7 | `width` | Width | 0–1 | 0.7 | Per-line FDN stereo panning depth |
| 8 | `drift` | Drift | 0–24st | 0.0 | Pitch drift amount (feedback + grains) |
| 9 | `feedback` | Feedback | 0–0.95 | 0.5 | FDN feedback gain |
| 10 | `drive` | Drive | 0–1 | 0.0 | Wavefolder intensity (NEW) |
| 11 | `mix` | Mix | 0–1 | 0.5 | Dry/wet blend (cosine interpolation) |
| 12 | `morph` | Morph | 0–1 | 0.0 | Preset morph position |
| 13 | `morphTime` | Morph Time | 0.1–30s | 2.0 | Duration of preset transitions |
| 14 | `bypass` | Bypass | on/off | off | Master bypass (smoothed) |
| 15 | `freeze` | Freeze | on/off | off | Freeze reverb tail |
| 16 | `filterPos` | Filter Position | Off/Wet/Final/Feedback | Off | Where dual filter is inserted |
| 17 | `texFilterType` | Texture Filter | LP/HP/BP | LP | Texture band filter mode |
| 18 | `texFilterCutoff` | Tex Cutoff | 20Hz–20kHz | 2000 | Texture band cutoff |
| 19 | `texFilterReso` | Tex Reso | 0.1–4.0 | 0.5 | Texture band resonance (maps to Q 0.5–50) |
| 20 | `texLfoRate` | LFO Rate | 0.01–8Hz | 0.5 | Texture band LFO speed |
| 21 | `texLfoDepth` | LFO Depth | 0–1 | 0.0 | Texture band LFO depth (±3 octaves at max) |
| 22 | `bodyFilterType` | Body Filter | LP/HP/BP | LP | Body band filter mode |
| 23 | `bodyFilterCutoff` | Body Cutoff | 20Hz–20kHz | 1000 | Body band cutoff |
| 24 | `bodyFilterReso` | Body Reso | 0.1–4.0 | 0.5 | Body band resonance (maps to Q 0.5–50) |
| 25 | `preset` | Preset | 18 options | INIT | Factory preset selector |

**Changes from v2:**
- **Added:** `drive` (wavefolder intensity)
- **Modified:** `filterPos` gains "Feedback" option
- **Modified:** `drift` now affects both feedback path and glitch grains
- **Modified:** `width` now controls FDN per-line panning (was stereo widener amount)
- **Removed:** none (parameter IDs preserved for DAW compatibility where possible)

---

## Presets (18 — Quantum Physics Theme)

### Category: Foundations (gentle, entry-level)

**1. INIT**
Default state, all effects minimal, clean starting point.
```
sensitivity=0.5, glitchFocus=0.5, density=0.0, chaos=0.0,
decay=1.0, damping=0.3, width=0.5, drift=0.0, feedback=0.3,
drive=0.0, mix=0.3, filterPos=Off
```

**2. Residual Spin**
Gentle room-like reverb with slow spatial drift. Subtle width modulation.
```
sensitivity=0.3, glitchFocus=0.5, density=0.0, chaos=0.0,
decay=2.5, damping=0.4, width=0.8, drift=0.5, feedback=0.4,
drive=0.0, mix=0.4, filterPos=Off
```

### Category: Ambient Textures

**3. Foam at the Planck Scale**
Micro-granular cloud with high density, subtle pitch drift. LP filter sweeps create frothy texture.
```
sensitivity=0.7, glitchFocus=0.8, density=0.9, chaos=0.4,
decay=4.0, damping=0.5, width=0.9, drift=3.0, feedback=0.5,
drive=0.0, mix=0.6, filterPos=Wet,
texFilterType=LP, texFilterCutoff=4000, texFilterReso=1.0,
texLfoRate=0.3, texLfoDepth=0.4,
bodyFilterType=LP, bodyFilterCutoff=2000, bodyFilterReso=0.5
```

**4. Both States Neither State**
Superposition-inspired dual processing — texture and body treated as opposing states. Glitch focus at center, moderate everything.
```
sensitivity=0.5, glitchFocus=0.5, density=0.4, chaos=0.3,
decay=6.0, damping=0.3, width=0.7, drift=2.0, feedback=0.6,
drive=0.1, mix=0.5, filterPos=Wet,
texFilterType=HP, texFilterCutoff=3000, texFilterReso=0.8,
texLfoRate=0.2, texLfoDepth=0.3,
bodyFilterType=LP, bodyFilterCutoff=1500, bodyFilterReso=0.8
```

**5. Spooky Geometry**
Wide stereo field with pitch-shifted feedback creating non-local spatial relationships.
```
sensitivity=0.4, glitchFocus=0.3, density=0.2, chaos=0.2,
decay=8.0, damping=0.2, width=1.0, drift=7.0, feedback=0.7,
drive=0.05, mix=0.5, filterPos=Off
```

### Category: Deep & Evolving

**6. The Invisible 96%**
Dark matter reverb — heavy damping, long decay, body-focused. LP filter removes everything bright; what remains is the invisible mass.
```
sensitivity=0.3, glitchFocus=0.2, density=0.1, chaos=0.1,
decay=15.0, damping=0.8, width=0.6, drift=1.0, feedback=0.8,
drive=0.1, mix=0.7, filterPos=Wet,
texFilterType=LP, texFilterCutoff=800, texFilterReso=0.5,
texLfoRate=0.05, texLfoDepth=0.2,
bodyFilterType=LP, bodyFilterCutoff=500, bodyFilterReso=0.7
```

**7. Through the Forbidden Gap**
Bandpass filter in feedback loop creates resonant peaks that shift over time. Material "tunnels" through.
```
sensitivity=0.5, glitchFocus=0.5, density=0.3, chaos=0.3,
decay=5.0, damping=0.2, width=0.7, drift=4.0, feedback=0.7,
drive=0.2, mix=0.6, filterPos=Feedback,
texFilterType=BP, texFilterCutoff=1200, texFilterReso=2.5,
texLfoRate=0.15, texLfoDepth=0.5,
bodyFilterType=BP, bodyFilterCutoff=600, bodyFilterReso=2.0
```

**8. The Unopened Box**
Freeze-ready preset. Activate freeze to "observe" (collapse) the sound. Without freeze: gentle evolving pad.
```
sensitivity=0.4, glitchFocus=0.4, density=0.2, chaos=0.2,
decay=10.0, damping=0.3, width=0.8, drift=3.0, feedback=0.6,
drive=0.0, mix=0.6, filterPos=Wet,
texFilterType=LP, texFilterCutoff=5000, texFilterReso=0.5,
texLfoRate=0.1, texLfoDepth=0.3,
bodyFilterType=LP, bodyFilterCutoff=3000, bodyFilterReso=0.5
```

### Category: Harmonic & Resonant

**9. Collapsing the Eigenstate**
High-Q filter in feedback loop creates pitched resonances. Drive adds harmonic complexity.
```
sensitivity=0.6, glitchFocus=0.6, density=0.3, chaos=0.4,
decay=4.0, damping=0.15, width=0.7, drift=5.0, feedback=0.75,
drive=0.4, mix=0.6, filterPos=Feedback,
texFilterType=BP, texFilterCutoff=2000, texFilterReso=3.5,
texLfoRate=0.4, texLfoDepth=0.6,
bodyFilterType=LP, bodyFilterCutoff=4000, bodyFilterReso=1.5
```

**10. Momentum of the Unknown**
Rising filter sweeps with feedback drive. Heisenberg-inspired — the more you probe, the less certain the pitch.
```
sensitivity=0.7, glitchFocus=0.7, density=0.5, chaos=0.5,
decay=3.0, damping=0.2, width=0.8, drift=8.0, feedback=0.65,
drive=0.3, mix=0.65, filterPos=Feedback,
texFilterType=HP, texFilterCutoff=1500, texFilterReso=2.0,
texLfoRate=0.8, texLfoDepth=0.7,
bodyFilterType=BP, bodyFilterCutoff=800, bodyFilterReso=1.5
```

**11. The Dirac Sea**
Self-oscillating filter creates pitched tones from the "vacuum." High resonance, moderate drive.
```
sensitivity=0.3, glitchFocus=0.3, density=0.1, chaos=0.1,
decay=7.0, damping=0.1, width=0.6, drift=2.0, feedback=0.7,
drive=0.25, mix=0.5, filterPos=Feedback,
texFilterType=BP, texFilterCutoff=440, texFilterReso=3.8,
texLfoRate=0.05, texLfoDepth=0.2,
bodyFilterType=LP, bodyFilterCutoff=2000, bodyFilterReso=2.0
```

### Category: Glitch & Chaos

**12. 10^32 Kelvin**
Big Bang intensity. Maximum density micro-grains, high chaos, drive cranked. Pure energy.
```
sensitivity=0.8, glitchFocus=0.9, density=1.0, chaos=0.9,
decay=2.0, damping=0.1, width=1.0, drift=12.0, feedback=0.6,
drive=0.8, mix=0.8, filterPos=Wet,
texFilterType=HP, texFilterCutoff=2000, texFilterReso=1.5,
texLfoRate=4.0, texLfoDepth=0.8,
bodyFilterType=BP, bodyFilterCutoff=1000, bodyFilterReso=1.0
```

**13. False Vacuum Decay**
Starts normal, builds to catastrophic. Moderate settings with high feedback and drive — the instability grows.
```
sensitivity=0.5, glitchFocus=0.5, density=0.4, chaos=0.5,
decay=6.0, damping=0.2, width=0.7, drift=6.0, feedback=0.85,
drive=0.5, mix=0.6, filterPos=Feedback,
texFilterType=LP, texFilterCutoff=6000, texFilterReso=1.5,
texLfoRate=0.3, texLfoDepth=0.4,
bodyFilterType=LP, bodyFilterCutoff=3000, bodyFilterReso=1.0
```

**14. Scattered Memories (Spaghettification)**
Extreme pitch stretch + long grains. Sound pulled apart like matter near a black hole.
```
sensitivity=0.4, glitchFocus=0.6, density=0.15, chaos=0.6,
decay=8.0, damping=0.3, width=0.9, drift=20.0, feedback=0.5,
drive=0.15, mix=0.7, filterPos=Wet,
texFilterType=LP, texFilterCutoff=3000, texFilterReso=0.8,
texLfoRate=0.1, texLfoDepth=0.3,
bodyFilterType=LP, bodyFilterCutoff=1500, bodyFilterReso=0.5
```

### Category: Experimental / Extreme

**15. Vibrating in Eleven**
String theory — 11 dimensions of modulation. Everything active at moderate-high levels.
```
sensitivity=0.7, glitchFocus=0.7, density=0.6, chaos=0.6,
decay=5.0, damping=0.2, width=1.0, drift=7.0, feedback=0.75,
drive=0.35, mix=0.7, filterPos=Feedback,
texFilterType=BP, texFilterCutoff=3000, texFilterReso=2.5,
texLfoRate=1.5, texLfoDepth=0.6,
bodyFilterType=HP, bodyFilterCutoff=500, bodyFilterReso=2.0
```

**16. Information Bleed**
Hawking radiation — information escaping the reverb. High feedback with HP filter slowly reveals hidden content.
```
sensitivity=0.6, glitchFocus=0.4, density=0.3, chaos=0.4,
decay=12.0, damping=0.6, width=0.8, drift=4.0, feedback=0.8,
drive=0.2, mix=0.5, filterPos=Feedback,
texFilterType=HP, texFilterCutoff=4000, texFilterReso=1.5,
texLfoRate=0.2, texLfoDepth=0.5,
bodyFilterType=HP, bodyFilterCutoff=1000, bodyFilterReso=1.0
```

**17. Symmetry Breaking**
Asymmetric processing — texture HP, body LP, wide stereo, glitch focused on texture. The symmetry of the input is broken.
```
sensitivity=0.8, glitchFocus=1.0, density=0.7, chaos=0.7,
decay=3.5, damping=0.3, width=1.0, drift=10.0, feedback=0.6,
drive=0.5, mix=0.75, filterPos=Final,
texFilterType=HP, texFilterCutoff=5000, texFilterReso=2.0,
texLfoRate=2.0, texLfoDepth=0.7,
bodyFilterType=LP, bodyFilterCutoff=400, bodyFilterReso=1.5
```

**18. Naked Singularity**
Maximum exposure — high drive, high feedback, filter in feedback loop with near-self-oscillation. Raw, unshielded.
```
sensitivity=0.6, glitchFocus=0.5, density=0.5, chaos=0.8,
decay=4.0, damping=0.1, width=0.9, drift=15.0, feedback=0.9,
drive=0.9, mix=0.85, filterPos=Feedback,
texFilterType=BP, texFilterCutoff=1500, texFilterReso=3.5,
texLfoRate=1.0, texLfoDepth=0.8,
bodyFilterType=BP, bodyFilterCutoff=700, bodyFilterReso=3.0
```

---

## Preset Morph System (unchanged from v2)

- Adjustable morph time (0.1s–30s)
- Log-space interpolation for frequency parameters (cutoffs)
- Cosine interpolation for mix parameter
- Linear interpolation for all other parameters
- User knob gestures during morph automatically exclude that parameter from the transition
- Morph position stored as 0–1 continuous value between current and target preset

---

## GUI Changes

### New Element
- **DRIVE knob** — added to the signal flow section, placed near FEEDBACK in the layout
- Knob style matches existing cyberpunk aesthetic (neon purple ring, gold value label)

### Modified Elements
- **Filter Position selector** — gains 4th option "Feedback" (was Off/Wet/Final)
- **Preset bar** — updated for 18 presets (was 34), new quantum physics names

### Unchanged
- 950x580 pixel window size
- 5-column layout structure
- Cyberpunk color scheme
- MorphKnob animated ring
- All other knob positions and styling

---

## Implementation Notes

### Files to Modify
- `src/DSP/DiffusionNetwork.h/.cpp` — 8-line FDN, per-line panning, gain staging
- `src/DSP/GlitchEngine.h/.cpp` — grain range, adaptive envelopes, drift on grains, chaos curve
- `src/DSP/PitchDrifter.h/.cpp` — ±24st range
- `src/DSP/DualFilter.h/.cpp` — Q range expansion, self-oscillation, LFO depth
- `src/DSP/StereoWidener.h/.cpp` — REMOVE (functionality absorbed into FDN)
- `src/PluginProcessor.h/.cpp` — new signal routing, wavefolder, drive parameter, feedback loop changes, remove internal soft clips
- `src/PluginEditor.h/.cpp` — DRIVE knob, filter position update, preset bar update
- `src/Presets/PresetData.h` — 18 new presets replacing 34 old ones
- `src/Presets/PresetManager.h/.cpp` — updated for new preset count and drive parameter
- `src/GUI/PresetBar.h/.cpp` — updated for 18 presets

### New Code (in PluginProcessor or new file)
- Wavefolder implementation (small enough to inline in PluginProcessor)

### Performance Considerations
- 8-line FDN doubles delay line memory (4→8 lines × 4096 samples × 2 channels = 256KB)
- 8×8 Hadamard matrix multiply per sample (64 multiply-adds vs 16 for 4×4)
- Per-line pan LFOs add 8 sine evaluations per sample
- Wavefolder adds 2 `sin()` calls per sample (feedback path only)
- Net: moderate CPU increase, well within budget for a reverb plugin
- **Pre-implementation audit requested:** full memory/CPU analysis of existing codebase before changes

---

## Performance Audit (Pre-Implementation)

Before implementing v3 changes, conduct a full analysis of the existing codebase for:

1. **Memory usage** — buffer allocations, unnecessary copies, allocation in audio thread
2. **CPU hotspots** — per-sample operations, function call overhead, branch prediction
3. **Optimization opportunities** — SIMD potential, lookup tables, unnecessary calculations
4. **Thread safety** — parameter smoothing, state access patterns

This audit should identify improvements that can be made alongside the v3 changes without impacting sound or design.

---

## Success Criteria

1. Every parameter has a clearly audible effect across its full range
2. At moderate settings (no drive, mid feedback), the effect is noticeably present
3. At extreme settings (high drive, high feedback, filter in feedback), the effect produces wild psychedelic sounds
4. At minimum settings, the effect is subtle but present (not inaudible)
5. The output never digitally clips (tanh limiter)
6. CPU usage remains reasonable for a reverb plugin (~5-10% on a modern CPU at 48kHz)
7. All 18 presets showcase distinct sonic territories
8. Preset morphing works smoothly across the new parameter set
