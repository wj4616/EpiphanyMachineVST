#include "PitchDrifter.h"

void PitchDrifter::buildHannWindow()
{
    // Hann window: w[n] = 0.5 * (1 - cos(2π*n / N))
    // It goes 0→1→0 over kGrainSize samples, avoiding clicks at grain edges.
    for (int n = 0; n < kGrainSize; ++n)
        hannWindow[n] = 0.5f * (1.0f - std::cos(
            juce::MathConstants<float>::twoPi * (float)n / (float)(kGrainSize - 1)));
}

void PitchDrifter::prepare(double sr, int /*samplesPerBlock*/)
{
    sampleRate = sr;
    buildHannWindow();
    reset();
}

void PitchDrifter::reset()
{
    std::fill(std::begin(bufL), std::end(bufL), 0.0f);
    std::fill(std::begin(bufR), std::end(bufR), 0.0f);
    writeHead = 0;

    // Start the two read heads half a grain apart so their windows are
    // perfectly interleaved (when head0 is at peak, head1 is at zero-crossing)
    readHead0 = 0.0f;
    readHead1 = (float)(kGrainSize / 2);
}

//==============================================================================
// Linear interpolation read from a circular float buffer.
// pos can be any float; we wrap it with the bitmask after flooring to int.
//==============================================================================
float PitchDrifter::readInterp(const float* buf, float pos) const
{
    // Wrap to positive range before casting to int to avoid UB with negative fmod
    pos = std::fmod(pos + (float)kBufSize * 4.0f, (float)kBufSize);
    int   idx  = (int)pos & kBufMask;
    float frac = pos - std::floor(pos);
    int   next = (idx + 1) & kBufMask;
    return buf[idx] * (1.0f - frac) + buf[next] * frac;
}

//==============================================================================
void PitchDrifter::process(juce::AudioBuffer<float>& buffer)
{
    const int numSamples = buffer.getNumSamples();
    auto* inL = buffer.getWritePointer(0);
    auto* inR = buffer.getWritePointer(1);

    // Bypass entirely when no drift is set (avoids artifacts from pitch ops)
    if (std::abs(driftSemitones) < 0.01f)
        return;

    // pitch ratio: 2^(semitones/12)
    // e.g. +12 semitones → ratio=2.0 (octave up, read twice as fast)
    const float pitchRatio = std::pow(2.0f, driftSemitones / 12.0f);

    for (int i = 0; i < numSamples; ++i)
    {
        // ---- 1. Write input into capture buffer ----
        bufL[writeHead & kBufMask] = inL[i];
        bufR[writeHead & kBufMask] = inR[i];

        // ---- 2. Compute Hann window weight for each read head ----
        // The head's position within the grain cycle (0..kGrainSize-1)
        // determines where it is in its Hann window.
        //
        // fmod wraps the read head position to [0, kGrainSize) to index
        // into the precomputed window table.
        float pos0InGrain = std::fmod(readHead0, (float)kGrainSize);
        float pos1InGrain = std::fmod(readHead1, (float)kGrainSize);
        if (pos0InGrain < 0) pos0InGrain += kGrainSize;
        if (pos1InGrain < 0) pos1InGrain += kGrainSize;

        int   wi0   = (int)pos0InGrain & (kGrainSize - 1);
        int   wi1   = (int)pos1InGrain & (kGrainSize - 1);
        float win0  = hannWindow[wi0];
        float win1  = hannWindow[wi1];

        // ---- 3. Read from buffer at each head (with linear interpolation) ----
        float out0L = readInterp(bufL, readHead0) * win0;
        float out0R = readInterp(bufR, readHead0) * win0;
        float out1L = readInterp(bufL, readHead1) * win1;
        float out1R = readInterp(bufR, readHead1) * win1;

        // Sum both windowed heads — when windows are complementary, their
        // sum is always ~1.0, giving a smooth, seamless output
        inL[i] = out0L + out1L;
        inR[i] = out0R + out1R;

        // ---- 4. Advance write head and read heads ----
        ++writeHead;
        readHead0 += pitchRatio;
        readHead1 += pitchRatio;

        // Wrap read heads within buffer bounds
        if (readHead0 >= (float)kBufSize) readHead0 -= (float)kBufSize;
        if (readHead1 >= (float)kBufSize) readHead1 -= (float)kBufSize;
    }
}
