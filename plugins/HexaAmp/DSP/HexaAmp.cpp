#include "HexaAmp.h"
#include "color_data.h"

namespace jnsc::hexa_amp {
void HexaAmp::prepare(size_t newMaxBlockSize, float newSampleRate) {
    using Response = BiquadFilter<float>::Response;

    // Store global parameters
    previousPowerAmpOutput = 0.0f;
    sampleRate = utils::detail::clampSampleRate(newSampleRate);
    preAmpCompressorThresholdMinDb = SQUISH_THRESHOLD_MIN_DB;
    preAmpCompressorThresholdMaxDb = SQUISH_THRESHOLD_MAX_DB;
    powerAmpCompressorThresholdMinDb = SQUISH_THRESHOLD_MIN_DB;
    powerAmpCompressorThresholdMaxDb = SQUISH_THRESHOLD_MAX_DB;
    makeupDb = MAKEUP_DEFAULT_DB;

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

    // Prepare interstage highpass, lowpass, and mid boost
    interstageFilter.prepare(1, OVERSAMPLING_FACTOR * sampleRate, 3);
    interstageFilter.section(0).setResponse(Response::Highpass);
    interstageFilter.section(0).setFrequency(Frequency<float>::Hertz(INTERSTAGE_HIGHPASS_CUTOFF_HZ));
    interstageFilter.section(0).setQ(INTERSTAGE_HIGHPASS_Q);
    interstageFilter.section(1).setResponse(Response::Lowpass);
    interstageFilter.section(1).setFrequency(Frequency<float>::Hertz(INTERSTAGE_LOWPASS_CUTOFF_HZ));
    interstageFilter.section(1).setQ(INTERSTAGE_LOWPASS_Q);
    interstageFilter.section(2).setResponse(Response::Peak);
    interstageFilter.section(2).setQ(MID_BOOST_Q);
    interstageFilter.section(2).setFrequency(Frequency<float>::Hertz(MID_BOOST_FREQUENCY_HZ));

    cabinetFilter.prepare(1, sampleRate, 13);
    for (size_t i = 0; i < 8; ++i)
        cabinetFilter.section(i).setResponse(Response::Peak);
    cabinetFilter.section(8).setResponse(Response::Highpass);
    cabinetFilter.section(8).setQ(color_data::highpassQ);
    for (size_t i = 9; i < 11; ++i) {
        cabinetFilter.section(i).setResponse(Response::Highshelf);
        cabinetFilter.section(i).setQ(color_data::shelfQ);
    }
    cabinetFilter.section(11).setResponse(Response::Lowpass);
    cabinetFilter.section(11).setQ(color_data::lowpassQ[0]);
    cabinetFilter.section(12).setResponse(Response::Lowpass);
    cabinetFilter.section(12).setQ(color_data::lowpassQ[1]);

    // Prepare envelope follower
    preAmpCompressorEnvelopeFollower.prepare(1, OVERSAMPLING_FACTOR * sampleRate);
    preAmpCompressorEnvelopeFollower.setAttackTime(Time<float>::Milliseconds(PRE_AMP_COMPRESSOR_ATTACK_MS), true);
    preAmpCompressorEnvelopeFollower.setReleaseTime(Time<float>::Milliseconds(PRE_AMP_COMPRESSOR_RELEASE_MS), true);

    preAmpCompressorGainComputer.prepare(1, OVERSAMPLING_FACTOR * sampleRate);
    preAmpCompressorGainComputer.setRatio(SQUISH_RATIO, true);
    preAmpCompressorGainComputer.setKnee(SQUISH_KNEE_DB, true);
    preAmpCompressorGainComputer.setControlSmoothingTime(Time<float>::Milliseconds(PARAM_SMOOTH_TIME_MS));

    powerAmpCompressorEnvelopeFollower.prepare(1, OVERSAMPLING_FACTOR * sampleRate);
    powerAmpCompressorEnvelopeFollower.setAttackTime(Time<float>::Milliseconds(POWER_AMP_COMPRESSOR_ATTACK_MS), true);
    powerAmpCompressorEnvelopeFollower.setReleaseTime(Time<float>::Milliseconds(POWER_AMP_COMPRESSOR_RELEASE_MS), true);
    powerAmpCompressorGainComputer.prepare(1, OVERSAMPLING_FACTOR * sampleRate);
    powerAmpCompressorGainComputer.setRatio(SQUISH_RATIO, true);
    powerAmpCompressorGainComputer.setKnee(SQUISH_KNEE_DB, true);
    powerAmpCompressorGainComputer.setControlSmoothingTime(Time<float>::Milliseconds(PARAM_SMOOTH_TIME_MS));

    // SET DSP PARAM SMOOTHING
    drive.prepare(1, OVERSAMPLING_FACTOR * sampleRate);
    level.prepare(1, OVERSAMPLING_FACTOR * sampleRate);
    makeupGain.prepare(1, OVERSAMPLING_FACTOR * sampleRate);
    powerAmpShape.prepare(1, OVERSAMPLING_FACTOR * sampleRate);
    powerAmpShape.setBounds(POWER_AMP_SHAPE_MIN, POWER_AMP_SHAPE_MAX);
    drive.setSmoothingTime(Time<float>::Milliseconds(PARAM_SMOOTH_TIME_MS));
    level.setSmoothingTime(Time<float>::Milliseconds(PARAM_SMOOTH_TIME_MS));
    makeupGain.setSmoothingTime(Time<float>::Milliseconds(PARAM_SMOOTH_TIME_MS));
    powerAmpShape.setSmoothingTime(Time<float>::Milliseconds(PARAM_SMOOTH_TIME_MS));

    // Set Default Parameters
    setDrive(50.0, true);  // 50% drive
    setBite(50.0, true);   // 50% bite
    setSquish(0.0, true);  // No compression
    setLevel(50.0, true);  // 50% level
    setColor(50.0, true);  // 50% color
    setPowerAmpShape(POWER_AMP_SHAPE_DEFAULT, true);
    setMidBoostFrequency(MID_BOOST_FREQUENCY_HZ, true);
    setTilt(50.0, true);   // 50% body
}

void HexaAmp::reset() {
    previousPowerAmpOutput = 0.0f;
    preFilter.reset();
    oversampler.reset();
    interstageFilter.reset();
    cabinetFilter.reset();
    preAmpCompressorEnvelopeFollower.reset();
    powerAmpCompressorEnvelopeFollower.reset();
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

        float peak = preAmpCompressorEnvelopeFollower.processSample(0, in);

        // Apply pre-amp non linearity
        float preAmpOutput = waveshaper.processSample(PRE_AMP_GAIN_LIN * in + PRE_AMP_BIAS, PRE_AMP_SHAPE) -
                             waveshaper.processSample(PRE_AMP_BIAS, PRE_AMP_SHAPE);

        // Interstage filtering
        float interstageOutput = interstageFilter.processSample(0, preAmpOutput);

        float driveGain = drive.getNextValue(0);
        float rms = powerAmpCompressorEnvelopeFollower.processSample(0, previousPowerAmpOutput);
        float fastReductionDb = preAmpCompressorGainComputer.processSample(0, peak);
        float slowReductionDb = powerAmpCompressorGainComputer.processSample(0, rms);

        float compressed = interstageOutput * utils::dB2Mag(fastReductionDb);
        compressed *= driveGain;
        compressed *= utils::dB2Mag(slowReductionDb);
        compressed *= makeupGain.getNextValue(0);

        // Process the power stage
        float powerAmpIn = compressed;
        float shape = powerAmpShape.getNextValue(0);
        float powerAmpOutput = waveshaper.processSample(powerAmpIn, shape);
        previousPowerAmpOutput = powerAmpOutput;
        oversampledBuffer[0][n] = powerAmpOutput;

        // Apply level gain
        oversampledBuffer[0][n] *= level.getNextValue(0);
    }

