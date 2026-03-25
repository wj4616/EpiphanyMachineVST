#pragma once
#include <JuceHeader.h>

//==============================================================================
// TextureBodyDetector — dual envelope follower that splits audio into
// "texture" (timbral movement) and "body" (stable level) streams.
//
// Two instances live in the Processor:
//   inputDetector  — runs on raw input, feeds GLITCH FOCUS routing
//   filterDetector — runs on the signal being filtered, feeds DualFilter
//
// Results are stored internally; use the getter methods after process().
//==============================================================================
class TextureBodyDetector
{
public:
    void prepare(double sampleRate, int maxBlockSize);
    void process(const juce::AudioBuffer<float>& input, int numSamples);
    void reset();

    void setSensitivity(float s) { sensitivity = s; }

    // Call these after process() to read the split streams
    const juce::AudioBuffer<float>& getTextureBuffer() const { return textureBuffer; }
    const juce::AudioBuffer<float>& getBodyBuffer()    const { return bodyBuffer; }
    // Per-sample gain array (length = last numSamples passed to process())
    const float* getTextureGain() const { return textureGainBuf.data(); }

private:
    double sampleRate = 48000.0;
    float sensitivity = 0.5f;

    // Fast follower: attack ~5ms, release ~50ms (tracks timbral movement)
    float fastEnvelope     = 0.0f;
    float fastAttackCoeff  = 0.0f;
    float fastReleaseCoeff = 0.0f;

    // Slow follower: attack ~100ms, release ~500ms (tracks steady level)
    float slowEnvelope     = 0.0f;
    float slowAttackCoeff  = 0.0f;
    float slowReleaseCoeff = 0.0f;

    // Smooth textureGain to prevent chattering artifacts (~5ms)
    float prevTextureGain = 0.0f;
    float smoothingCoeff  = 0.0f;

    // Internal output buffers — allocated in prepare()
    juce::AudioBuffer<float> textureBuffer;
    juce::AudioBuffer<float> bodyBuffer;
    std::vector<float>       textureGainBuf;
};
