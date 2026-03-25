#pragma once
#include <JuceHeader.h>

//==============================================================================
// PitchDrifter — dual-head granular pitch shifter.
//
// Classic trick: two read heads advance at `pitchRatio` speed through a
// circular buffer, offset by half a grain window.  Each head is windowed
// with a Hann function, so when one head is at the centre of a grain the
// other is at the boundary — their windows sum to 1.0 everywhere (no gaps).
//
// Lives ONLY in the feedback path.  When DRIFT=0, pass-through (no shift).
//==============================================================================
class PitchDrifter
{
public:
    void prepare(double sampleRate, int samplesPerBlock);
    void process(juce::AudioBuffer<float>& buffer);  // in-place stereo
    void reset();

    // drift in semitones, range -12..+12
    void setDrift(float semitones) { driftSemitones = semitones; }

private:
    static constexpr int kBufSize   = 4096;
    static constexpr int kBufMask   = kBufSize - 1;
    static constexpr int kGrainSize = 2048;  // ~42ms at 48kHz

    float bufL[kBufSize] = {};
    float bufR[kBufSize] = {};

    int   writeHead = 0;

    // Two read heads — each is a float for sub-sample accuracy
    float readHead0 = 0.0f;
    float readHead1 = 0.0f;

    float driftSemitones = 0.0f;
    double sampleRate = 48000.0;

    // Precomputed Hann window table to avoid per-sample trig
    std::array<float, kGrainSize> hannWindow;

    void buildHannWindow();

    // Read a sample from a circular buffer at a fractional position
    float readInterp(const float* buf, float pos) const;
};
