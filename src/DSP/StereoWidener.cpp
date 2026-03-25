#include "StereoWidener.h"

void StereoWidener::prepare(double sr, int /*samplesPerBlock*/)
{
    sampleRate  = sr;
    lfoPhaseInc = 0.15f / (float)sr; // 0.15 Hz spatial LFO
    reset();
}

void StereoWidener::reset()
{
    lfoPhase = 0.0f;
}

void StereoWidener::process(juce::AudioBuffer<float>& buffer)
{
    const int numSamples = buffer.getNumSamples();
    auto* L = buffer.getWritePointer(0);
    auto* R = buffer.getWritePointer(1);

    // Map width 0..1 to widthFactor: 0→0.0, 0.5→1.0, 1.0→2.0
    // (linear remap: widthFactor = width * 2.0)
    const float widthFactor = widthParam * 2.0f;

    // Max LFO gain deviation: ±0.5dB → linear multiplier ≈ ±0.0577
    const float lfoDepth = 0.0577f;
    const float twoPi    = juce::MathConstants<float>::twoPi;

    for (int i = 0; i < numSamples; ++i)
    {
        // ---- Mid-Side encoding ----
        float mid  = (L[i] + R[i]) * 0.5f;
        float side = (L[i] - R[i]) * 0.5f;

        // ---- Scale side channel to adjust width ----
        side *= widthFactor;

        // ---- Decode back to L/R ----
        L[i] = mid + side;
        R[i] = mid - side;

        // ---- Spatial LFO: gently rocks L/R balance (~0.5dB peak) ----
        // This creates a slow, breathing stereo movement in the wet signal.
        float lfo = lfoDepth * std::sin(lfoPhase * twoPi);
        L[i] *= (1.0f + lfo);
        R[i] *= (1.0f - lfo);

        // ---- Soft clip to prevent runaway output from feedback ----
        // x / (1 + |x|) maps any value to (-1, +1), similar to tanh but faster
        L[i] = L[i] / (1.0f + std::abs(L[i]));
        R[i] = R[i] / (1.0f + std::abs(R[i]));

        lfoPhase += lfoPhaseInc;
        if (lfoPhase >= 1.0f) lfoPhase -= 1.0f;
    }
}
