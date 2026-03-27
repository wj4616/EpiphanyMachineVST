#pragma once
#include <JuceHeader.h>
#include "DSP/GlitchEngine.h"
#include "DSP/DiffusionNetwork.h"
#include "DSP/PitchDrifter.h"
#include "DSP/StereoWidener.h"
#include "DSP/TextureBodyDetector.h"
#include "DSP/DualFilter.h"
#include "Presets/PresetManager.h"

class EpiphanyMachineProcessor : public juce::AudioProcessor
{
public:
    EpiphanyMachineProcessor();
    ~EpiphanyMachineProcessor() override = default;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }
    const juce::String getName() const override { return "Epiphany Machine"; }
    bool acceptsMidi() const override  { return false; }
    bool producesMidi() const override { return false; }
    double getTailLengthSeconds() const override { return 8.0; }
    int getNumPrograms() override  { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return "Default"; }
    void changeProgramName(int, const juce::String&) override {}
    void getStateInformation(juce::MemoryBlock&) override {}
    void setStateInformation(const void*, int) override {}
    bool isBusesLayoutSupported(const BusesLayout&) const override;

    juce::AudioProcessorValueTreeState apvts;
    PresetManager presetManager;

private:
    // --- Core DSP (Alien Machine heritage) ---
    GlitchEngine     glitchEngine;
    DiffusionNetwork diffusionNetwork;
    PitchDrifter     pitchDrifter;
    StereoWidener    stereoWidener;

    // --- Epiphany additions ---
    TextureBodyDetector inputDetector;   // runs on raw input → glitch focus routing
    TextureBodyDetector filterDetector;  // runs on the signal being filtered → DualFilter
    DualFilter          dualFilter;

    // Feedback carry-over between blocks
    float feedbackL = 0.0f, feedbackR = 0.0f;
    float fbLpfStateL = 0.0f, fbLpfStateR = 0.0f, fbLpfCoeff = 0.0f;

    // Working buffers (allocated in prepareToPlay)
    juce::AudioBuffer<float> dryBuffer;
    juce::AudioBuffer<float> glitchInputBuffer;
    juce::AudioBuffer<float> cleanPassBuffer;
    juce::AudioBuffer<float> wetBuffer;
    juce::AudioBuffer<float> filteredBuffer;
    juce::AudioBuffer<float> processedBuffer;
    juce::AudioBuffer<float> fbBuf;  // feedback pitch-drift buffer — pre-allocated, never heap in processBlock

    // SmoothedValues for per-sample parameters
    juce::SmoothedValue<float> morphSmoothed;
    juce::SmoothedValue<float> mixSmoothed;
    juce::SmoothedValue<float> feedbackSmoothed;
    juce::SmoothedValue<float> glitchFocusSmoothed;
    juce::SmoothedValue<float> bypassSmoothed;

    int lastPresetIdx = 0;

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EpiphanyMachineProcessor)
};
