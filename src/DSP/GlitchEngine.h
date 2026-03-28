#pragma once
#include <JuceHeader.h>

class GlitchEngine
{
public:
    GlitchEngine();
    void prepare(double sampleRate, int samplesPerBlock);
    void process(juce::AudioBuffer<float>& buffer);
    void reset();

    void setDensity(float d) { density = d; }
    void setChaos(float c)   { chaos = c; }
    void setDrift(float d)   { drift = juce::jlimit(0.0f, 24.0f, d); }

private:
    static constexpr int kBufSize = 131072;
    static constexpr int kBufMask = kBufSize - 1;

    std::array<float, kBufSize> bufL, bufR;

    int   writeHead        = 0;
    int   grainSamplesLeft = 0;
    int   grainStart       = 0;
    int   grainLength      = 0;
    float grainLengthMs    = 100.0f;
    bool  grainReverse     = false;
    float grainSpeed       = 1.0f;

    float  density    = 0.3f;
    float  chaos      = 0.2f;
    float  drift      = 0.0f;
    double sampleRate = 48000.0;

    juce::Random rng;

    void  triggerNewGrain();
    float getWindowValue(int samplePos, int grainLenSamples, float grainMs) const;
};