    // Downsample
    oversampler.downsample(oversampledBuffer.readPtrs(), outputs, numSamples);

    // Post-filter processing
    cabinetFilter.processBlock(outputs, outputs, numSamples);
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
    interstageFilter.section(2).setGain(Gain<float>::Decibels(biteDb));
}

void HexaAmp::setSquish(float squishPercent, bool skipSmoothing) {
    squishAmount = std::clamp(squishPercent * 0.01f, 0.0f, 1.0f);
    updateDynamicsThresholds(skipSmoothing);
    setMakeup(makeupDb, skipSmoothing);
}

void HexaAmp::setPreAmpCompressorThresholdMin(float decibels, bool skipSmoothing) {
    preAmpCompressorThresholdMinDb = std::clamp(decibels, -60.0f, 0.0f);
    updateDynamicsThresholds(skipSmoothing);
}

void HexaAmp::setPreAmpCompressorThresholdMax(float decibels, bool skipSmoothing) {
    preAmpCompressorThresholdMaxDb = std::clamp(decibels, -60.0f, 0.0f);
    updateDynamicsThresholds(skipSmoothing);
}

void HexaAmp::updateDynamicsThresholds(bool skipSmoothing) {
    float low = std::min(preAmpCompressorThresholdMinDb, preAmpCompressorThresholdMaxDb);
    float high = std::max(preAmpCompressorThresholdMinDb, preAmpCompressorThresholdMaxDb);
    float thresholdDb = high + (low - high) * squishAmount;
    preAmpCompressorGainComputer.setThreshold(thresholdDb, skipSmoothing);
    float powerAmpCompressorLow = std::min(powerAmpCompressorThresholdMinDb, powerAmpCompressorThresholdMaxDb);
    float powerAmpCompressorHigh = std::max(powerAmpCompressorThresholdMinDb, powerAmpCompressorThresholdMaxDb);
    powerAmpCompressorGainComputer.setThreshold(powerAmpCompressorHigh + (powerAmpCompressorLow - powerAmpCompressorHigh) * squishAmount, skipSmoothing);
}

