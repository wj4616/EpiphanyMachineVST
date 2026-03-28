#pragma once
#include <JuceHeader.h>

//==============================================================================
// DiffusionNetwork — 8-line FDN reverb.
// v3: 8 lines, 8x8 FHT, RT60 decay, per-line pan LFOs, setDecaySeconds API.
//==============================================================================
class DiffusionNetwork
{
public:
    void prepare(double sampleRate, int samplesPerBlock);
    void process(juce::AudioBuffer<float>& buffer);  // in-place stereo
    void reset();

    void setDecaySeconds(float s)  { decaySeconds = std::max(0.01f, s); }
    void setDamping(float d)       { dampingParam = d; }
    void setWidth(float w)         { widthParam = juce::jlimit(0.0f, 1.0f, w); }
    void setFreezeActive(bool on);

private:
    static constexpr int kNumLines = 8;
    static constexpr int kBufSize  = 16384;
    static constexpr int kBufMask  = kBufSize - 1;
    static constexpr int kBaseLengths[kNumLines] = {1087,1283,1637,1933,2311,2719,3109,3511};
    static constexpr float kLfoRates[kNumLines]  = {0.07f,0.11f,0.15f,0.19f,0.23f,0.29f,0.31f,0.37f};
    static constexpr float kBasePan[kNumLines]   = {-0.8f,0.8f,-0.4f,0.4f,-0.6f,0.6f,-0.2f,0.2f};

    int delayLengths[kNumLines] = {};
    float bufL[kNumLines][kBufSize] = {};
    float bufR[kNumLines][kBufSize] = {};
    int   writeIndex[kNumLines] = {};
    float lpfStateL[kNumLines]  = {};
    float lpfStateR[kNumLines]  = {};
    float lfoPhase[kNumLines]   = {};

    float decaySeconds = 6.0f;
    float dampingParam = 0.5f;
    float widthParam   = 0.6f;
    double sampleRate  = 48000.0;

    juce::SmoothedValue<float> freezeInputGain;
    juce::SmoothedValue<float> freezeFeedbackBoost;
    juce::SmoothedValue<float> freezeDampingMute;
    void hadamardMix(float v[kNumLines]) const;
};
