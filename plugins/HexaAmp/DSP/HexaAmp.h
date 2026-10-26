// Jonssonic H E X A - A M P
// Abstract amp-simulation processor for the HexaAmp plugin.
// SPDX-License-Identifier: MIT

#pragma once
#include <jonssonic/core/common/audio_buffer.h>
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
     * @param ASYMMETRY_SCALE Scale factor for asymmetry control.
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

    // MID BOOSTER

    static constexpr float MID_BOOST_FREQUENCY_HZ = 900.0;
    static constexpr float MID_BOOST_Q = 0.7;
    static constexpr float MID_BOOST_MIN_DB = -9.0;
    static constexpr float MID_BOOST_MAX_DB = 12.0;

    // POST FILTER
    static constexpr float POST_HIGH_PASS_CUTOFF_HZ = 80.0;

    static constexpr float POST_LOW_MID_MIN_FREQUENCY_HZ = 110.0;
    static constexpr float POST_LOW_MID_MAX_FREQUENCY_HZ = 145.0;
    static constexpr float POST_LOW_MID_MIN_GAIN_DB = 5.0;
    static constexpr float POST_LOW_MID_MAX_GAIN_DB = 7.0;
    static constexpr float POST_LOW_MID_Q = 1.2;

    static constexpr float POST_HIGH_MID_MIN_FREQUENCY_HZ = 2200.0;
    static constexpr float POST_HIGH_MID_MAX_FREQUENCY_HZ = 3300.0;
    static constexpr float POST_HIGH_MID_MIN_GAIN_DB = 6.0;
    static constexpr float POST_HIGH_MID_MAX_GAIN_DB = 8.0;
    static constexpr float POST_HIGH_MID_Q = 1.4;

    static constexpr float POST_LOWPASS_MIN_CUTOFF_HZ = 4500.0;
    static constexpr float POST_LOWPASS_MAX_CUTOFF_HZ = 6000.0;
    static constexpr float POST_LOWPASS_STAGE1_Q = 0.5411961f;
    static constexpr float POST_LOWPASS_STAGE2_Q = 1.3065630f;

    static constexpr float PRE_AMP_GAIN_LIN = 1.5;
    static constexpr float PRE_AMP_BIAS = 0.4;
    static constexpr float PRE_AMP_SHAPE = 2.0;

    static constexpr float DRIVE_MIN_DB = 0.0;
    static constexpr float DRIVE_MAX_DB = 24.0;
    static constexpr float LEVEL_MIN_DB = -60.0;
    static constexpr float LEVEL_MAX_DB = 0;

    static constexpr float POWER_AMP_SHAPE = 14.0;
    static constexpr float POWER_AMP_BIAS_MAX = -0.3;
    static constexpr float POWER_AMP_HEADROOM_MIN = 0.33;

    static constexpr float SQUISH_TH_MIN_DB = -36.0; // dB
    static constexpr float SQUISH_TH_MAX_DB = -12.0; // dB

    // Dynamics
    static constexpr float COMPRESSOR_RATIO = 4.0;         // Ratio for the compressor
    static constexpr float COMPRESSOR_KNEE = 6.0;          // Knee width in dB
    static constexpr float BIAS_MOD_ATTACK_MS = 15.0;      // Attack time in milliseconds
    static constexpr float BIAS_MOD_RELEASE_MS = 250.0;    // Release time in milliseconds
    static constexpr float HEADROOM_MOD_ATTACK_MS = 5.0;   // Attack time in milliseconds
    static constexpr float HEADROOM_MOD_RELEASE_MS = 80.0; // Release time in milliseconds

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
     * @param squishPercent Squish amount in percent.
     * @param skipSmoothing If true, skip smoothing and set immediately.
     */
    void setSquish(float squishPercent, bool skipSmoothing = false);

    /**
     * @brief Set output level in percent.
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
    // GLOBAL PARAMETERS
    float sampleRate = 0.0f;

    // OVERSAMPLED BUFFER
    AudioBuffer<float> oversampledBuffer;

    // PARAMS
    DspParam<float> drive;
    DspParam<float> level;

    // FILTERS
    BiquadFilter<float> preFilter;
    BiquadFilter<float> midBooster;
    BiquadFilter<float> postFilter;

    // DYNAMICS
    EnvelopeFollower<float, EnvelopeType::RMS> biasEnvelopeFollower;
    EnvelopeFollower<float, EnvelopeType::Peak> headroomEnvelopeFollower;
    GainComputer<float, CompressorPolicy<float>> biasGainComputer;
    GainComputer<float, CompressorPolicy<float>> headroomGainComputer;

    // NON-LINEAR PROCESSING
    Oversampler<float, OVERSAMPLING_FACTOR> oversampler;
    WaveShaper<float, WaveShaperType::Dynamic> waveshaper;
};

} // namespace jnsc::hexa_amp
