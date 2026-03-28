#include "GlitchEngine.h"

GlitchEngine::GlitchEngine() { bufL.fill(0.0f); bufR.fill(0.0f); }

void GlitchEngine::prepare(double sr, int) { sampleRate = sr; reset(); }

void GlitchEngine::reset()
{
    bufL.fill(0.0f); bufR.fill(0.0f);
    writeHead = grainSamplesLeft = grainStart = grainLength = 0;
    grainLengthMs = 100.0f; grainReverse = false; grainSpeed = 1.0f;
}

float GlitchEngine::getWindowValue(int samplePos, int grainLenSamples, float grainMs) const
{
    float t = juce::jlimit(0.0f, 1.0f,
        (float)samplePos / (float)std::max(grainLenSamples - 1, 1));

    if (grainMs > 50.0f)
    {
        float s = std::sin(t * juce::MathConstants<float>::pi);
        return s * s;  // Hann
    }
    else if (grainMs >= 10.0f)
    {
        const float half = 0.15f;  // Tukey alpha=0.3, half=0.15
        if (t < half)
            return 0.5f * (1.0f - std::cos(t / half * juce::MathConstants<float>::pi));
        else if (t > 1.0f - half)
            return 0.5f * (1.0f - std::cos((1.0f - t) / half * juce::MathConstants<float>::pi));
        return 1.0f;
    }
    else
    {
        // Trapezoid: 0.5ms ramps
        const float rampT = (0.5e-3f * (float)sampleRate) / (float)std::max(grainLenSamples, 1);
        if (t < rampT)             return t / rampT;
        else if (t > 1.0f - rampT) return (1.0f - t) / rampT;
        return 1.0f;
    }
}

void GlitchEngine::process(juce::AudioBuffer<float>& buffer)
{
    const int N  = buffer.getNumSamples();
    auto* inL = buffer.getWritePointer(0);
    auto* inR = buffer.getWritePointer(1);

    for (int i = 0; i < N; ++i)
    {
        bufL[writeHead & kBufMask] = inL[i];
        bufR[writeHead & kBufMask] = inR[i];
        ++writeHead;

        if (grainSamplesLeft <= 0) triggerNewGrain();

        const int consumed = grainLength - grainSamplesLeft;
        const float readPos = grainReverse
            ? (float)(grainStart + grainLength - consumed)
            : (float)grainStart + (float)consumed * grainSpeed;

        const int   idx0 = (int)readPos & kBufMask;
        const int   idx1 = (idx0 + 1) & kBufMask;
        const float frac = readPos - std::floor(readPos);
        float outL = bufL[idx0] * (1.0f - frac) + bufL[idx1] * frac;
        float outR = bufR[idx0] * (1.0f - frac) + bufR[idx1] * frac;

        const float win = getWindowValue(consumed, grainLength, grainLengthMs);
        inL[i] = outL * win;
        inR[i] = outR * win;
        --grainSamplesLeft;
    }
}

void GlitchEngine::triggerNewGrain()
{
    const float maxMs = juce::jmap(density, 0.0f, 1.0f, 1000.0f, 50.0f);
    const float minMs = juce::jmap(density, 0.0f, 1.0f,  500.0f,  1.0f);
    grainLengthMs    = minMs + rng.nextFloat() * (maxMs - minMs);
    grainLength      = juce::jmax(1, (int)(grainLengthMs * 0.001f * (float)sampleRate));
    grainSamplesLeft = grainLength;

    const int recentWindow = (int)(sampleRate * 0.2);
    if (chaos > 0.7f && rng.nextFloat() < (chaos - 0.7f) / 0.3f)
        grainStart = rng.nextInt(kBufSize);
    else
    {
        int offset = (int)(rng.nextFloat() * chaos * recentWindow);
        grainStart = (writeHead - grainLength - offset + kBufSize) & kBufMask;
    }

    grainReverse = rng.nextFloat() < chaos * 0.6f;
    grainSpeed   = 1.0f;
    if (!grainReverse)
    {
        float r = rng.nextFloat();
        if      (r < chaos * 0.4f) grainSpeed = 0.5f;
        else if (r < chaos * 0.6f) grainSpeed = 2.0f;
    }

    if (drift > 0.0f)
    {
        float randomSt = (rng.nextFloat() * 2.0f - 1.0f) * drift;
        grainSpeed *= std::pow(2.0f, randomSt / 12.0f);
    }
}
