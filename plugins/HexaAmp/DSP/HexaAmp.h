// Jonssonic H E X A - A M P
// Abstract amp-simulation processor for the HexaAmp plugin.
// SPDX-License-Identifier: MIT

#pragma once
#include <jonssonic/core/common/audio_buffer.h>
#include <jonssonic/core/common/dsp_param.h>
#include <jonssonic/core/common/quantities.h>
#include <jonssonic/core/dynamics/envelope_follower.h>
#include <jonssonic/core/dynamics/gain_computer.h>
#include <jonssonic/core/filters/biquad_filter.h>
#include <jonssonic/core/nonlinear/wave_shaper.h>
#include <jonssonic/core/oversampling/oversampler.h>
#include <jonssonic/utils/buffer_utils.h>
#include <jonssonic/utils/math_utils.h>
namespace jnsc::hexa_amp {
/**
 * @brief Abstract Amp-simulation processor for the HexaAmp plugin.
 */
class HexaAmp {
    /**
     * @brief Tunable constants for the HexaAmp.
     * @param OVERSAMPLING_FACTOR Oversampling factor for waveshaping.
     * @param PARAM_SMOOTH_TIME_MS Smoothing time for parameter changes in milliseconds.
     * @param SHAPE_MIN Minimum shape parameter value (soft clipping).
     * @param SHAPE_MAX Maximum shape parameter value (hard clipping).
     * @param HIGH_PASS_CUTOFF_HZ Cutoff frequency for highpass filters.
     */

    // GENERAL
    static constexpr size_t OVERSAMPLING_FACTOR = 8;
    static constexpr float PARAM_SMOOTH_TIME_MS = 50.0;

    // PRE FILTER
    static constexpr float PRE_HIGH_PASS_CUTOFF_HZ = 50.0;

    static constexpr float PRE_TILT_FREQUENCY_HZ = 800.0;
    static constexpr float PRE_TILT_MIN_DB = -9.0;
    static constexpr float PRE_TILT_MAX_DB = 9.0;
    static constexpr float PRE_TILT_Q = 0.3;

    // INTERSTAGE FILTER

    static constexpr float INTERSTAGE_HIGHPASS_CUTOFF_HZ = 50.0f;
    static constexpr float INTERSTAGE_HIGHPASS_Q = 0.7071068f;
    static constexpr float INTERSTAGE_LOWPASS_CUTOFF_HZ = 15000.0f;
    static constexpr float INTERSTAGE_LOWPASS_Q = 0.7071068f;

    static constexpr float MID_BOOST_FREQUENCY_HZ = 900.0;
    static constexpr float MID_BOOST_Q = 0.7;
    static constexpr float MID_BOOST_MIN_DB = -16.0;
    static constexpr float MID_BOOST_MAX_DB = 16.0;

    static constexpr float PRE_AMP_GAIN_LIN = 1.5;
    static constexpr float PRE_AMP_BIAS = 0.2;
    static constexpr float PRE_AMP_SHAPE = 2.0;

    static constexpr float DRIVE_MIN_DB = 0.0;
    static constexpr float DRIVE_MAX_DB = 32.0;

    static constexpr float POWER_AMP_SHAPE_MIN = 2.0f;
    static constexpr float POWER_AMP_SHAPE_MAX = 20.0f;
    static constexpr float POWER_AMP_SHAPE_DEFAULT = 6.0f;
    // Dynamics
    static constexpr float PRE_AMP_COMPRESSOR_ATTACK_MS = 5.0f;
    static constexpr float PRE_AMP_COMPRESSOR_RELEASE_MS = 120.0f;
    static constexpr float SQUISH_THRESHOLD_MIN_DB = -32.0f;
    static constexpr float SQUISH_THRESHOLD_MAX_DB = -8.0f;
    static constexpr float SQUISH_RATIO = 4.0f;
    static constexpr float SQUISH_KNEE_DB = 6.0f;
    static constexpr float MAKEUP_DEFAULT_DB = 18.0f;
    static constexpr float POWER_AMP_COMPRESSOR_ATTACK_MS = 20.0f;
    static constexpr float POWER_AMP_COMPRESSOR_RELEASE_MS = 250.0f;

  public:
    /// Default constructor.
    HexaAmp() = default;

    /**
     * @brief Parameterized constructor that calls @ref prepare.
     * @param newMaxBlockSize Maximum block size for processing (at base sample rate)
     * @param newSampleRate Sample rate in Hz
     */
    HexaAmp(size_t newMaxBlockSize, float newSampleRate) { prepare(newMaxBlockSize, newSampleRate); }

    /// Default destructor.
    ~HexaAmp() = default;

    /// No copy nor move semantics
    HexaAmp(const HexaAmp&) = delete;
    HexaAmp& operator=(const HexaAmp&) = delete;
    HexaAmp(HexaAmp&&) = delete;
    HexaAmp& operator=(HexaAmp&&) = delete;

    /**
     * @brief Prepare the HexaAmp effect for processing.
     * @param newMaxBlockSize Maximum block size for processing (at base sample rate)
     * @param newSampleRate Sample rate in Hz
     */
    void prepare(size_t newMaxBlockSize, float newSampleRate);

