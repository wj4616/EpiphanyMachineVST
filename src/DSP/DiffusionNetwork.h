#pragma once
#include <JuceHeader.h>

//==============================================================================
// DiffusionNetwork — 4-line Feedback Delay Network (FDN) reverb.
//
// FREEZE mode: stops new input entering the FDN and sets feedback to ~0.999,
// creating an infinite sustain of the current tail. All transitions are
// smoothed with SmoothedValues to avoid clicks.
//==============================================================================
class DiffusionNetwork
{
public:
    void prepare(double sampleRate, int samplesPerBlock);
    void process(juce::AudioBuffer<float>& buffer);  // in-place stereo
    void reset();

    void setDecay(float d)        { decayParam = d; }
    void setDamping(float d)      { dampingParam = d; }
    void setFreezeActive(bool on);

private:
    static constexpr int kNumLines = 4;
    static constexpr int kBufSize  = 2048;
    static constexpr int kBufMask  = kBufSize - 1;
    static constexpr int kBaseLengths[kNumLines] = { 1087, 1283, 1637, 1933 };
    int delayLengths[kNumLines] = {};

    float bufL[kNumLines][kBufSize] = {};
    float bufR[kNumLines][kBufSize] = {};
    int   writeIndex[kNumLines] = {};
    float lpfStateL[kNumLines] = {};
    float lpfStateR[kNumLines] = {};
    float lfoPhase[kNumLines]  = {};
    static constexpr float kLfoRates[kNumLines] = { 0.3f, 0.5f, 0.7f, 0.9f };

    float decayParam   = 0.6f;
    float dampingParam = 0.5f;
    double sampleRate  = 48000.0;

    //==========================================================================
    // Freeze SmoothedValues — ramp over ~42ms (2048 samples at 48kHz) so
    // toggling freeze creates no audible click or pop.
    //==========================================================================
    juce::SmoothedValue<float> freezeInputGain;     // 1.0→0.0 when freezing
    juce::SmoothedValue<float> freezeFeedbackBoost; // 0.0→1.0 when freezing
    juce::SmoothedValue<float> freezeDampingMute;   // 0.0→1.0 when freezing

    bool isFrozen = false;

    void hadamardMix(float v[kNumLines]) const;
};