void HexaAmp::setLevel(float gainPercent, bool skipSmoothing) {
    // Implementation for setting level
    float t = std::clamp(gainPercent * 0.01f, 0.0f, 1.0f);
    level.setTarget(t * t, skipSmoothing);
}

void HexaAmp::setPreAmpCompressorAttack(float milliseconds, bool skipSmoothing) {
    preAmpCompressorEnvelopeFollower.setAttackTime(Time<float>::Milliseconds(std::clamp(milliseconds, 0.1f, 100.0f)), skipSmoothing);
}

void HexaAmp::setPreAmpCompressorRelease(float milliseconds, bool skipSmoothing) {
    preAmpCompressorEnvelopeFollower.setReleaseTime(Time<float>::Milliseconds(std::clamp(milliseconds, 5.0f, 1500.0f)), skipSmoothing);
}

void HexaAmp::setPowerAmpCompressorAttack(float milliseconds, bool skipSmoothing) {
    powerAmpCompressorEnvelopeFollower.setAttackTime(Time<float>::Milliseconds(std::clamp(milliseconds, 0.1f, 200.0f)), skipSmoothing);
}

void HexaAmp::setPowerAmpCompressorRelease(float milliseconds, bool skipSmoothing) {
    powerAmpCompressorEnvelopeFollower.setReleaseTime(Time<float>::Milliseconds(std::clamp(milliseconds, 5.0f, 2000.0f)), skipSmoothing);
}

void HexaAmp::setMidBoostFrequency(float hertz, bool /*skipSmoothing*/) {
    float frequency = std::clamp(hertz, 200.0f, 3000.0f);
    interstageFilter.section(2).setFrequency(Frequency<float>::Hertz(frequency));
}

void HexaAmp::setPowerAmpCompressorThresholdMin(float decibels, bool skipSmoothing) {
    powerAmpCompressorThresholdMinDb = std::clamp(decibels, -60.0f, 0.0f);
    updateDynamicsThresholds(skipSmoothing);
}

void HexaAmp::setPowerAmpCompressorThresholdMax(float decibels, bool skipSmoothing) {
    powerAmpCompressorThresholdMaxDb = std::clamp(decibels, -60.0f, 0.0f);
    updateDynamicsThresholds(skipSmoothing);
}

void HexaAmp::setMakeup(float decibels, bool skipSmoothing) {
    makeupDb = std::clamp(decibels, 0.0f, 48.0f);
    makeupGain.setTarget(utils::dB2Mag(makeupDb * squishAmount), skipSmoothing);
}

void HexaAmp::setPowerAmpShape(float shape, bool skipSmoothing) {
    powerAmpShape.setTarget(std::clamp(shape, POWER_AMP_SHAPE_MIN, POWER_AMP_SHAPE_MAX), skipSmoothing);
}

void HexaAmp::setColor(float colorPercent, bool /*skipSmoothing*/) {
    const float t = std::clamp(colorPercent * 0.01f, 0.0f, 1.0f);
    const auto gain = [t](float a, float b) { return a + t * (b - a); };
    const auto geometric = [t](float a, float b) {
        return std::exp(std::log(a) + t * (std::log(b) - std::log(a)));
    };
    for (size_t i = 0; i < 8; ++i) {
        const auto& p = color_data::peaks[i];
        cabinetFilter.section(i).setGain(Gain<float>::Decibels(gain(p[0], p[1])));
        cabinetFilter.section(i).setFrequency(Frequency<float>::Hertz(geometric(p[2], p[3])));
        cabinetFilter.section(i).setQ(geometric(p[4], p[5]));
    }
    cabinetFilter.section(8).setFrequency(Frequency<float>::Hertz(
        geometric(color_data::highpassFrequencyHz[0], color_data::highpassFrequencyHz[1])));
    const float shelfFrequency = geometric(color_data::shelfFrequencyHz[0], color_data::shelfFrequencyHz[1]);
    const float shelfGain = gain(color_data::shelfGainDb[0], color_data::shelfGainDb[1]);
    for (size_t i = 9; i < 11; ++i) {
        cabinetFilter.section(i).setFrequency(Frequency<float>::Hertz(shelfFrequency));
        cabinetFilter.section(i).setGain(Gain<float>::Decibels(shelfGain));
    }
    const float lowpassFrequency = geometric(color_data::lowpassFrequencyHz[0], color_data::lowpassFrequencyHz[1]);
    cabinetFilter.section(11).setFrequency(Frequency<float>::Hertz(lowpassFrequency));
    cabinetFilter.section(12).setFrequency(Frequency<float>::Hertz(lowpassFrequency));

}

void HexaAmp::setTilt(float tiltPercent, bool skipSmoothing) {
    // Implementation for setting body
    float tiltDb = PRE_TILT_MIN_DB + (PRE_TILT_MAX_DB - PRE_TILT_MIN_DB) * (tiltPercent * 0.01);
    preFilter.section(1).setGain(Gain<float>::Decibels(tiltDb));
}

} // namespace jnsc::hexa_amp
