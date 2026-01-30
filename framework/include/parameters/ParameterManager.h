// Jonssonic Plugin Framework
// Parameter manager with lock-free FIFO and APVTS integration
// Design inspired by mrta_utils from João Rossi's Modern Real-Time Audio (2025)
// https://github.com/joaorossi/modern-real-time-audio-2025/tree/main/modules/mrta_utils
// SPDX-License-Identifier: MIT

#pragma once

#include "ParameterIdUtils.h"
#include "ParameterSet.h"
#include <juce_audio_processors/juce_audio_processors.h>

#include <functional>
#include <juce_data_structures/juce_data_structures.h>
#include <memory>
#include <sstream>
#include <unordered_map>

#include <vector>

namespace jnsc::juce_interface {

/**
 * @brief Central parameter management system
 *
 * Features:
 * - Wraps JUCE AudioProcessorValueTreeState
 * - Lock-free FIFO for GUI → Audio thread communication
 * - Event-based callbacks for parameter changes
 * - Automatic state save/load
 *
 * Usage:
 *   ParameterManager<ParamID> paramManager(createParams(), processor);
 *   paramManager.on(ParamID::Rate, [](float v, bool skip) { flanger.setRate(v, skip); });
 *
 *   // In processBlock:
 *   paramManager.update();  // Pulls from FIFO, triggers callbacks
 */

// ==============================================================================
// ParameterManager class
// ==============================================================================
template <typename IDType>
class ParameterManager : public juce::AudioProcessorValueTreeState::Listener {
  public:
    /**
     * @brief Callback type for parameter changes
     * @param value New normalized value [0, 1]
     * @param skipSmoothing If true, DSP setter should skip smoothing
     */
    using Callback = std::function<void(float, bool)>;

    /**
     * @brief Construct parameter manager
     * @param params Parameter definitions
     * @param processor Audio processor instance
     */
    ParameterManager(const ParameterSet<IDType>& params, juce::AudioProcessor& processor);

    /**
     * @brief Destructor
     */
    ~ParameterManager();

    /**
     * @brief Register a callback for parameter changes
     * @param id Parameter ID
     * @param callback Function to call when parameter changes (on audio thread)
     */
    void on(IDType id, Callback callback);

    /**
     * @brief Update parameters from FIFO (call once per processBlock)
     *
     * Pulls pending parameter changes from the lock-free FIFO and
     * triggers registered callbacks on the audio thread.
     */
    void update();

    /**
     * @brief Sync all parameters to DSP (call in prepareToPlay)
     *
     * Triggers all registered callbacks with current APVTS values.
     * Use this to initialize DSP with parameter defaults.
     *
     * @param skipSmoothing If true, DSP setters will skip smoothing (recommended for
     * initialization)
     */
    void syncAll(bool skipSmoothing = true);

    /**
     * @brief Get current parameter value
     * @param id Parameter ID
     * @return Current normalized value [0, 1]
     */
    float getValue(IDType id) const;

    /**
     * @brief Get parameter value in native range
     * @param id Parameter ID
     * @return Value in parameter's native range (e.g., Hz, dB, etc.)
     */
    float getNativeValue(IDType id) const;

    /**
     * @brief Set parameter value (thread-safe, from GUI)
     * @param id Parameter ID
     * @param value Normalized value [0, 1]
     */
    void setValue(IDType id, float value);

    /**
     * @brief Save plugin state to memory block
     * @param destData Destination memory block
     */
    void saveState(juce::MemoryBlock& destData) const;

    /**
     * @brief Load plugin state from memory block
     * @param data Source data
     * @param sizeInBytes Size of data
     */
    void loadState(const void* data, int sizeInBytes);

    /**
     * @brief Get underlying APVTS (for GUI attachments)
     */
    juce::AudioProcessorValueTreeState& getAPVTS() { return *apvts; }
    const juce::AudioProcessorValueTreeState& getAPVTS() const { return *apvts; }

  private:
    // Store parameter IDs for listener management
    std::vector<juce::String> registeredParamIDs;

    // Struct for parameter change events
    struct ParameterChange {
        IDType id;
        float value;
    };

    // Create APVTS from ParameterSet
    void createAPVTS(const ParameterSet<IDType>& params, juce::AudioProcessor& processor);

