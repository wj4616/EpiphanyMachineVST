#include "DiffusionNetwork.h"

void DiffusionNetwork::setFreezeActive(bool on)
{
    isFrozen = on;
    // Target values: frozen = input off, feedback maxed, damping muted
    freezeInputGain.setTargetValue(on ? 0.0f : 1.0f);
    freezeFeedbackBoost.setTargetValue(on ? 1.0f : 0.0f);
    freezeDampingMute.setTargetValue(on ? 1.0f : 0.0f);
}

void DiffusionNetwork::prepare(double sr, int /*samplesPerBlock*/)
{
    sampleRate = sr;
    for (int i = 0; i < kNumLines; ++i)
        delayLengths[i] = (int)(kBaseLengths[i] * (sr / 48000.0));

    for (int i = 0; i < kNumLines; ++i)
        lfoPhase[i] = (float)i / (float)kNumLines;

    // Freeze smoothers ramp over 2048 samples (~42ms at 48kHz)
    const double freezeRamp = 2048.0 / sr;
    freezeInputGain.reset(sr, freezeRamp);
    freezeFeedbackBoost.reset(sr, freezeRamp);
    freezeDampingMute.reset(sr, freezeRamp);
    freezeInputGain.setCurrentAndTargetValue(1.0f);
    freezeFeedbackBoost.setCurrentAndTargetValue(0.0f);
    freezeDampingMute.setCurrentAndTargetValue(0.0f);

    reset();
}

void DiffusionNetwork::reset()
{
    for (int i = 0; i < kNumLines; ++i)
    {
        std::fill(std::begin(bufL[i]), std::end(bufL[i]), 0.0f);
        std::fill(std::begin(bufR[i]), std::end(bufR[i]), 0.0f);
        writeIndex[i]  = 0;
        lpfStateL[i]   = lpfStateR[i] = 0.0f;
    }
}

//==============================================================================
// Hadamard butterfly — two-stage mix of 4 values.
// This distributes energy from each delay line to all others evenly,
// making the reverb "diffuse" rather than producing discrete echoes.
//==============================================================================
void DiffusionNetwork::hadamardMix(float v[kNumLines]) const
{
    float a = v[0] + v[1], b = v[0] - v[1];
    float c = v[2] + v[3], d = v[2] - v[3];
    v[0] = (a + c) * 0.5f;
    v[1] = (b + d) * 0.5f;
    v[2] = (a - c) * 0.5f;
    v[3] = (b - d) * 0.5f;
}

void DiffusionNetwork::process(juce::AudioBuffer<float>& buffer)
{
    const int numSamples = buffer.getNumSamples();
    auto* inL = buffer.getWritePointer(0);
    auto* inR = buffer.getWritePointer(1);

    const float normalGain = decayParam * 0.95f;
    const float damping    = dampingParam * 0.9f;

    const float twoPi = juce::MathConstants<float>::twoPi;
    const float lfoPhaseInc[kNumLines] = {
        kLfoRates[0] / (float)sampleRate, kLfoRates[1] / (float)sampleRate,
        kLfoRates[2] / (float)sampleRate, kLfoRates[3] / (float)sampleRate
    };

    for (int i = 0; i < numSamples; ++i)
    {
        // --- Advance freeze smoothers ---
        const float inputGain  = freezeInputGain.getNextValue();
        const float fbBoost    = freezeFeedbackBoost.getNextValue();
        const float dampMute   = freezeDampingMute.getNextValue();

        // Freeze blends feedback gain between normal and 0.999
        const float feedbackGain = normalGain * (1.0f - fbBoost) + 0.999f * fbBoost;
        // Freeze blends damping toward 0 (bright, non-darkening frozen tail)
        const float activeDamping = damping * (1.0f - dampMute);

        float inSampleL = inL[i] * inputGain;
        float inSampleR = inR[i] * inputGain;

        float readL[kNumLines], readR[kNumLines];

        for (int line = 0; line < kNumLines; ++line)
        {
            const float lfoDepth = 3.0f + line * 1.0f;
            float lfoMod = lfoDepth * std::sin(lfoPhase[line] * twoPi);

            float readPos = (float)(writeIndex[line] - delayLengths[line]) + lfoMod;
            int   intPos  = (int)readPos;
            float frac    = readPos - (float)intPos;
            if (frac < 0.0f) { frac += 1.0f; --intPos; }

            int idx0 = intPos & kBufMask;
            int idx1 = (intPos + 1) & kBufMask;
            readL[line] = bufL[line][idx0] * (1.0f - frac) + bufL[line][idx1] * frac;
            readR[line] = bufR[line][idx0] * (1.0f - frac) + bufR[line][idx1] * frac;

            lfoPhase[line] += lfoPhaseInc[line];
            if (lfoPhase[line] >= 1.0f) lfoPhase[line] -= 1.0f;
        }

        hadamardMix(readL);
        hadamardMix(readR);

        for (int line = 0; line < kNumLines; ++line)
        {
            float fbL = readL[line] * feedbackGain;
            float fbR = readR[line] * feedbackGain;

            lpfStateL[line] = (1.0f - activeDamping) * fbL + activeDamping * lpfStateL[line];
            lpfStateR[line] = (1.0f - activeDamping) * fbR + activeDamping * lpfStateR[line];

            // Denormal prevention: tiny constant flushes subnormal floats
            // that cause massive CPU spikes in long reverb tails
            bufL[line][writeIndex[line]] = inSampleL + lpfStateL[line] + 1.0e-18f;
            bufR[line][writeIndex[line]] = inSampleR + lpfStateR[line] + 1.0e-18f;

            writeIndex[line] = (writeIndex[line] + 1) & kBufMask;
        }

        inL[i] = (readL[0] + readL[1]) * 0.5f;
        inR[i] = (readR[2] + readR[3]) * 0.5f;
    }
}
