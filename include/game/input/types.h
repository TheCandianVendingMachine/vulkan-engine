#pragma once
#include <SDL3/SDL_events.h>
#include <engine/newtype.h>

#include <cstddef>
#include <cstdint>

struct InputId : engine::NewType<InputId, std::size_t>, engine::Hashable<InputId>, engine::Eq<InputId> {
        using NewType::NewType;
};

struct Action : engine::NewType<Action, const char*> {
        using NewType::NewType;
};

enum class Press {
    DOWN,
    UP,
    HELD,
    DOUBLE
};

enum class MouseAxis : std::uint8_t {
    X,
    Y
};

struct MouseWheel {
        float magnitude = 0.f;
};

enum class InputType {
    SCANCODE,
    KEYCODE,
    MOUSE_WHEEL,
    MOUSE_BUTTON,
    MOUSE_AXIS,
    GAMEPAD_BUTTON,
    GAMEPAD_AXIS
};

struct Input {
        InputId input_id = InputId(0);
        union {
                SDL_Scancode scancode;
                SDL_Keycode keycode;
                MouseWheel wheel;
                std::uint8_t button;
                MouseAxis axis;
                SDL_GamepadButton gamepad_button;
                SDL_GamepadAxis gamepad_axis;
        } event;
        InputType type;
};

auto operator==(const Input& lhs, const Input& rhs) -> bool;

namespace std {
    template <class Key>
    struct hash;

    template <>
    struct hash<InputId> : engine::Hashable<InputId> {};

    template <>
    struct hash<Input> {
            auto operator()(const Input& input) const noexcept -> std::size_t;
    };
} // namespace std
