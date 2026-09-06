#include "game/input/mapping.h"

namespace {
    const std::vector<Action> EMPTY_ACTIONS{};

    auto hash_combine(std::size_t seed, std::size_t value) noexcept -> std::size_t {
        return seed ^ (value + 0x9e3779b97f4a7c15ULL + (seed << 6U) + (seed >> 2U));
    }
}

auto PressInputPairHash::operator()(const PressInputPair& mapping) const noexcept -> std::size_t {
    const auto& [press, input_pair] = mapping;
    const auto& [primary, chord]    = input_pair;

    auto seed = std::hash<Press>{}(press);
    seed      = hash_combine(seed, std::hash<Input>{}(primary));
    if (chord.has_value()) {
        seed = hash_combine(seed, std::hash<bool>{}(true));
        seed = hash_combine(seed, std::hash<Input>{}(*chord));
    } else {
        seed = hash_combine(seed, std::hash<bool>{}(false));
    }
    return seed;
}

auto ActionMap::action_from_scancode(SDL_Scancode) const -> const std::vector<Action>& {
    return EMPTY_ACTIONS;
}

auto ActionMap::action_from_keycode(SDL_Keycode) const -> const std::vector<Action>& {
    return EMPTY_ACTIONS;
}

auto ActionMap::action_from_mouse_button(std::uint8_t) const -> const std::vector<Action>& {
    return EMPTY_ACTIONS;
}

auto ActionMap::action_from_mouse_motion(float, float) const -> const std::vector<Action>& {
    return EMPTY_ACTIONS;
}

auto ActionMap::action_from_gamepad_button(InputId, SDL_GamepadButton) const -> const std::vector<Action>& {
    return EMPTY_ACTIONS;
}

auto ActionMap::action_from_gamepad_axis(InputId, SDL_GamepadAxis) const -> const std::vector<Action>& {
    return EMPTY_ACTIONS;
}

auto ActionMap::map_scancode(SDL_Scancode, Action) -> void {
}

auto ActionMap::map_keycode(SDL_Keycode, Action) -> void {
}

auto ActionMap::map_mouse_button(std::uint8_t, Action) -> void {
}

auto ActionMap::map_mouse_motion(MouseAxis, Action) -> void {
}

auto ActionMap::map_gamepad_button(InputId, SDL_GamepadButton, Action) -> void {
}

auto ActionMap::map_gamepad_axis(InputId, SDL_GamepadAxis, Action) -> void {
}

auto ActionMap::load_from_file(engine::fileio::File&) -> ActionMap {
    return ActionMap{};
}

auto ActionMap::save_to_file(engine::fileio::File&) -> void {
}
