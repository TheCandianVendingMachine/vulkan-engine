#include "game/input/types.h"

#include <cstddef>
#include <functional>

namespace {
    auto hash_combine(std::size_t seed, std::size_t value) noexcept -> std::size_t {
        return seed ^ (value + 0x9e3779b97f4a7c15ULL + (seed << 6U) + (seed >> 2U));
    }

    auto hash_input_event(const Input& input) noexcept -> std::size_t {
        switch (input.type) {
            case InputType::SCANCODE:
                return std::hash<int>{}(static_cast<int>(input.event.scancode));
            case InputType::KEYCODE:
                return std::hash<SDL_Keycode>{}(input.event.keycode);
            case InputType::MOUSE_WHEEL:
                return std::hash<float>{}(input.event.wheel.magnitude);
            case InputType::MOUSE_BUTTON:
                return std::hash<std::uint8_t>{}(input.event.button);
            case InputType::MOUSE_AXIS:
                return std::hash<int>{}(static_cast<int>(input.event.axis));
            case InputType::GAMEPAD_BUTTON:
                return std::hash<int>{}(static_cast<int>(input.event.gamepad_button));
            case InputType::GAMEPAD_AXIS:
                return std::hash<int>{}(static_cast<int>(input.event.gamepad_axis));
        }
        return 0;
    }
}

auto operator==(const Input& lhs, const Input& rhs) -> bool {
    if (lhs.input_id != rhs.input_id || lhs.type != rhs.type) {
        return false;
    }

    switch (lhs.type) {
        case InputType::SCANCODE:
            return lhs.event.scancode == rhs.event.scancode;
        case InputType::KEYCODE:
            return lhs.event.keycode == rhs.event.keycode;
        case InputType::MOUSE_WHEEL:
            return lhs.event.wheel.magnitude == rhs.event.wheel.magnitude;
        case InputType::MOUSE_BUTTON:
            return lhs.event.button == rhs.event.button;
        case InputType::MOUSE_AXIS:
            return lhs.event.axis == rhs.event.axis;
        case InputType::GAMEPAD_BUTTON:
            return lhs.event.gamepad_button == rhs.event.gamepad_button;
        case InputType::GAMEPAD_AXIS:
            return lhs.event.gamepad_axis == rhs.event.gamepad_axis;
    }
    return false;
}

auto std::hash<Input>::operator()(const Input& input) const noexcept -> std::size_t {
    auto seed = std::hash<InputId>{}(input.input_id);
    seed      = hash_combine(seed, std::hash<InputType>{}(input.type));
    seed      = hash_combine(seed, hash_input_event(input));
    return seed;
}
