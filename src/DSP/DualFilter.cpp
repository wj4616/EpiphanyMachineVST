#include "DualFilter.h"

void DualFilter::prepare(double sr, int /*samplesPerBlock*/)
{
    sampleRate = sr;

    texCutoffSmoothed .reset(sr, 0.02); texCutoffSmoothed .setCurrentAndTargetValue(2000.0f);
    texResoSmoothed   .reset(sr, 0.02); texResoSmoothed   .setCurrentAndTargetValue(0.3f);
    texLfoRateSmoothed.reset(sr, 0.02); texLfoRateSmoothed.setCurrentAndTargetValue(0.5f);
    texLfoDepthSmoothed.reset(sr, 0.02);texLfoDepthSmoothed.setCurrentAndTargetValue(0.0f);
    bodyCutoffSmoothed.reset(sr, 0.02); bodyCutoffSmoothed.setCurrentAndTargetValue(1200.0f);
    bodyResoSmoothed  .reset(sr, 0.02); bodyResoSmoothed  .setCurrentAndTargetValue(0.2f);

    reset();
}

void DualFilter::reset()
{
    texL = texR = bodyL = bodyR = {};
    texLfoPhase = 0.0f;
}

//==============================================================================
// Cytomic TPT SVF per-sample processor.
//
// Coefficients are computed OUTSIDE this function (once per sample) so they
// can be shared between L and R channels (same cutoff, same Q).
//
//   g  = tan(π * fc / fs)          ← bilinear frequency mapping
//   k  = 1/Q                        ← Q to k conversion
//   a1 = 1 / (1 + g*(g+k))
//   a2 = g * a1
//   a3 = g * a2
//
// Per-sample update (ic1eq, ic2eq are the integrator states):
//   v3    = input - ic2eq
//   v1    = a1*ic1eq + a2*v3         ← bandpass output
//   v2    = ic2eq + a2*ic1eq + a3*v3 ← lowpass output
//   ic1eq = 2*v1 - ic1eq             ← integrator update (trapezoidal trick)
//   ic2eq = 2*v2 - ic2eq
//   hp    = input - k*v1 - v2        ← highpass output
//==============================================================================
float DualFilter::processTPT(TPTState& s, float input,
                              float a1, float a2, float a3, float k, int type)
{
    float v3 = input - s.ic2eq;
    float v1 = a1 * s.ic1eq + a2 * v3;
    float v2 = s.ic2eq + a2 * s.ic1eq + a3 * v3;
    s.ic1eq  = 2.0f * v1 - s.ic1eq;
    s.ic2eq  = 2.0f * v2 - s.ic2eq;

    float lp = v2;
    float bp = v1;
    float hp = input - k * bp - lp;

    switch (type) {
        case 1:  return hp;
        case 2:  return bp;
        default: return lp;
    }
}

void DualFilter::process(const juce::AudioBuffer<float>& textureIn,
                          const juce::AudioBuffer<float>& bodyIn,
                          juce::AudioBuffer<float>& output,
                          int numSamples)
{
    const auto* texL_in  = textureIn.getReadPointer(0);
    const auto* texR_in  = textureIn.getReadPointer(1);
    const auto* bodL_in  = bodyIn.getReadPointer(0);
    const auto* bodR_in  = bodyIn.getReadPointer(1);
    auto* outL = output.getWritePointer(0);
    auto* outR = output.getWritePointer(1);

    const float pi = juce::MathConstants<float>::pi;

    for (int i = 0; i < numSamples; ++i)
    {
        // --- Texture filter coefficients ---
        float texCutoff = texCutoffSmoothed.getNextValue();
        float texReso   = texResoSmoothed.getNextValue();
        float lfoRate   = texLfoRateSmoothed.getNextValue();
        float lfoDepth  = texLfoDepthSmoothed.getNextValue();

        // LFO modulates cutoff ±3 octaves at full depth
        float lfoVal = std::sin(texLfoPhase * juce::MathConstants<float>::twoPi);
        float modCut = texCutoff * std::pow(2.0f, lfoVal * lfoDepth * 3.0f);
        modCut = juce::jlimit(20.0f, 20000.0f, modCut);

        // Advance LFO phase (rate in Hz → phase increment per sample)
        texLfoPhase += lfoRate / (float)sampleRate;
        if (texLfoPhase >= 1.0f) texLfoPhase -= 1.0f;

        // Compute TPT coefficients — tan() ensures stability at all frequencies
        float texG  = std::tan(pi * modCut / (float)sampleRate);
        float texK  = 1.0f / (0.5f + texReso * 24.5f);  // Q 0.4–50, self-oscillating above ~40
        float texA1 = 1.0f / (1.0f + texG * (texG + texK));
        float texA2 = texG * texA1;
        float texA3 = texG * texA2;

        // --- Body filter coefficients ---
        float bodCutoff = bodyCutoffSmoothed.getNextValue();
        float bodReso   = bodyResoSmoothed.getNextValue();
        float bodG  = std::tan(pi * bodCutoff / (float)sampleRate);
        float bodK  = 1.0f / (0.5f + bodReso * 24.5f);
        float bodA1 = 1.0f / (1.0f + bodG * (bodG + bodK));
        float bodA2 = bodG * bodA1;
        float bodA3 = bodG * bodA2;

        // --- Process and sum ---
        outL[i] = processTPT(texL, texL_in[i], texA1, texA2, texA3, texK, texFilterType)
                + processTPT(bodyL, bodL_in[i], bodA1, bodA2, bodA3, bodK, bodyFilterType);
        outR[i] = processTPT(texR, texR_in[i], texA1, texA2, texA3, texK, texFilterType)
                + processTPT(bodyR, bodR_in[i], bodA1, bodA2, bodA3, bodK, bodyFilterType);
    }
}
