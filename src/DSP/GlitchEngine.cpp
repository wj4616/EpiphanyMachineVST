#include "GlitchEngine.h"

GlitchEngine::GlitchEngine()
{
    bufL.fill(0.0f);
    bufR.fill(0.0f);
}

void GlitchEngine::prepare(double sr, int /*samplesPerBlock*/)
{
    sampleRate = sr;
    reset();
}

void GlitchEngine::reset()
{
    bufL.fill(0.0f);
    bufR.fill(0.0f);
    writeHead = playHead = grainSamplesLeft = 0;
    grainStart = grainLength = 0;
    grainReverse = false;
    grainSpeed = 1.0f;
    crossfadeCounter = 0;
    prevSampleL = prevSampleR = 0.0f;
}

//==============================================================================
// process() — called once per block from the Processor.
// We process sample-by-sample for grain-accurate triggering.
//==============================================================================
void GlitchEngine::process(juce::AudioBuffer<float>& buffer)
{
    const int numSamples = buffer.getNumSamples();
    auto* inL = buffer.getWritePointer(0);
    auto* inR = buffer.getWritePointer(1);

    //==========================================================================
    // DENSITY → grain trigger interval.
    // At density=0: event every ~500ms (long ambient grains).
    // At density=1: event every ~10ms (rapid stutter).
    //
    // We map density to a trigger period in samples, then count down.
    // When the counter hits zero, a new grain starts.
    //==========================================================================
    for (int i = 0; i < numSamples; ++i)
    {
        // ---- 1. Always write incoming audio into the capture buffer ----
        bufL[writeHead & kBufMask] = inL[i];
        bufR[writeHead & kBufMask] = inR[i];
        ++writeHead;

        // ---- 2. Check if it's time to trigger a new grain ----
        if (grainSamplesLeft <= 0)
            triggerNewGrain();

        // ---- 3. Read grain sample from the buffer ----
        float outL, outR;

        if (grainReverse)
        {
            // Read backwards: start + length - sampleIndex
            // We track position by counting down within the grain
            int pos = (grainStart + grainLength - (grainLength - grainSamplesLeft)) & kBufMask;
            outL = bufL[pos];
            outR = bufR[pos];
        }
        else if (grainSpeed != 1.0f)
        {
            // Half-speed: advance playhead at 0.5 samples/sample using linear interp
            // playHead stores a fixed-point position; we use a float accumulator
            int   idx  = playHead & kBufMask;
            outL = bufL[idx];
            outR = bufR[idx];

            // Only advance playhead every other sample for half speed
            if ((grainLength - grainSamplesLeft) % 2 == 0)
                ++playHead;
        }
        else
        {
            int pos = playHead & kBufMask;
            outL = bufL[pos];
            outR = bufR[pos];
            ++playHead;
        }

        // ---- 4. Hann window crossfade at grain boundaries ----
        // At the start of each grain (crossfadeCounter > 0), we blend from
        // the previous grain's last sample to avoid a hard click.
        if (crossfadeCounter > 0)
        {
            // t goes from 1.0 → 0.0 over kCrossfade samples
            float t = (float)crossfadeCounter / (float)kCrossfade;
            outL = outL * (1.0f - t) + prevSampleL * t;
            outR = outR * (1.0f - t) + prevSampleR * t;
            --crossfadeCounter;
        }

        prevSampleL = outL;
        prevSampleR = outR;

        inL[i] = outL;
        inR[i] = outR;

        --grainSamplesLeft;
    }
}

//==============================================================================
// triggerNewGrain() — set up the next grain's position, length, direction, speed.
//==============================================================================
void GlitchEngine::triggerNewGrain()
{
    // ---- Grain length based on DENSITY ----
    // Low density → long grains (100–500ms). High density → short (10–80ms).
    const float maxGrainMs = 500.0f - density * 420.0f; // 500ms → 80ms
    const float minGrainMs = 100.0f - density * 90.0f;  // 100ms → 10ms
    float grainMs = minGrainMs + rng.nextFloat() * (maxGrainMs - minGrainMs);
    grainLength = (int)(grainMs * 0.001f * (float)sampleRate);
    grainSamplesLeft = grainLength;

    // ---- Grain start position ----
    // At low chaos, pick from recent audio (last 200ms).
    // At high chaos (>0.7), allow random jump anywhere in the buffer.
    int recentWindow = (int)(sampleRate * 0.2); // 200ms of recent audio

    if (chaos > 0.7f && rng.nextFloat() < (chaos - 0.7f) / 0.3f)
    {
        // Random jump anywhere in the buffer
        grainStart = rng.nextInt(kBufSize);
    }
    else
    {
        // Pick from recent audio, slightly randomised by chaos
        int offset = (int)(rng.nextFloat() * chaos * recentWindow);
        grainStart = (writeHead - grainLength - offset + kBufSize) & kBufMask;
    }

    playHead = grainStart;

    // ---- Direction and speed based on CHAOS ----
    grainReverse = false;
    grainSpeed    = 1.0f;

    if (chaos > 0.0f)
    {
        float r = rng.nextFloat();

        if (r < chaos * 0.4f)
            grainReverse = true;                         // reverse

        if (r > 0.6f && r < 0.6f + chaos * 0.3f)
            grainSpeed = 0.5f;                           // half speed
    }

    // Start crossfade counter so the grain fades in smoothly
    crossfadeCounter = kCrossfade;
}
