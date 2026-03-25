#pragma once
#include <JuceHeader.h>

//==============================================================================
// StereoWidener — mid-side width control + slow spatial LFO.
//
// Mid-side encoding separates the "shared" content (mid) from the
// "difference" content (side).  Scaling the side channel widens or
// narrows the stereo image without changing the mono sum.
//==============================================================================
class StereoWidener
{
public:
    void prepare(double sampleRate, int samplesPerBlock);
    void process(juce::AudioBuffer<float>& buffer);  // in-place stereo
    void reset();

    // width: 0=mono, 0.5=natural, 1.0=extra wide
    void setWidth(float w) { widthParam = w; }

private:
    float widthParam  = 0.6f;
    float lfoPhase    = 0.0f;   // phase of the spatial LFO (0..1)
    float lfoPhaseInc = 0.0f;   // advance per sample (set in prepare)
    double sampleRate = 48000.0;
};