    // AudioProcessorValueTreeState::Listener override
    void parameterChanged(const juce::String& parameterID, float newValue) override;

    // Create JUCE parameter from ParamVariant
    std::unique_ptr<juce::RangedAudioParameter>
    createJuceParameter(const typename ParameterSet<IDType>::ParamVariant& param);

    std::unique_ptr<juce::AudioProcessorValueTreeState> apvts;            // Underlying APVTS
    juce::AbstractFifo fifo{128};                                         // Lock-free FIFO for parameter changes
    std::vector<ParameterChange> fifoBuffer;                              // FIFO buffer
    std::unordered_map<IDType, Callback> callbacks;                       // Registered callbacks
    std::unordered_map<IDType, juce::RangedAudioParameter*> parameterMap; // ID to parameter map
};

//==============================================================================
// Implementation
//==============================================================================

template <typename IDType>
ParameterManager<IDType>::ParameterManager(const ParameterSet<IDType>& params, juce::AudioProcessor& processor) {
    fifoBuffer.resize(128);
    createAPVTS(params, processor);

    // Register as listener for all parameters
    for (auto* param : apvts->processor.getParameters()) {
        if (auto* paramWithID = dynamic_cast<juce::AudioProcessorParameterWithID*>(param)) {
            apvts->addParameterListener(paramWithID->paramID, this);
            registeredParamIDs.push_back(paramWithID->paramID);
        }
    }
}

// Unregister listeners in destructor
template <typename IDType>
ParameterManager<IDType>::~ParameterManager() {
    if (apvts) {
        for (const auto& paramID : registeredParamIDs) {
            apvts->removeParameterListener(paramID, this);
        }
    }
}
// Listener callback: push changes into FIFO for audio thread
template <typename IDType>
void ParameterManager<IDType>::parameterChanged(const juce::String& parameterID, float newValue) {
    // Find enum ID from string
    for (const auto& [id, param] : parameterMap) {
        if (param->paramID == parameterID) {
            int start1, size1, start2, size2;
            fifo.prepareToWrite(1, start1, size1, start2, size2);
            if (size1 > 0) {
                fifoBuffer[static_cast<size_t>(start1)] = {id, newValue};
                fifo.finishedWrite(1);
            }
            break;
        }
    }
}

template <typename IDType>
void ParameterManager<IDType>::on(IDType id, Callback callback) {
    callbacks[id] = std::move(callback);
}

template <typename IDType>
void ParameterManager<IDType>::update() {
    const int numReady = fifo.getNumReady();
    if (numReady == 0)
        return;

    int start1, size1, start2, size2;
    fifo.prepareToRead(numReady, start1, size1, start2, size2);

    // Process first block
    for (int i = 0; i < size1; ++i) {
        const auto& change = fifoBuffer[static_cast<size_t>(start1 + i)];
        DBG("[ParameterManager] update: id=" + idToString(change.id) + ", value=" + juce::String(change.value));
        auto it = callbacks.find(change.id);
        if (it != callbacks.end()) {
            it->second(change.value, false); // Real-time changes use smoothing
        }
    }

    // Process second block (if wrapped)
    for (int i = 0; i < size2; ++i) {
        const auto& change = fifoBuffer[static_cast<size_t>(start2 + i)];
        DBG("[ParameterManager] update: id=" + idToString(change.id) + ", value=" + juce::String(change.value));
        auto it = callbacks.find(change.id);
        if (it != callbacks.end()) {
            it->second(change.value, false); // Real-time changes use smoothing
        }
    }

    fifo.finishedRead(size1 + size2);
}

template <typename IDType>
void ParameterManager<IDType>::syncAll(bool skipSmoothing) {
    for (const auto& [id, callback] : callbacks) {
        callback(getNativeValue(id), skipSmoothing);
    }
}

template <typename IDType>
float ParameterManager<IDType>::getValue(IDType id) const {
    auto it = parameterMap.find(id);
    if (it != parameterMap.end()) {
        return it->second->getValue();
    }
    return 0.0f;
}

template <typename IDType>
float ParameterManager<IDType>::getNativeValue(IDType id) const {
    auto it = parameterMap.find(id);
    if (it != parameterMap.end()) {
        return it->second->convertFrom0to1(it->second->getValue());
    }
    return 0.0f;
}

template <typename IDType>
void ParameterManager<IDType>::setValue(IDType id, float value) {
    auto it = parameterMap.find(id);
    if (it != parameterMap.end()) {
        it->second->setValueNotifyingHost(value);

        // Push to FIFO for audio thread
        int start1, size1, start2, size2;
        fifo.prepareToWrite(1, start1, size1, start2, size2);

        if (size1 > 0) {
            fifoBuffer[static_cast<size_t>(start1)] = {id, getNativeValue(id)};
            fifo.finishedWrite(1);
        }
    }
}

template <typename IDType>
void ParameterManager<IDType>::saveState(juce::MemoryBlock& destData) const {
    auto state = apvts->copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    juce::AudioProcessor::copyXmlToBinary(*xml, destData);
}

template <typename IDType>
void ParameterManager<IDType>::loadState(const void* data, int sizeInBytes) {
    std::unique_ptr<juce::XmlElement> xml(juce::AudioProcessor::getXmlFromBinary(data, sizeInBytes));
    if (xml && xml->hasTagName(apvts->state.getType())) {
        apvts->replaceState(juce::ValueTree::fromXml(*xml));

        // Trigger callbacks for all loaded values (skip smoothing for instant preset load)
        for (const auto& [id, callback] : callbacks) {
            callback(getNativeValue(id), true); // Skip smoothing when loading state
        }
    }
}

template <typename IDType>
void ParameterManager<IDType>::createAPVTS(const ParameterSet<IDType>& params, juce::AudioProcessor& processor) {
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    for (const auto& paramVariant : params.getAll()) {
        auto juceParam = createJuceParameter(paramVariant);

        // Store parameter pointer for quick access
        std::visit(
            [this](auto&& param) {
                parameterMap[param.id] = nullptr; // Will be set after APVTS creation
            },
            paramVariant);

        layout.add(std::move(juceParam));
    }

    apvts = std::make_unique<juce::AudioProcessorValueTreeState>(processor, nullptr, "Parameters", std::move(layout));

    // Now populate parameter map with actual pointers
    for (const auto& paramVariant : params.getAll()) {
        std::visit(
            [this](auto&& param) {
                auto* p = apvts->getParameter(idToString(param.id));
                if (p) {
                    parameterMap[param.id] = p;
                }
            },
            paramVariant);
    }
}

template <typename IDType>
std::unique_ptr<juce::RangedAudioParameter>
ParameterManager<IDType>::createJuceParameter(const typename ParameterSet<IDType>::ParamVariant& paramVariant) {

    return std::visit(
        [this](auto&& param) -> std::unique_ptr<juce::RangedAudioParameter> {
            using T = std::decay_t<decltype(param)>;
            // Use juce::String everywhere for parameter IDs
            const juce::String juceIdStr = idToString(param.id);

            if constexpr (std::is_same_v<T, FloatParam<IDType>>) {
                return std::make_unique<juce::AudioParameterFloat>(
                    juce::ParameterID{juceIdStr, 1},
                    param.name,
                    juce::NormalisableRange<float>(param.min, param.max, 0.01f, param.skew),
                    param.defaultValue,
                    param.unit);
            } else if constexpr (std::is_same_v<T, IntParam<IDType>>) {
                return std::make_unique<juce::AudioParameterInt>(juce::ParameterID{juceIdStr, 1},
                                                                 param.name,
                                                                 param.min,
                                                                 param.max,
                                                                 param.defaultValue,
                                                                 param.unit);
            } else if constexpr (std::is_same_v<T, BoolParam<IDType>>) {
                return std::make_unique<juce::AudioParameterBool>(juce::ParameterID{juceIdStr, 1},
                                                                  param.name,
                                                                  param.defaultValue);
            } else if constexpr (std::is_same_v<T, ChoiceParam<IDType>>) {
                juce::StringArray choices;
                for (const auto& choice : param.choices) {
                    choices.add(choice);
                }
                return std::make_unique<juce::AudioParameterChoice>(juce::ParameterID{juceIdStr, 1},
                                                                    param.name,
                                                                    choices,
                                                                    param.defaultIndex);
            }
        },
        paramVariant);
}

} // namespace jnsc::juce_interface
