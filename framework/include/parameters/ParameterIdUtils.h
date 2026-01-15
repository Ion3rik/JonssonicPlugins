// Jonssonic Plugin Framework
// Utility for converting enum parameter IDs to string IDs
// SPDX-License-Identifier: MIT

#pragma once

#include <juce_core/juce_core.h>
#include <type_traits>

namespace jnsc::juce_interface {

/**
 * @brief Converts an enum parameter ID to a string (e.g., "param_0")
 *
 * Usage: idToString(MyEnum::Param)
 *
 * This works for any enum class with underlying int values.
 */

// Only enable idToString for enum types
template <typename EnumType, std::enable_if_t<std::is_enum_v<EnumType>, int> = 0>
inline juce::String idToString(EnumType id) {
    return "param_" + juce::String(static_cast<int>(id));
}

} // namespace jnsc::juce_interface
