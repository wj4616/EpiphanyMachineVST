#include "TextureBodyDetector.h"

void TextureBodyDetector::prepare(double sr, int maxBlockSize)
{
    sampleRate = sr;

    // One-pole coefficient: coeff = exp(-1 / (timeSeconds * sampleRate))
    // Higher coeff = slower response.  Mnemonic: "closer to 1 = lazier".
    fastAttackCoeff   = expf(-1.0f / (0.005f * (float)sr));   // 5ms attack
    fastReleaseCoeff  = expf(-1.0f / (0.05f  * (float)sr));   // 50ms release
    slowAttackCoeff   = expf(-1.0f / (0.1f   * (float)sr));   // 100ms attack
    slowReleaseCoeff  = expf(-1.0f / (0.5f   * (float)sr));   // 500ms release
    smoothingCoeff    = expf(-1.0f / (0.005f * (float)sr));   // 5ms smoothing

    // Allocate output buffers
    textureBuffer.setSize(2, maxBlockSize);
    bodyBuffer.setSize(2, maxBlockSize);
    textureGainBuf.resize(maxBlockSize, 0.0f);

    reset();
}

void TextureBodyDetector::reset()
{
    fastEnvelope    = 0.0f;
    slowEnvelope    = 0.0f;
    prevTextureGain = 0.0f;
    textureBuffer.clear();
    bodyBuffer.clear();
}

void TextureBodyDetector::process(const juce::AudioBuffer<float>& input, int numSamples)
{
    const auto* inL = input.getReadPointer(0);
    const auto* inR = input.getReadPointer(1);
    auto* texL = textureBuffer.getWritePointer(0);
    auto* texR = textureBuffer.getWritePointer(1);
    auto* bodL = bodyBuffer.getWritePointer(0);
    auto* bodR = bodyBuffer.getWritePointer(1);

    const float sensitivityGain = 1.0f + sensitivity * 7.0f;  // maps 0–1 → 1x–8x

    for (int i = 0; i < numSamples; ++i)
    {
        // Analyse mono sum — stereo image is preserved by applying the gain to both channels
        float mono       = (inL[i] + inR[i]) * 0.5f;
        float inputLevel = std::abs(mono);

        // --- Fast envelope (attack or release based on signal direction) ---
        if (inputLevel > fastEnvelope)
            fastEnvelope = fastAttackCoeff * fastEnvelope + (1.0f - fastAttackCoeff) * inputLevel;
        else
            fastEnvelope = fastReleaseCoeff * fastEnvelope + (1.0f - fastReleaseCoeff) * inputLevel;

        // --- Slow envelope ---
        if (inputLevel > slowEnvelope)
            slowEnvelope = slowAttackCoeff * slowEnvelope + (1.0f - slowAttackCoeff) * inputLevel;
        else
            slowEnvelope = slowReleaseCoeff * slowEnvelope + (1.0f - slowReleaseCoeff) * inputLevel;

        // --- Texture detection ---
        // The fast follower sees timbral changes before the slow one catches up.
        // Their difference captures that "change" — scaled by SENSITIVITY.
        float rawTexture = fastEnvelope - slowEnvelope;
        float tg = juce::jlimit(0.0f, 1.0f, rawTexture * sensitivityGain);

        // Smooth to prevent chattering (rapid texture↔body switching causes buzzing)
        tg = smoothingCoeff * prevTextureGain + (1.0f - smoothingCoeff) * tg;
        prevTextureGain = tg;
        textureGainBuf[i] = tg;

        // --- Split signal ---
        texL[i] = inL[i] * tg;
        texR[i] = inR[i] * tg;
        bodL[i] = inL[i] * (1.0f - tg);
        bodR[i] = inR[i] * (1.0f - tg);
    }
}