    /**
     * @brief Reset the HexaAmp effect to its initial state.
     */
    void reset();

    /**
     * @brief Process a mono block of samples.
     * @param input Input buffer (numSamples)
     * @param output Output buffer (numSamples)
     * @param numSamples Number of samples to process
     */
    void processBlock(const float* input, float* output, size_t numSamples);

    // SETTERS FOR PARAMETERS

    /**
     * @brief Set drive in percent.
     * @param drivePercent Drive amount in percent.
     * @param skipSmoothing If true, skip smoothing and set immediately.
     */
    void setDrive(float drivePercent, bool skipSmoothing = false);

    /**
     * @brief Set bite amount in percent.
     * @param bitePercent Bite amount in percent.
     * @param skipSmoothing If true, skip smoothing and set immediately.
     */
    void setBite(float bitePercent, bool skipSmoothing = false);

    /**
     * @brief Set squish amount in percent.
     * @param squishPercent Compression amount from 0 to 100 percent.
     * @param skipSmoothing If true, skip smoothing and set immediately.
     */
    void setSquish(float squishPercent, bool skipSmoothing = false);
    void setPreAmpCompressorThresholdMin(float decibels, bool skipSmoothing = false);
    void setPreAmpCompressorThresholdMax(float decibels, bool skipSmoothing = false);

    /**
     * @brief Set output level in percent using a squared linear-gain mapping.
     * @param gainPercent Output gain in percent.
     * @param skipSmoothing If true, skip smoothing and set immediately.
     */
    void setLevel(float gainPercent, bool skipSmoothing = false);

    /**
     * @brief Set color amount in percent.
     * @param colorPercent Color amount in percent.
     * @param skipSmoothing If true, skip smoothing and set immediately.
     */
    void setColor(float colorPercent, bool skipSmoothing = false);
    void setPreAmpCompressorAttack(float milliseconds, bool skipSmoothing = false);
    void setPreAmpCompressorRelease(float milliseconds, bool skipSmoothing = false);
    void setPowerAmpCompressorAttack(float milliseconds, bool skipSmoothing = false);
    void setPowerAmpCompressorRelease(float milliseconds, bool skipSmoothing = false);
    void setMidBoostFrequency(float hertz, bool skipSmoothing = false);
    void setPowerAmpCompressorThresholdMin(float decibels, bool skipSmoothing = false);
    void setPowerAmpCompressorThresholdMax(float decibels, bool skipSmoothing = false);
    void setMakeup(float decibels, bool skipSmoothing = false);
    void setPowerAmpShape(float shape, bool skipSmoothing = false);

    /**
     * @brief Set tilt amount in percent. (0 = darkest, 100 = brightest)
     * @param tilPercent Tilt amount in percent.
     * @param skipSmoothing If true, skip smoothing and set immediately.
     */
    void setTilt(float tiltPercent, bool skipSmoothing = false);

    /// Get number of channels.
    size_t getNumChannels() const { return 1; }

    /// Get sample rate.
    float getSampleRate() const { return sampleRate; }

    /// Get latency in samples at base sample rate.
    size_t getLatencySamples() const { return oversampler.getLatencySamples(); }

  private:
    void updateDynamicsThresholds(bool skipSmoothing);

    // GLOBAL PARAMETERS
    float sampleRate = 0.0f;
    float squishAmount = 0.0f;
    float previousPowerAmpOutput = 0.0f;
    float preAmpCompressorThresholdMinDb = SQUISH_THRESHOLD_MIN_DB;
    float preAmpCompressorThresholdMaxDb = SQUISH_THRESHOLD_MAX_DB;
    float powerAmpCompressorThresholdMinDb = SQUISH_THRESHOLD_MIN_DB;
    float powerAmpCompressorThresholdMaxDb = SQUISH_THRESHOLD_MAX_DB;
    float makeupDb = MAKEUP_DEFAULT_DB;

    // OVERSAMPLED BUFFER
    AudioBuffer<float> oversampledBuffer;

    // PARAMS
    DspParam<float> drive;
    DspParam<float> level;
    DspParam<float> makeupGain;
    DspParam<float> powerAmpShape;

    // FILTERS
    BiquadFilter<float> preFilter;
    BiquadFilter<float> interstageFilter;
    BiquadFilter<float> cabinetFilter;

    // DYNAMICS
    EnvelopeFollower<float, EnvelopeType::Peak> preAmpCompressorEnvelopeFollower;
    GainComputer<float, CompressorPolicy<float>> preAmpCompressorGainComputer;
    EnvelopeFollower<float, EnvelopeType::RMS> powerAmpCompressorEnvelopeFollower;
    GainComputer<float, CompressorPolicy<float>> powerAmpCompressorGainComputer;

    // NON-LINEAR PROCESSING
    Oversampler<float, OVERSAMPLING_FACTOR> oversampler;
    WaveShaper<float, WaveShaperType::Dynamic> waveshaper;
};

} // namespace jnsc::hexa_amp
