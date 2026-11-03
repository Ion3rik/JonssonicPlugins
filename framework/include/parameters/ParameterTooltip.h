#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <utility>

namespace jnsc::juce_interface {

class ParameterTooltip {
  public:
    virtual ~ParameterTooltip() = default;
    virtual const juce::String& getTooltipText() const = 0;
};

template <typename Parameter>
class ParameterWithTooltip final : public Parameter, public ParameterTooltip {
  public:
    template <typename... Args>
    ParameterWithTooltip(juce::String text, Args&&... args)
        : Parameter(std::forward<Args>(args)...), tooltipText(std::move(text)) {}

    const juce::String& getTooltipText() const override { return tooltipText; }

  private:
    juce::String tooltipText;
};

inline juce::String getParameterTooltip(const juce::AudioProcessorParameter& parameter) {
    if (const auto* described = dynamic_cast<const ParameterTooltip*>(&parameter))
        return described->getTooltipText();
    return {};
}

} // namespace jnsc::juce_interface
