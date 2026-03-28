#include "DiffusionNetwork.h"

void DiffusionNetwork::setFreezeActive(bool on)
{
    freezeInputGain.setTargetValue(on ? 0.0f : 1.0f);
    freezeFeedbackBoost.setTargetValue(on ? 1.0f : 0.0f);
    freezeDampingMute.setTargetValue(on ? 1.0f : 0.0f);
}

void DiffusionNetwork::prepare(double sr, int)
{
    sampleRate = sr;
    for (int i = 0; i < kNumLines; ++i)
    {
        delayLengths[i] = (int)(kBaseLengths[i] * (sr / 48000.0));
        lfoPhase[i]     = (float)i / (float)kNumLines;
    }
    const double freezeRamp = 2048.0 / sr;
    freezeInputGain.reset(sr, freezeRamp);     freezeInputGain.setCurrentAndTargetValue(1.0f);
    freezeFeedbackBoost.reset(sr, freezeRamp); freezeFeedbackBoost.setCurrentAndTargetValue(0.0f);
    freezeDampingMute.reset(sr, freezeRamp);   freezeDampingMute.setCurrentAndTargetValue(0.0f);
    reset();
}

void DiffusionNetwork::reset()
{
    for (int i = 0; i < kNumLines; ++i)
    {
        std::fill(std::begin(bufL[i]), std::end(bufL[i]), 0.0f);
        std::fill(std::begin(bufR[i]), std::end(bufR[i]), 0.0f);
        writeIndex[i]  = 0;
        lpfStateL[i]   = 0.0f;
        lpfStateR[i]   = 0.0f;
    }
}

// 8-element Fast Hadamard Transform — 3 butterfly stages, normalised by 1/sqrt(8).
void DiffusionNetwork::hadamardMix(float v[kNumLines]) const
{
    for (int stride = 1; stride < kNumLines; stride <<= 1)
        for (int i = 0; i < kNumLines; i += stride * 2)
            for (int j = 0; j < stride; ++j)
            {
                float a = v[i+j], b = v[i+j+stride];
                v[i+j] = a + b; v[i+j+stride] = a - b;
            }
    for (int i = 0; i < kNumLines; ++i) v[i] *= 0.3536f;  // 1/sqrt(8)
}

void DiffusionNetwork::process(juce::AudioBuffer<float>& buffer)
{
    const int numSamples = buffer.getNumSamples();
    auto* inL = buffer.getWritePointer(0);
    auto* inR = buffer.getWritePointer(1);

    const float damping = dampingParam * 0.9f;
    const float twoPi   = juce::MathConstants<float>::twoPi;

    // Pre-compute RT60 gains and LFO increments (constant per block)
    float normalGains[kNumLines];
    float lfoInc[kNumLines];
    for (int line = 0; line < kNumLines; ++line)
    {
        normalGains[line] = std::pow(10.0f,
            -3.0f * (float)delayLengths[line] / (decaySeconds * (float)sampleRate));
        lfoInc[line] = kLfoRates[line] / (float)sampleRate;
    }

    for (int i = 0; i < numSamples; ++i)
    {
        const float inputGain  = freezeInputGain.getNextValue();
        const float fbBoost    = freezeFeedbackBoost.getNextValue();
        const float dampMute   = freezeDampingMute.getNextValue();
        const float activeDamp = damping * (1.0f - dampMute);

        float tapL[kNumLines], tapR[kNumLines];

        // Step 1: Read delay outputs, apply feedback gain + LPF
        for (int line = 0; line < kNumLines; ++line)
        {
            const float fbGain = normalGains[line] * (1.0f - fbBoost) + 0.999f * fbBoost;
            int readIdx = (writeIndex[line] - delayLengths[line] + kBufSize) & kBufMask;
            float fL = bufL[line][readIdx] * fbGain;
            float fR = bufR[line][readIdx] * fbGain;
            lpfStateL[line] = (1.0f - activeDamp) * fL + activeDamp * lpfStateL[line] + 1e-18f;
            lpfStateR[line] = (1.0f - activeDamp) * fR + activeDamp * lpfStateR[line] + 1e-18f;
            tapL[line] = lpfStateL[line];
            tapR[line] = lpfStateR[line];

            lfoPhase[line] += lfoInc[line];
            if (lfoPhase[line] >= 1.0f) lfoPhase[line] -= 1.0f;
        }

        // Step 2: FHT mixing in feedback path — cross-couples all delay lines
        hadamardMix(tapL);
        hadamardMix(tapR);

        // Step 3: Write mixed feedback + new input to delay lines
        for (int line = 0; line < kNumLines; ++line)
        {
            bufL[line][writeIndex[line]] = inL[i] * inputGain + tapL[line];
            bufR[line][writeIndex[line]] = inR[i] * inputGain + tapR[line];
            writeIndex[line] = (writeIndex[line] + 1) & kBufMask;
        }

        // Step 4: Sum outputs with per-line panning
        float outL = 0.0f, outR = 0.0f;
        for (int line = 0; line < kNumLines; ++line)
        {
            float panLfo = 0.05f * std::sin(lfoPhase[line] * twoPi);
            float pan    = juce::jlimit(-1.0f, 1.0f, kBasePan[line] * widthParam + panLfo);
            float sig    = (tapL[line] + tapR[line]) * 0.5f;
            outL += sig * 0.5f * (1.0f - pan);
            outR += sig * 0.5f * (1.0f + pan);
        }
        inL[i] = outL * (2.0f / (float)kNumLines);
        inR[i] = outR * (2.0f / (float)kNumLines);
    }
}
