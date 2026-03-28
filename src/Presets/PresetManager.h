#pragma once
#include <JuceHeader.h>
#include "PresetData.h"
#include <set>
#include <atomic>
#include <vector>
#include <utility>

//==============================================================================
// PresetManager — handles preset selection and smooth parameter morphing.
//
// Lives as a member of PluginProcessor. The processor calls advance() once
// per processBlock to step the morph forward.
//
// USER OVERRIDE DETECTION:
//   We register as a listener on all float parameters. When the user drags
//   a knob while a morph is in progress, JUCE calls parameterGestureChanged()
//   with gestureIsStarting=true. We add that parameter to excludedParams so
//   it won't be overwritten by the morph for the rest of the morph duration.
//==============================================================================
class PresetManager : public juce::AudioProcessorParameter::Listener
{
public:
    PresetManager();
    ~PresetManager() override;

    // Call once after apvts is constructed
    void initialise(juce::AudioProcessorValueTreeState& apvts);

    // Call at the start of each processBlock to advance and apply the morph
    void advance(double sampleRate, int numSamples);

    // Trigger a preset change (called when the "preset" APVTS parameter changes)
    void selectPreset(int index, float morphTimeSec);

    // JUCE parameter listener callbacks
    void parameterValueChanged(int, float) override {}   // not needed
    void parameterGestureChanged(int parameterIndex, bool gestureIsStarting) override;

private:
    juce::AudioProcessorValueTreeState* apvts = nullptr;

    bool  isMorphing     = false;
    float morphProgress  = 0.0f;   // 0.0 → 1.0
    float morphTimeSec   = 2.0f;

    PresetData startValues;   // values at morph start (captured from current params)
    PresetData targetValues;  // destination preset

    // Parameters that the user touched during this morph — excluded from interpolation
    std::set<int> excludedParams;

    // Flag to suppress listener during programmatic writes
    std::atomic<bool> systemChanging{false};
    std::vector<std::pair<std::string, int>> paramIndexCache;

    // Capture current APVTS float values into a PresetData struct
    PresetData captureCurrentValues() const;

    // Write interpolated values to APVTS for one morph step
    void applyInterpolated(float progress);

    // Helpers — linear lerp for most params, log-space for frequencies
    static float lerpLinear(float a, float b, float t) { return a + (b - a) * t; }
    static float lerpLog(float a, float b, float t)
    {
        // Interpolate frequency in log space to preserve musical intervals
        return std::exp(lerpLinear(std::log(std::max(a, 0.001f)),
                                   std::log(std::max(b, 0.001f)), t));
    }
    static float lerpCosine(float a, float b, float t)
    {
        // Equal-power cosine interpolation for MIX — prevents audible dip at 50%
        float smooth = 0.5f - 0.5f * std::cos(t * juce::MathConstants<float>::pi);
        return a + (b - a) * smooth;
    }

    // Helper to write a float param if it's not excluded
    void writeParam(const char* id, int listenerIdx, float value);
    void writeChoice(const char* id, int value, float progress, int startVal, int endVal);

};
