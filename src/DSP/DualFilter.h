#pragma once
#include <JuceHeader.h>

//==============================================================================
// DualFilter — two Cytomic TPT State Variable Filters.
//
// WHY TPT (Trapezoidal/Bilinear) INSTEAD OF CHAMBERLIN SVF?
//   The naive Chamberlin SVF uses f = 2*sin(π*fc/fs) which becomes UNSTABLE
//   for cutoffs above ~fs/6, and clicks badly when cutoff is modulated fast.
//   The Cytomic TPT SVF uses g = tan(π*fc/fs) (bilinear mapping), which:
//   - Is unconditionally stable at ALL frequencies up to Nyquist
//   - Handles LFO modulation at 10 Hz without any clicks
//   - Has the exact same CPU cost (one tan() instead of one sin())
//   Source: Andy Simper, "Solving the Continuous SVF Equations Using Trapezoidal Integration" (2013)
//
// Each filter processes its own stream (texture or body) and the outputs
// are summed to recreate the full signal.
//==============================================================================
class DualFilter
{
public:
    void prepare(double sampleRate, int samplesPerBlock);
    void process(const juce::AudioBuffer<float>& textureIn,
                 const juce::AudioBuffer<float>& bodyIn,
                 juce::AudioBuffer<float>& output,
                 int numSamples);
    void reset();

    void setTexFilterType(int t)    { texFilterType = t; }   // 0=LP 1=HP 2=BP
    void setTexCutoff(float hz)     { texCutoffSmoothed.setTargetValue(hz); }
    void setTexReso(float r)        { texResoSmoothed.setTargetValue(r); }
    void setTexLfoRate(float hz)    { texLfoRateSmoothed.setTargetValue(hz); }
    void setTexLfoDepth(float d)    { texLfoDepthSmoothed.setTargetValue(d); }

    void setBodyFilterType(int t)   { bodyFilterType = t; }  // 0=LP 1=HP 2=BP
    void setBodyCutoff(float hz)    { bodyCutoffSmoothed.setTargetValue(hz); }
    void setBodyReso(float r)       { bodyResoSmoothed.setTargetValue(r); }

private:
    double sampleRate = 48000.0;

    //==========================================================================
    // Cytomic TPT SVF state — two integrator outputs per channel per filter.
    // ic1eq = "integrator 1 estimate", ic2eq = "integrator 2 estimate".
    // These carry the filter's "memory" between samples.
    //==========================================================================
    struct TPTState { float ic1eq = 0.0f, ic2eq = 0.0f; };
    TPTState texL, texR, bodyL, bodyR;

    int texFilterType  = 0;  // LP
    int bodyFilterType = 0;  // LP

    juce::SmoothedValue<float> texCutoffSmoothed;
    juce::SmoothedValue<float> texResoSmoothed;
    juce::SmoothedValue<float> texLfoRateSmoothed;
    juce::SmoothedValue<float> texLfoDepthSmoothed;
    juce::SmoothedValue<float> bodyCutoffSmoothed;
    juce::SmoothedValue<float> bodyResoSmoothed;

    float texLfoPhase = 0.0f;

    // Process one sample through TPT SVF; returns selected output (LP/HP/BP).
    // Coefficients a1/a2/a3 are precomputed per-sample (or per-block for perf).
    float processTPT(TPTState& s, float input, float a1, float a2, float a3,
                     float k, int type);
};
