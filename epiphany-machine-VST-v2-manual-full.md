# EPIPHANY MACHINE
## User Manual & Sound Design Guide — v2.0

### [Support EpiphanyMachineArt.com](https://epiphanymachineart.com)

Epiphany Machine transforms synthesizer signals into deep, evolving stereo soundscapes. It blends lush ambient diffusion with rhythmic glitch artifacts, splits your sound into its moving and stable components for independent filtering, and freezes reverb tails into infinite ambient beds. 33 factory presets span the full spectrum from motionless dark voids to explosive bright chaos. Designed for live ambient performance.

---

## Contents

1. [Quick Start](#1-quick-start)
2. [How the Signal Flows](#2-how-the-signal-flows)
3. [The Controls](#3-the-controls)
4. [Freeze](#4-freeze)
5. [The Dual Filter](#5-the-dual-filter)
6. [Factory Presets — Complete Guide](#6-factory-presets--complete-guide)
7. [Live Performance Guide](#7-live-performance-guide)
8. [Sound Design Techniques](#8-sound-design-techniques)
9. [Parameter Reference](#9-parameter-reference)

---

## 1. Quick Start

Insert Epiphany Machine on a track receiving your synthesizer audio. Accepts mono or stereo input, always produces stereo output.

1. Check **BYPASS** (top-right) is not engaged.
2. Play a sustained pad or drone.
3. Turn **MIX** to 50%. Hear ambient reverb blend with your signal.
4. Slowly raise **MORPH**. Glitch artifacts appear.
5. Cycle presets with **[>]** arrow. Transitions are smooth, controlled by **MORPH TIME**.
6. Hit **FREEZE** to capture the reverb tail forever. Play over it. Hit FREEZE again to release.

> **TIP:** Start with "Phantom Drift" and a simple sustained chord.

---

## 2. How the Signal Flows

The GUI is laid out left-to-right following the signal path.

**Stage 1: Detect + Split** — SENSITIVITY controls texture/body separation. GLITCH FOCUS chooses what feeds the glitch engine.

**Stage 2: Glitch Engine** — MORPH blends clean/glitched input. DENSITY sets fragment rate. CHAOS sets randomness. Fragments feed INTO the reverb.

**Stage 3: Diffusion + Feedback** — DECAY, DAMPING, WIDTH shape the reverb. DRIFT pitch-shifts each feedback pass. FEEDBACK controls recirculation.

**Stage 4: Dual Filter** — When active, processes texture and body independently.

**Stage 5: Output** — MIX blends wet and dry.

---

## 3. The Controls

- **SENSITIVITY** — Detection aggressiveness. Low = dramatic changes only. High = catches subtle vibrato.
- **GLITCH FOCUS** — 0 = body glitched, texture clean. 0.5 = full signal. 1.0 = texture glitched, body clean.
- **MORPH** — Central control. 0 = pure ambient. 1.0 = full glitch.
- **DENSITY** — Glitch fragment rate and size.
- **CHAOS** — Unpredictability. 0 = predictable. 1.0 = wild.
- **DECAY** — Reverb tail length.
- **DAMPING** — High-frequency absorption. 0 = bright. 0.8+ = dark.
- **WIDTH** — Stereo spread.
- **DRIFT** — Pitch shift per feedback pass. +12 = shimmer. -12 = descending. +7 = fifth.
- **FEEDBACK** — Recirculation. Higher = accumulating textures.
- **MIX** — Dry/wet blend.

---

## 4. Freeze

Captures the reverb tail and sustains it indefinitely. DRIFT and FEEDBACK still affect the frozen tail.

1. Play a drone, let reverb build.
2. Hit **FREEZE**. Tail sustains forever.
3. Change synth patch. Play over frozen bed.
4. Hit **FREEZE** again. Old tail fades, new input enters.

> **TIP:** Freeze a tail, morph to a different preset, unfreeze at the peak for dramatic transition.

---

## 5. The Dual Filter

Splits audio into texture (moving) and body (stable), filtering each independently.

- **OFF** = bypassed
- **WET** = filters only the effect output (most useful)
- **FINAL** = filters everything including dry signal

**Texture Filter:** TYPE, CUTOFF, RESO, LFO RATE, LFO DEPTH
**Body Filter:** TYPE, CUTOFF, RESO (no LFO)

> **TIP:** Classic combo: Texture BP at 2-4 kHz with slow LFO. Body LP at 800-1200 Hz.

---

## 6. Factory Presets — Complete Guide

34 presets total: INIT + 33 factory presets in 6 categories. Use [<] [>] to cycle. MORPH TIME controls transition smoothness. Grab any knob during a transition to override it.

### INIT
*The blank canvas.* Default settings. MORPH 0.3 | MIX 0.5 | DECAY 0.6 | Filter OFF. Start here and sculpt.

---

### Deep Ambient
*Pure reverb spaces for vast, motionless atmospheres. No or minimal glitch.*

#### Void Meditation
*Motionless dark abyss. An infinite void you float in.*

**Key settings:** MORPH 0 | DECAY 0.92 | DAMPING 0.8 | Filter WET: both LP, dark

No glitch artifacts. Near-infinite reverb tail with heavy damping removes all brightness. Both filters set very low to darken further. The glacial texture LFO (one cycle every 20 seconds) adds barely perceptible drift.

**Best with:** Simple, sustained chords. Minimal movement. Let the space do the work.

> **Performance tip:** Use FREEZE to capture a perfect moment, then stop playing and just listen to the void.

#### Eternal Hall
*Bright, infinite concert hall. Pristine and open.*

**Key settings:** MORPH 0 | DECAY 0.95 | DAMPING 0.2 | Filter OFF | WIDTH 0.8

The brightest ambient preset. Near-zero damping preserves all high-frequency detail in the tail. No filtering applied. Maximum decay creates truly infinite-feeling sustain.

**Best with:** Any source material. Especially effective with plucked or bell-like tones.

> **Performance tip:** Slowly introduce MORPH (to 0.1-0.2) to add faint glitch fragments into the pristine space.

#### Cathedral of Dust
*Ancient stone. Muffled reflections in a vast, crumbling space.*

**Key settings:** MORPH 0.05 | DECAY 0.9 | DAMPING 0.9 | Filter FINAL: both LP very low

Extreme damping plus FINAL-mode low-pass filtering on both streams creates a massively dark, muffled sound. The tiniest hint of glitch (MORPH 0.05) adds subtle imperfection — dust falling from ancient walls.

**Best with:** Low-register drones. Bass pads. Anything with fundamental weight below 300 Hz.

#### Weightless
*Floating in zero gravity. Airy, wide, and spacious.*

**Key settings:** MORPH 0 | DECAY 0.85 | WIDTH 1.0 | Filter WET: Tex HP, Body LP

Maximum stereo width with the texture HP filter removing low-end from evolving elements while the body LP keeps the foundation full. The separation creates a sense of floating — bass anchors you while highs drift freely across the stereo field.

**Best with:** Mid-range pads. Sounds with both low-end body and high-frequency shimmer.

> **Performance tip:** Gradually narrow WIDTH from 1.0 to 0.3 during a piece to create a sense of closing in, then snap it back open.

#### Frozen Lake
*Perfectly still. Cold, crystalline, minimal.*

**Key settings:** MORPH 0 | DECAY 0.8 | DAMPING 0.5 | Filter WET: moderate LP on both

The most minimal preset. Balanced damping, moderate decay, no glitch, no drift. A clean, still, reflective space. The filters gently soften without making things dark.

**Best with:** Sparse, delicate playing. Single notes. Silence between events.

> **Performance tip:** This preset rewards patience. Play less. The space between notes is the music.


---

### Shimmer
*Pitch drift creates evolving harmonics. From classic shimmer to dark descents and dissonant clouds.*

#### Phantom Drift
*Classic ascending shimmer. Octave-up crystalline harmonics.*

**Key settings:** DRIFT +12 | FEEDBACK 0.6 | DAMPING 0.15 | Filter WET: Tex HP, Body LP

The definitive shimmer sound. Each feedback pass adds an octave, building cascading harmonics. Low damping keeps everything bright and sparkling. HP on texture lets the shimmer harmonics ring while LP on body keeps the foundation warm.

**Best with:** Simple pads, single notes, slow arpeggios. The simpler the input, the more complex the output.

> **Performance tip:** Feed a single sustained note and listen to the harmonics build for 30+ seconds.

#### Descending Angel
*Downward shimmer. Sounds sink into deepening darkness.*

**Key settings:** DRIFT -12 | FEEDBACK 0.55 | DAMPING 0.6 | Filter WET: both LP

The inverse of Phantom Drift. Each feedback pass drops an octave, creating a deepening, darkening tail. High damping accelerates the descent into murk. Both LP filters add warmth to the sinking harmonics.

**Best with:** Mid-to-high register pads. The descent is more dramatic when the source starts bright.

> **Performance tip:** Play a bright chord, let it descend for 20 seconds, then play the same chord an octave lower. The layers merge beautifully.

#### Fifth Dimension
*Consonant fifth (+7) stacking. Churchy, organ-like, expansive.*

**Key settings:** DRIFT +7 | FEEDBACK 0.55 | DAMPING 0.3

A perfect fifth stacks musically — each pass sounds like adding another organ stop. The result is rich and consonant rather than chaotic. Moderate damping lets the harmonics ring but not indefinitely.

**Best with:** Sustained fifths or octaves. Simple intervals. Complex chords can get muddy.

> **Performance tip:** Play a root note and let the fifths build, then add the actual fifth on your synth to reinforce the stacking.

#### Dissonant Cloud
*Minor third (+3) stacking. Unsettling harmonic tension.*

**Key settings:** DRIFT +3 | FEEDBACK 0.5 | DAMPING 0.4 | Filter WET: Tex BP with resonance

Minor third intervals stack into clusters that become increasingly dissonant and chromatic. The BP texture filter with resonance emphasizes the clash frequencies. Creates a beautiful unease.

**Best with:** Single notes or simple intervals. The dissonance builds from simplicity.

> **Performance tip:** Start with a single note, let the minor thirds accumulate, then slowly raise DAMPING to darken the cloud.

#### Tritone Abyss
*The devil's interval (+6). Maximum harmonic tension.*

**Key settings:** DRIFT +6 | FEEDBACK 0.5 | DAMPING 0.65 | Filter WET: Tex LP, Body LP dark

The tritone is the most dissonant interval in Western music. Stacking it creates haunting, unstable harmonic fields. Heavy damping and dark filtering keep it from becoming harsh — instead it becomes ominous and deep.

**Best with:** Low-register single notes. The tritone stacking is most effective with simple, dark sources.

> **Performance tip:** Use with FREEZE: let the tritone cloud build, freeze it, then play consonant chords on top for contrast.

#### Solar Flare
*Explosive bright harmonics sprayed across the stereo field.*

**Key settings:** DRIFT +12 | FEEDBACK 0.7 | DAMPING 0.05 | WIDTH 1.0 | Filter WET: both HP

Maximum brightness with maximum feedback and drift. Both filters in HP mode strips away all low end — pure sparkle and air. Full stereo width with high sensitivity texture-focused glitching creates bright stuttering fragments.

**Best with:** Mid/high-register patches. Strings, bells, glass timbres. Anything that shimmers naturally.

> **Performance tip:** The opposite of Deep Submersion. Try morphing between the two over 10 seconds.


---

### Glitch Ambient
*Rhythmic stuttering fragments dissolved into ambient space. From chaos to meditative repetition.*

#### Neural Storm
*Maximum chaos. Rapid stuttering in a dense, pulsing field.*

**Key settings:** MORPH 0.85 | DENSITY 0.8 | CHAOS 0.75 | GLITCH FOCUS 1.0 | Filter WET: Tex BP fast LFO

The most intense preset. Only timbral movement gets glitched (FOCUS 1.0) at maximum density and chaos. The BP texture filter with 2.5 Hz LFO creates a wah-like quality on the fragments. +5 semitone drift adds dissonant stacking.

**Best with:** Anything with internal modulation — filter sweeps, LFO movement, vibrato. The more movement, the more material the glitch engine has.

> **Performance tip:** Start from INIT and morph to Neural Storm over 8 seconds for a dramatic build.

#### Broken Transmission
*Radio static fragments dissolving into warm reverb.*

**Key settings:** MORPH 0.7 | DENSITY 0.65 | CHAOS 0.6 | GLITCH FOCUS 1.0 | Filter WET: Tex BP, Body LP

Like intercepting an alien signal through heavy static. The texture-focused glitching captures timbral movement, the BP filter gives fragments a vocal, radio-like quality. Moderate decay lets fragments dissolve relatively quickly.

**Best with:** Modulated pads. Patches with chorus, phaser, or filter movement.

> **Performance tip:** Sweep your synth's filter cutoff slowly — each sweep gets captured and broken apart.

#### Scattered Memories
*Sparse glitch fragments floating in a warm ambient wash.*

**Key settings:** MORPH 0.5 | DENSITY 0.35 | CHAOS 0.45 | DECAY 0.7 | Filter WET: both LP warm

The gentle side of glitch. Sparse fragments appear occasionally and dissolve into a warm, damped reverb. No pitch drift — pure texture without harmonic evolution. A nostalgic, hazy feeling.

**Best with:** Any sustained sound. Especially effective with sounds that have natural timbral variation.

> **Performance tip:** This preset pairs beautifully with FREEZE. Capture a warm wash, then play sparse notes on top.

#### Stutter Prayer
*Rhythmic, meditative repetition. Predictable and hypnotic.*

**Key settings:** MORPH 0.6 | DENSITY 0.7 | CHAOS 0.05 | DRIFT 0 | Filter WET: both LP moderate

High density with near-zero chaos creates a locked, rhythmic stutter pattern. No randomness — fragments repeat predictably, creating a meditative pulse. No pitch drift keeps things grounded.

**Best with:** Sustained pads with minimal modulation. The stutter pattern IS the modulation.

> **Performance tip:** Try this with a perfectly static drone — the glitch engine creates all the rhythm from the steady input.

#### Insect Choir
*Tiny, rapid, high-pitched glitch fragments. Alien and alive.*

**Key settings:** MORPH 0.75 | DENSITY 0.9 | CHAOS 0.8 | DRIFT +12 | GLITCH FOCUS 1.0 | Filter WET: Tex HP fast LFO, Body HP

Maximum density creates micro-fragments. Both filters in HP mode removes all warmth — pure high-frequency chirping. The fast LFO (5 Hz) on texture creates a trilling quality. Octave-up drift adds ascending energy. Sounds like a colony of alien insects.

**Best with:** Any source. The extreme processing transforms everything into the same alien character.

> **Performance tip:** Layer this with Void Meditation on a parallel track for an incredible contrast.

#### Living Texture
*Balanced organic pad with rhythmic glitch breathing.*

**Key settings:** MORPH 0.4 | DENSITY 0.5 | CHAOS 0.4 | DRIFT +7 | GLITCH FOCUS 1.0 | Filter WET: Tex BP slow LFO, Body LP

The sweet spot for live performance. Moderate settings across the board with texture-focused glitching. As you play and modulate your synth, evolving elements become rhythmic patterns floating above a stable drone. The slow texture LFO adds gradual tonal drift to fragments.

**Best with:** Evolving pads with moderate internal modulation. Patches where you actively sweep filters or modulate parameters while playing.

> **Performance tip:** This is the preset to learn the plugin with. Map MORPH to a physical knob and explore.


---

### Deep and Dark
*Subterranean, underwater, and nocturnal. Heavy filtering, descending pitch, and maximum depth.*

#### Deep Submersion
*Underwater cathedral. Everything submerged and muffled.*

**Key settings:** DRIFT -5 | DAMPING 0.85 | MIX 0.75 | Filter FINAL: both LP very low

Negative drift makes the feedback descend. Very high damping removes brightness. Both filters extremely low in FINAL mode — everything sounds like it is heard through deep water. The ultra-slow texture LFO (one cycle every 33 seconds) creates barely perceptible movement.

**Best with:** Low-register drones, bass pads. The descending pitch creates a sinking sensation.

> **Performance tip:** Try morphing from Solar Flare to Deep Submersion over 10 seconds — bright to dark in one gesture.

#### Tectonic Drone
*Sub-bass rumble. The earth moving slowly beneath you.*

**Key settings:** DRIFT -7 | DAMPING 0.95 | GLITCH FOCUS 0.0 | Filter FINAL: both LP extremely low

The darkest preset. Near-maximum damping with both filters below 600 Hz in FINAL mode. The descending fifth drift (-7) creates a geological, grinding quality. GLITCH FOCUS at 0.0 means the stable body gets glitched — fracturing the foundation itself.

**Best with:** Low bass drones. Sub-bass patches. Anything below 200 Hz.

> **Performance tip:** Pair with a sub-bass synth patch and turn your monitors up. You should feel this more than hear it.

#### Obsidian Mirror
*Dark, reflective, perfectly still. Glass-like low end.*

**Key settings:** MORPH 0 | DECAY 0.88 | DAMPING 0.75 | DRIFT 0 | Filter FINAL: both LP moderate-low

No glitch, no drift — pure dark reverb. The high decay creates a long, resonant tail but the heavy damping and LP filtering keep it all below the mid frequencies. A deep, reflective stillness.

**Best with:** Sustained low-mid pads. Sounds with natural warmth.

> **Performance tip:** Add the tiniest amount of MORPH (0.05) and listen for subtle imperfections appearing in the glass.

#### Gravity Well
*Everything pulled downward. Descending, compressing, heavy.*

**Key settings:** DRIFT -12 | FEEDBACK 0.55 | DAMPING 0.8 | MORPH 0.3 | Filter FINAL: both LP low

Full octave-down drift with high feedback creates a relentless downward pull. Light glitch (MORPH 0.3) adds fractured weight. Everything sinks — notes, harmonics, texture. The most oppressive preset.

**Best with:** Mid-range input. The descending effect is most dramatic when sounds have somewhere to fall.

> **Performance tip:** Play ascending arpeggios while the plugin drags everything down — the tension between your playing and the effect is compelling.

#### Midnight Fog
*Dense, obscuring, warm. Low visibility ambient.*

**Key settings:** MORPH 0.1 | DECAY 0.85 | DAMPING 0.7 | Filter WET: both LP warm

Warm and enveloping rather than cold and dark. Moderate damping preserves some mid-range presence while removing harshness. Slight glitch adds subtle movement in the fog. The gentlest of the dark presets.

**Best with:** Any mid-range pad. Warm analog tones work especially well.

> **Performance tip:** A good 'default dark' preset when you want depth without extremity.


---

### Crystalline and Metallic
*Resonant filters, sharp transients, and geometric precision. The filter becomes the instrument.*

#### Crystal Lattice
*Precise micro-glitches in geometric, resonant patterns.*

**Key settings:** DRIFT +7 | GLITCH FOCUS 0.0 | CHAOS 0.1 | Filter FINAL: both BP resonant

The body gets glitched (FOCUS 0.0) with low chaos — rhythmic, predictable fracturing of the foundation. Both filters in BP mode creates two resonant peaks at 800 Hz and 3500 Hz — a hollowed, crystalline quality. The consonant fifth drift adds musical harmonic stacking.

**Best with:** Rich, harmonically complex pads. The dual bandpass reveals hidden overtones.

> **Performance tip:** Try sweeping both filter cutoffs slowly in opposite directions.

#### Bell Garden
*Resonant pings ringing in a reverberant space.*

**Key settings:** MORPH 0.35 | GLITCH FOCUS 1.0 | Filter WET: Tex BP high resonance, Body LP open

The texture filter at high resonance (0.65) on BP mode turns evolving elements into ringing, bell-like tones. The body LP stays open to preserve the full foundation. Octave-up drift adds ascending harmonics to the bell tones.

**Best with:** Sounds with distinct textural variation — plucked-style pads, sounds with obvious attack/sustain difference.

> **Performance tip:** Play staccato chords and listen to each attack ring as a bell through the resonant filter.

#### Ice Fracture
*Sharp, cracking, brittle. High-frequency crystalline splinters.*

**Key settings:** DENSITY 0.7 | CHAOS 0.5 | DAMPING 0.05 | GLITCH FOCUS 1.0 | Filter WET: Tex HP fast LFO, Body HP

High density glitching with zero damping preserves all the high-frequency sharpness. Both HP filters remove warmth entirely. The fast 3 Hz LFO on texture creates a rapid twinkling effect on the splinter fragments. Bright and aggressive.

**Best with:** Bright, high-register patches. Anything with high-frequency content.

> **Performance tip:** Contrast with Midnight Fog — alternate between the two for maximum dynamic range.

#### Singing Metal
*The filter IS the instrument. Resonant metallic drones.*

**Key settings:** MORPH 0.2 | DRIFT 0 | Filter WET: both BP high resonance (0.6-0.7), slow LFO on Tex

Both filters set to BP with very high resonance turn them into resonant oscillators excited by your input signal. The slow texture LFO drifts the resonant frequency, creating a singing, metallic quality. The input feeds the resonance — the output sounds more like the filters than the source.

**Best with:** Noisy, harmonically rich textures. White noise patches. Anything broadband.

> **Performance tip:** Try feeding actual noise into this preset — the filters will sing pure tones from chaos.

#### Prism
*Light split into spectral colors. Bright, separated, analytical.*

**Key settings:** DRIFT +7 | WIDTH 0.9 | Filter WET: Tex HP, Body BP resonant

The HP texture filter separates high-frequency shimmer while the BP body filter isolates and emphasizes a specific mid-frequency band. Wide stereo creates spatial separation between the spectral components. The fifth drift adds consonant harmonic structure.

**Best with:** Pads with broad frequency content. Rich, layered timbres.

> **Performance tip:** Slowly sweep the Body filter cutoff to 'tune' which frequency the prism focuses on.


---

### Experimental and Extreme
*Unusual parameter combinations and sound design tools. Starting points for exploration.*

#### Event Horizon
*Maximum feedback and drift. Sounds stretch to infinity.*

**Key settings:** FEEDBACK 0.85 | DRIFT +12 | DECAY 0.9 | MORPH 0.5 | CHAOS 0.5

Feedback at the absolute maximum with octave-up drift and high decay. Sounds that enter never fully leave — they accumulate, pitch-shift, and layer endlessly. The effect builds continuously. Use MIX to control how much of the vortex you hear.

**Best with:** Single notes, one at a time. Give each note space to accumulate.

> **Performance tip:** Play one note. Wait 30 seconds. Play another. Each one adds to the growing harmonic field.

#### Ghost in the Machine
*Fractured foundation, floating clarity above.*

**Key settings:** MORPH 0.65 | GLITCH FOCUS 0.0 | DRIFT +3 | Filter WET: Tex LP open, Body BP resonant

GLITCH FOCUS at 0.0 sends the stable body to the glitch engine while texture floats free. The body BP filter adds resonant character to the fractured foundation. A strange inversion — the stable parts get destroyed, the moving parts remain clear.

**Best with:** Sounds with both steady and evolving components. Pads with internal movement.

> **Performance tip:** Play a steady drone and listen to the foundation fracture beneath the clean texture.

#### Parallel Universe
*Texture and body diverge into completely separate worlds.*

**Key settings:** MORPH 0.5 | GLITCH FOCUS 1.0 | DRIFT +5 | Filter FINAL: Tex HP fast LFO, Body LP dark

The most extreme texture/body separation. FINAL mode filters apply to the combined output: the HP texture filter removes all warmth from evolving elements while the LP body filter darkens the foundation. Fast texture LFO adds rhythmic movement to only the bright elements. Two realities coexisting.

**Best with:** Rich, complex pads. Sounds where texture and body are naturally distinct.

> **Performance tip:** Automate GLITCH FOCUS slowly from 0.0 to 1.0 over a minute to shift which 'universe' the glitch engine disrupts.

#### Temporal Collapse
*Everything at once. Dense, chaotic, overwhelming.*

**Key settings:** MORPH 0.9 | DENSITY 0.85 | CHAOS 0.9 | DRIFT +6 | FEEDBACK 0.65 | WIDTH 1.0 | Filter WET: Tex BP fast LFO

Nearly every parameter at maximum. Extreme glitch density, maximum chaos, tritone drift, high feedback, full width. The fast 4 Hz LFO on the BP texture filter adds pulsing rhythm to the mayhem. Not subtle. Not for the faint-hearted.

**Best with:** Anything. The extreme processing transforms everything into beautiful chaos.

> **Performance tip:** Use as a build-up destination: morph from Frozen Lake to Temporal Collapse over 10 seconds.

#### The Long Exhale
*Ultra-slow everything. Glacial patience rewarded.*

**Key settings:** MORPH 0.1 | DECAY 0.95 | MIX 0.8 | Tex LFO 0.01 Hz | Filter WET: both LP moderate

The slowest preset. Texture LFO at 0.01 Hz takes 100 seconds per cycle. Near-maximum decay means sounds sustain for minutes. Minimal glitch adds occasional subtle fragments. This preset is about deep listening — changes happen so slowly you almost imagine them.

**Best with:** Long, sustained tones. Single notes held for 30+ seconds.

> **Performance tip:** Set a timer for 5 minutes. Play one chord. Close your eyes. Listen to the microscopic evolution.

#### Machine Awakening
*Starts clean. Designed to be evolved during performance.*

**Key settings:** MORPH 0 | CHAOS 0 | DENSITY 0.5 | DRIFT +7 | GLITCH FOCUS 1.0 | Filter WET: Tex BP, Body LP

A performance preset. Starts with zero glitch and zero chaos — a clean canvas. But DENSITY is at 0.5, DRIFT at +7, and the filters are set. As you raise MORPH and CHAOS during your performance, the Machine 'awakens' — glitch patterns emerge, harmonics build, the texture filter starts shaping fragments.

**Best with:** Any source. This preset is about YOUR performance gestures, not the input sound.

> **Performance tip:** Map MORPH and CHAOS to two physical knobs. Start at zero. Over the course of a 10-minute piece, slowly bring the Machine to life.

---

## 7. Live Performance Guide

### The three knobs that matter most

- **MORPH** — Your primary tool. Sweep slowly over minutes.
- **MIX** — Your safety valve. Pull down when it gets intense.
- **GLITCH FOCUS** — Your secret weapon. Changes character without changing intensity.

### Preset morphing as performance

Set MORPH TIME to 6-10 seconds. Cycle presets during a piece. Every parameter drifts smoothly.

### Suggested performance journeys

- **The Awakening:** Machine Awakening > Living Texture > Neural Storm > Temporal Collapse
- **The Descent:** Eternal Hall > Phantom Drift > Frozen Lake > Deep Submersion > Gravity Well
- **Light to Dark:** Solar Flare > Prism > Scattered Memories > Midnight Fog > Void Meditation
- **Tension Arc:** The Long Exhale > Dissonant Cloud > Tritone Abyss > Event Horizon > Frozen Lake

> **TIP:** Map MORPH to an expression pedal. Both hands free for your synth, foot controls effect intensity.

---

## 8. Sound Design Techniques

### Modulation as material
With GLITCH FOCUS at 1.0, your synth's modulation becomes raw material. Each filter sweep, vibrato, or waveform morph gets captured, fragmented, and dissolved into reverb.

### Dual filter as frequency sculptor
Texture HP at 2-3 kHz + Body LP at 600-800 Hz creates a scooped middle. Highs shimmer, lows rumble, with space between.

### Resonance as voice
Texture BP with high resonance (0.5-0.7) and slow LFO. The filter sings at the cutoff frequency. Input excites the resonance but the output sounds more like the filter.

### Freeze layering
Build a tail, freeze, play new material on top. Unfreeze briefly, freeze again. Each freeze captures the accumulated state.

### Chaos evolution
Start CHAOS at 0, DENSITY at 0.5. Over minutes, slowly increase CHAOS. The slow build is far more musical than instant chaos.

### Preset chaining
Plan 3-4 presets that tell a sonic story. Set MORPH TIME to 8 seconds. Cycle through them, letting each transition breathe.

---

## 9. Parameter Reference

### Detect + Split
| Parameter | Range | Default | Description |
|-----------|-------|---------|-------------|
| BYPASS | On/Off | Off | Master bypass |
| FREEZE | On/Off | Off | Freeze reverb tail |
| SENSITIVITY | 0.0-1.0 | 0.5 | Detection aggressiveness |
| GLITCH FOCUS | 0.0-1.0 | 0.5 | Body(0) / Texture(1) to glitch |

### Glitch Engine
| Parameter | Range | Default | Description |
|-----------|-------|---------|-------------|
| MORPH | 0.0-1.0 | 0.3 | Ambient to glitch blend |
| DENSITY | 0.0-1.0 | 0.3 | Fragment rate/size |
| CHAOS | 0.0-1.0 | 0.2 | Randomization |

### Diffusion + Feedback
| Parameter | Range | Default | Description |
|-----------|-------|---------|-------------|
| DECAY | 0.0-1.0 | 0.6 | Tail length |
| DAMPING | 0.0-1.0 | 0.5 | HF absorption |
| WIDTH | 0.0-1.0 | 0.6 | Stereo spread |
| DRIFT | -12 to +12 | 0 | Pitch shift (semitones) |
| FEEDBACK | 0.0-0.85 | 0.3 | Recirculation |

### Dual Filter — Texture
| Parameter | Range | Default | Description |
|-----------|-------|---------|-------------|
| FILTER POS | Off/Wet/Final | Off | Filter placement |
| TYPE | LP/HP/BP | LP | Filter mode |
| CUTOFF | 20-20k Hz | 2000 | Cutoff frequency |
| RESO | 0.0-1.0 | 0.3 | Resonance |
| LFO RATE | 0.01-10 Hz | 0.5 | Modulation speed |
| LFO DEPTH | 0.0-1.0 | 0.0 | Modulation amount |

### Dual Filter — Body
| Parameter | Range | Default | Description |
|-----------|-------|---------|-------------|
| TYPE | LP/HP/BP | LP | Filter mode |
| CUTOFF | 20-20k Hz | 1200 | Cutoff frequency |
| RESO | 0.0-1.0 | 0.2 | Resonance |

### Presets + Output
| Parameter | Range | Default | Description |
|-----------|-------|---------|-------------|
| PRESET | 34 choices | INIT | Preset selector |
| MORPH TIME | 0.1-10 sec | 2.0 | Transition duration |
| MIX | 0.0-1.0 | 0.5 | Dry/wet blend |

---

### [Support EpiphanyMachineArt.com](https://epiphanymachineart.com)

Epiphany Machine v2.0 — Play slowly. Listen deeply. Let the machine surprise you.
