#pragma once
#include <JuceHeader.h>

//==============================================================================
// GlitchEngine — circular-buffer stutter/repeat effect.
//
// Internally captures incoming audio into a ring buffer and replays
// "grains" (short frozen segments) at rates and positions controlled
// by DENSITY and CHAOS.
//==============================================================================
class GlitchEngine
{
public:
    GlitchEngine();

    void prepare(double sampleRate, int samplesPerBlock);
    void process(juce::AudioBuffer<float>& buffer);  // in-place stereo
    void reset();

    void setDensity(float d) { density = d; }
    void setChaos(float c)   { chaos = c; }

private:
    //==========================================================================
    // Circular buffer — power-of-2 size so we can wrap with a bitmask.
    // Storing two channels (L/R) so grains stay phase-coherent in stereo.
    //==========================================================================
    static constexpr int kBufSize = 131072;       // 2^17, ~2.7s at 48kHz
    static constexpr int kBufMask = kBufSize - 1; // bitmask for fast wrap
    static constexpr int kCrossfade = 256;         // ~5ms at 48kHz

    std::array<float, kBufSize> bufL, bufR;

    int   writeHead  = 0;   // always advances 1 per sample
    int   playHead   = 0;   // advances at speed determined by current grain
    int   grainSamplesLeft = 0;  // how many samples remain in current grain
    int   grainStart = 0;        // buffer position where current grain began
    int   grainLength = 0;
    bool  grainReverse = false;
    float grainSpeed = 1.0f;   // 0.5 = half speed, 1.0 = normal
    int   crossfadeCounter = 0; // counts from kCrossfade down to 0 at boundaries

    // Previous grain end samples for crossfade blending
    float prevSampleL = 0.0f;
    float prevSampleR = 0.0f;

    float density = 0.3f;
    float chaos   = 0.2f;
    double sampleRate = 48000.0;

    juce::Random rng;  // single instance, not per-sample

    void triggerNewGrain();
    float getSample(const std::array<float, kBufSize>& buf,
                    int position, bool reverse, float speed,
                    int sampleIndex) const;
};
