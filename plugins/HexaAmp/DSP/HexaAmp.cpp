#include "HexaAmp.h"

namespace jnsc::hexa_amp {

void HexaAmp::prepare(size_t newMaxBlockSize, float newSampleRate) {
    using Response = BiquadFilter<float>::Response;

    // Store global parameters
    sampleRate = utils::detail::clampSampleRate(newSampleRate);

    // Prepare oversampler
    oversampler.prepare(1, newMaxBlockSize);
    oversampledBuffer.resize(1, OVERSAMPLING_FACTOR * newMaxBlockSize);

    // Prepare two sections for pre-filter: highpass, tilt
    preFilter.prepare(1, sampleRate, 2);

    // HIGHPASS
    preFilter.section(0).setResponse(Response::Highpass);
    preFilter.section(0).setFrequency(Frequency<float>::Hertz(PRE_HIGH_PASS_CUTOFF_HZ));

    // TILT
    preFilter.section(1).setResponse(Response::Tilt);
    preFilter.section(1).setFrequency(Frequency<float>::Hertz(PRE_TILT_FREQUENCY_HZ));
    preFilter.section(1).setQ(PRE_TILT_Q);

    // Prepare mid booster
    midBooster.prepare(1, OVERSAMPLING_FACTOR * sampleRate);
    midBooster.setResponse(Response::Peak);
    midBooster.setQ(MID_BOOST_Q);
    midBooster.setFrequency(Frequency<float>::Hertz(MID_BOOST_FREQUENCY_HZ));

    // Prepare post-filter sections for color control (highpass, bass, low-mid, high-mid, lowpassx2)
    postFilter.prepare(1, sampleRate, 6);
    postFilter.section(0).setResponse(Response::Highpass);
    postFilter.section(0).setQ(0.7071068f);
    postFilter.section(1).setResponse(Response::Peak);
    postFilter.section(2).setResponse(Response::Peak);
    postFilter.section(3).setResponse(Response::Peak);
    postFilter.section(1).setQ(POST_BASS_Q);
    postFilter.section(2).setQ(POST_LOW_MID_Q);
    postFilter.section(3).setQ(POST_HIGH_MID_Q);

    // 4th order butterworth
    postFilter.section(4).setResponse(Response::Lowpass);
    postFilter.section(5).setResponse(Response::Lowpass);
    postFilter.section(4).setQ(POST_LOWPASS_STAGE1_Q);
    postFilter.section(5).setQ(POST_LOWPASS_STAGE2_Q);

    // Prepare envelope followers
    biasEnvelopeFollower.prepare(1, OVERSAMPLING_FACTOR * sampleRate);
    biasEnvelopeFollower.setAttackTime(Time<float>::Milliseconds(BIAS_MOD_ATTACK_MS), true);
    biasEnvelopeFollower.setReleaseTime(Time<float>::Milliseconds(BIAS_MOD_RELEASE_MS), true);
    headroomEnvelopeFollower.prepare(1, OVERSAMPLING_FACTOR * sampleRate);
    headroomEnvelopeFollower.setAttackTime(Time<float>::Milliseconds(HEADROOM_MOD_ATTACK_MS), true);
    headroomEnvelopeFollower.setReleaseTime(Time<float>::Milliseconds(HEADROOM_MOD_RELEASE_MS), true);

    // SET DSP PARAM SMOOTHING
    drive.prepare(1, OVERSAMPLING_FACTOR * sampleRate);
    level.prepare(1, OVERSAMPLING_FACTOR * sampleRate);
    squish.prepare(1, OVERSAMPLING_FACTOR * sampleRate);
    squish.setBounds(0.0f, 1.0f);
    drive.setSmoothingTime(Time<float>::Milliseconds(PARAM_SMOOTH_TIME_MS));
    level.setSmoothingTime(Time<float>::Milliseconds(PARAM_SMOOTH_TIME_MS));
    squish.setSmoothingTime(Time<float>::Milliseconds(PARAM_SMOOTH_TIME_MS));

    // Set Default Parameters
    setDrive(50.0, true);  // 50% drive
    setBite(50.0, true);   // 50% bite
    setSquish(50.0, true); // 50% squish
    setLevel(50.0, true);  // 50% level
    setColor(50.0, true);  // 50% color
    setTilt(50.0, true);   // 50% body
}

void HexaAmp::reset() {
    preFilter.reset();
    oversampler.reset();
    midBooster.reset();
    postFilter.reset();
    biasEnvelopeFollower.reset();
    headroomEnvelopeFollower.reset();
    oversampledBuffer.clear();
}

void HexaAmp::processBlock(const float* input, float* output, size_t numSamples) {
    const float* inputs[] = {input};
    float* outputs[] = {output};

    // Pre-filter processing
    preFilter.processBlock(inputs, outputs, numSamples);

    // Upsample
    size_t numOversampledSamples = oversampler.upsample(outputs, oversampledBuffer.writePtrs(), numSamples);

    // Distortion processing
    for (size_t n = 0; n < numOversampledSamples; ++n) {

        // Current input
        float in = oversampledBuffer[0][n];

        // Apply pre-amp non linearity
        float preAmpOutput = waveshaper.processSample(PRE_AMP_GAIN_LIN * in + PRE_AMP_BIAS, PRE_AMP_SHAPE) -
                             waveshaper.processSample(PRE_AMP_BIAS, PRE_AMP_SHAPE);

        // Mid Boost
        float midBoosted = midBooster.processSample(0, preAmpOutput);

        const float squishAmount = squish.getNextValue(0);

        // Process bias mod envelope follower
        float biasEnv = biasEnvelopeFollower.processSample(0, midBoosted);
        float biasMod = squishAmount * biasEnv / (biasEnv + BIAS_MOD_SENSITIVITY);
        float bias = biasMod * POWER_AMP_BIAS_MAX;

        // Process headroom mod envelope follower
        float headroomEnv = headroomEnvelopeFollower.processSample(0, midBoosted);
        float headroomMod = squishAmount * headroomEnv / (headroomEnv + HEADROOM_MOD_SENSITIVITY);
        float headroom = 1.0f - (1.0f - POWER_AMP_HEADROOM_MIN) * headroomMod;

        // Process the biased sample through the waveshaper with current headroom
        float driveGain = drive.getNextValue(0);
        float driven = midBoosted * driveGain;
        float powerAmpIn = driven / headroom;

        oversampledBuffer[0][n] = std::sqrt(headroom) * (waveshaper.processSample(powerAmpIn + bias, POWER_AMP_SHAPE) -
                                                         waveshaper.processSample(bias, POWER_AMP_SHAPE));

        // Apply level gain
        oversampledBuffer[0][n] *= level.getNextValue(0);
    }

    // Downsample
    oversampler.downsample(oversampledBuffer.readPtrs(), outputs, numSamples);

    // Post-filter processing
    postFilter.processBlock(outputs, outputs, numSamples);
}

void HexaAmp::setDrive(float drivePercent, bool skipSmoothing) {
    // Implementation for setting drive
    float t = std::clamp(drivePercent * 0.01f, 0.0f, 1.0f);
    float driveDb = DRIVE_MIN_DB + (DRIVE_MAX_DB - DRIVE_MIN_DB) * t;
    drive.setTarget(utils::dB2Mag(driveDb), skipSmoothing);
}

void HexaAmp::setBite(float bitePercent, bool skipSmoothing) {
    // Implementation for setting bite
    float biteDb = MID_BOOST_MIN_DB + (MID_BOOST_MAX_DB - MID_BOOST_MIN_DB) * (bitePercent * 0.01);
    midBooster.setGain(Gain<float>::Decibels(biteDb));
}

void HexaAmp::setSquish(float squishPercent, bool skipSmoothing) {
    // Implementation for setting squish
    float amount = std::clamp(squishPercent * 0.01f, 0.0f, 1.0f);
    squish.setTarget(amount, skipSmoothing);
}

void HexaAmp::setLevel(float gainPercent, bool skipSmoothing) {
    // Implementation for setting level
    float t = std::clamp(gainPercent * 0.01f, 0.0f, 1.0f);
    level.setTarget(t * t, skipSmoothing);
}

void HexaAmp::setColor(float colorPercent, bool skipSmoothing) {
    const float t = std::clamp(colorPercent * 0.01f, 0.0f, 1.0f);
    const auto frequency = [t](float start, float end) { return start * std::pow(end / start, t); };
    const auto gain = [t](float start, float end) { return start + (end - start) * t; };

    postFilter.section(0).setFrequency(
        Frequency<float>::Hertz(frequency(POST_HIGH_PASS_MIN_CUTOFF_HZ, POST_HIGH_PASS_MAX_CUTOFF_HZ)));

    postFilter.section(1).setGain(Gain<float>::Decibels(gain(POST_BASS_MAX_GAIN_DB, POST_BASS_MIN_GAIN_DB)));
    postFilter.section(1).setFrequency(
        Frequency<float>::Hertz(frequency(POST_BASS_MIN_FREQUENCY_HZ, POST_BASS_MAX_FREQUENCY_HZ)));

    postFilter.section(2).setGain(Gain<float>::Decibels(gain(POST_LOW_MID_MAX_GAIN_DB, POST_LOW_MID_MIN_GAIN_DB)));
    postFilter.section(2).setFrequency(
        Frequency<float>::Hertz(frequency(POST_LOW_MID_MIN_FREQUENCY_HZ, POST_LOW_MID_MAX_FREQUENCY_HZ)));

    postFilter.section(3).setGain(Gain<float>::Decibels(gain(POST_HIGH_MID_MIN_GAIN_DB, POST_HIGH_MID_MAX_GAIN_DB)));
    postFilter.section(3).setFrequency(
        Frequency<float>::Hertz(frequency(POST_HIGH_MID_MIN_FREQUENCY_HZ, POST_HIGH_MID_MAX_FREQUENCY_HZ)));

    const float lowpassFreq = frequency(POST_LOWPASS_MIN_CUTOFF_HZ, POST_LOWPASS_MAX_CUTOFF_HZ);
    postFilter.section(4).setFrequency(Frequency<float>::Hertz(lowpassFreq));
    postFilter.section(5).setFrequency(Frequency<float>::Hertz(lowpassFreq));
}

void HexaAmp::setTilt(float tiltPercent, bool skipSmoothing) {
    // Implementation for setting body
    float tiltDb = PRE_TILT_MIN_DB + (PRE_TILT_MAX_DB - PRE_TILT_MIN_DB) * (tiltPercent * 0.01);
    preFilter.section(1).setGain(Gain<float>::Decibels(tiltDb));
}

} // namespace jnsc::hexa_amp
