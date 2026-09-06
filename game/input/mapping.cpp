#include "game/input/mapping.h"

#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_scancode.h>

#include <algorithm>
#include <array>
#include <string>

namespace {
    const std::vector<Action> EMPTY_ACTIONS{};

    auto hash_combine(std::size_t seed, std::size_t value) noexcept -> std::size_t {
        return seed ^ (value + 0x9e3779b97f4a7c15ULL + (seed << 6U) + (seed >> 2U));
    }

    auto input_id(std::size_t id = 0) -> InputId {
        return InputId{id};
    }

    auto scancode_input(SDL_Scancode scancode) -> Input {
        Input input{};
        input.input_id       = input_id();
        input.event.scancode = scancode;
        input.type           = InputType::SCANCODE;
        return input;
    }

    auto keycode_input(SDL_Keycode keycode) -> Input {
        Input input{};
        input.input_id      = input_id();
        input.event.keycode = keycode;
        input.type          = InputType::KEYCODE;
        return input;
    }

    auto mouse_button_input(std::uint8_t button) -> Input {
        Input input{};
        input.input_id     = input_id();
        input.event.button = button;
        input.type         = InputType::MOUSE_BUTTON;
        return input;
    }

    auto mouse_axis_input(MouseAxis axis) -> Input {
        Input input{};
        input.input_id   = input_id();
        input.event.axis = axis;
        input.type       = InputType::MOUSE_AXIS;
        return input;
    }

    auto gamepad_button_input(InputId gamepad_id, SDL_GamepadButton button) -> Input {
        Input input{};
        input.input_id             = gamepad_id;
        input.event.gamepad_button = button;
        input.type                 = InputType::GAMEPAD_BUTTON;
        return input;
    }

    auto gamepad_axis_input(InputId gamepad_id, SDL_GamepadAxis axis) -> Input {
        Input input{};
        input.input_id         = gamepad_id;
        input.event.gamepad_axis = axis;
        input.type             = InputType::GAMEPAD_AXIS;
        return input;
    }

    auto action_name(Action action) -> const char* {
        return static_cast<const char* const&>(action);
    }

    auto quoted(std::string_view value) -> std::string {
        auto out = std::string{"\""};
        for (auto ch : value) {
            if (ch == '\\' || ch == '"') {
                out.push_back('\\');
            }
            out.push_back(ch);
        }
        out.push_back('"');
        return out;
    }

    auto press_name(Press press) -> std::string_view {
        switch (press) {
            case Press::DOWN:
                return "down";
            case Press::UP:
                return "up";
            case Press::HELD:
                return "held";
            case Press::DOUBLE:
                return "double";
        }
        return "down";
    }

    auto mouse_axis_name(MouseAxis axis) -> std::string_view {
        switch (axis) {
            case MouseAxis::X:
                return "X";
            case MouseAxis::Y:
                return "Y";
        }
        return "X";
    }

    auto mouse_button_name(std::uint8_t button) -> std::string_view {
        switch (button) {
            case SDL_BUTTON_LEFT:
                return "Left";
            case SDL_BUTTON_MIDDLE:
                return "Middle";
            case SDL_BUTTON_RIGHT:
                return "Right";
            case SDL_BUTTON_X1:
                return "X1";
            case SDL_BUTTON_X2:
                return "X2";
            default:
                return "Unknown";
        }
    }

    auto input_to_toml(Input input, std::optional<Press> press = std::nullopt) -> std::string {
        auto out = std::string{"{ id = "};
        switch (input.type) {
            case InputType::SCANCODE:
                out += quoted(SDL_GetScancodeName(input.event.scancode));
                break;
            case InputType::KEYCODE:
                out += quoted(SDL_GetKeyName(input.event.keycode));
                break;
            case InputType::MOUSE_BUTTON:
                out += quoted(mouse_button_name(input.event.button));
                break;
            case InputType::MOUSE_AXIS:
                out += quoted(mouse_axis_name(input.event.axis));
                break;
            case InputType::GAMEPAD_BUTTON:
                out += quoted(SDL_GetGamepadStringForButton(input.event.gamepad_button));
                break;
            case InputType::GAMEPAD_AXIS:
                out += quoted(SDL_GetGamepadStringForAxis(input.event.gamepad_axis));
                break;
            case InputType::MOUSE_WHEEL:
                out += quoted("Wheel");
                break;
        }
        if (press.has_value()) {
            out += ", state = ";
            out += quoted(press_name(*press));
        }
        out += " }";
        return out;
    }

    auto input_group(InputType type) -> std::string_view {
        switch (type) {
            case InputType::SCANCODE:
            case InputType::KEYCODE:
                return "keyboard";
            case InputType::MOUSE_WHEEL:
            case InputType::MOUSE_BUTTON:
            case InputType::MOUSE_AXIS:
                return "mouse";
            case InputType::GAMEPAD_BUTTON:
            case InputType::GAMEPAD_AXIS:
                return "gamepad";
        }
        return "keyboard";
    }

    auto input_array_name(InputType type) -> std::string_view {
        switch (type) {
            case InputType::SCANCODE:
                return "scancodes";
            case InputType::KEYCODE:
                return "keycodes";
            case InputType::MOUSE_BUTTON:
            case InputType::GAMEPAD_BUTTON:
                return "button";
            case InputType::MOUSE_AXIS:
            case InputType::GAMEPAD_AXIS:
                return "axis";
            case InputType::MOUSE_WHEEL:
                return "wheel";
        }
        return "inputs";
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

auto ActionMap::action_from_scancode(SDL_Scancode scancode) const -> const std::vector<Action>& {
    auto key = PressInputPair{Press::DOWN, InputPair{scancode_input(scancode), std::nullopt}};
    auto it  = press_mappings_.find(key);
    return it == press_mappings_.end() ? EMPTY_ACTIONS : it->second;
}

auto ActionMap::action_from_keycode(SDL_Keycode keycode) const -> const std::vector<Action>& {
    auto key = PressInputPair{Press::DOWN, InputPair{keycode_input(keycode), std::nullopt}};
    auto it  = press_mappings_.find(key);
    return it == press_mappings_.end() ? EMPTY_ACTIONS : it->second;
}

auto ActionMap::action_from_mouse_button(std::uint8_t button) const -> const std::vector<Action>& {
    auto key = PressInputPair{Press::DOWN, InputPair{mouse_button_input(button), std::nullopt}};
    auto it  = press_mappings_.find(key);
    return it == press_mappings_.end() ? EMPTY_ACTIONS : it->second;
}

auto ActionMap::action_from_mouse_motion(float x_axis, float y_axis) const -> const std::vector<Action>& {
    if (x_axis != 0.f) {
        auto it = axis_mappings_.find(mouse_axis_input(MouseAxis::X));
        return it == axis_mappings_.end() ? EMPTY_ACTIONS : it->second;
    }
    if (y_axis != 0.f) {
        auto it = axis_mappings_.find(mouse_axis_input(MouseAxis::Y));
        return it == axis_mappings_.end() ? EMPTY_ACTIONS : it->second;
    }
    return EMPTY_ACTIONS;
}

auto ActionMap::action_from_gamepad_button(InputId gamepad_id, SDL_GamepadButton button) const -> const std::vector<Action>& {
    auto key = PressInputPair{Press::DOWN, InputPair{gamepad_button_input(gamepad_id, button), std::nullopt}};
    auto it  = press_mappings_.find(key);
    return it == press_mappings_.end() ? EMPTY_ACTIONS : it->second;
}

auto ActionMap::action_from_gamepad_axis(InputId gamepad_id, SDL_GamepadAxis axis) const -> const std::vector<Action>& {
    auto it = axis_mappings_.find(gamepad_axis_input(gamepad_id, axis));
    return it == axis_mappings_.end() ? EMPTY_ACTIONS : it->second;
}

auto ActionMap::map_scancode(SDL_Scancode scancode, Action action) -> void {
    map_press(Press::DOWN, scancode_input(scancode), std::nullopt, action);
}

auto ActionMap::map_keycode(SDL_Keycode keycode, Action action) -> void {
    map_press(Press::DOWN, keycode_input(keycode), std::nullopt, action);
}

auto ActionMap::map_mouse_button(std::uint8_t button, Action action) -> void {
    map_press(Press::DOWN, mouse_button_input(button), std::nullopt, action);
}

auto ActionMap::map_mouse_motion(MouseAxis axis, Action action) -> void {
    map_axis(mouse_axis_input(axis), action);
}

auto ActionMap::map_gamepad_button(InputId gamepad_id, SDL_GamepadButton button, Action action) -> void {
    map_press(Press::DOWN, gamepad_button_input(gamepad_id, button), std::nullopt, action);
}

auto ActionMap::map_gamepad_axis(InputId gamepad_id, SDL_GamepadAxis axis, Action action) -> void {
    map_axis(gamepad_axis_input(gamepad_id, axis), action);
}

auto ActionMap::build() -> ActionMapBuilder {
    return ActionMapBuilder{};
}

auto ActionMap::load_from_file(engine::fileio::File& file) -> ActionMap {
    (void)file;
    return ActionMap{};
}

auto ActionMap::save_to_file(engine::fileio::File& file) const -> void {
    auto output = std::string{};

    auto write_entry = [&](Action action, Input input, std::optional<Press> press, std::optional<Input> chord) {
        output += "[[action.";
        output += action_name(action);
        output += ".";
        output += input_group(input.type);
        output += "]]\n";
        output += input_array_name(input.type);
        output += " = [ ";
        output += input_to_toml(input, press);
        output += " ]\n";
        if (chord.has_value()) {
            output += "chord = ";
            output += input_to_toml(*chord);
            output += "\n";
        }
        output += "\n";
    };

    for (const auto& [input, actions] : axis_mappings_) {
        for (auto action : actions) {
            write_entry(action, input, std::nullopt, std::nullopt);
        }
    }

    for (const auto& [mapping, actions] : press_mappings_) {
        const auto& [press, input_pair] = mapping;
        const auto& [input, chord]      = input_pair;
        for (auto action : actions) {
            write_entry(action, input, press, chord);
        }
    }

    auto buffer = std::vector<char>(output.begin(), output.end());
    (void)file.write_buffer(buffer);
}

auto ActionMap::map_axis(Input input, Action action) -> void {
    axis_mappings_[input].push_back(action);
}

auto ActionMap::map_press(Press press, Input input, std::optional<Input> chord, Action action) -> void {
    press_mappings_[PressInputPair{press, InputPair{input, chord}}].push_back(action);
}

auto ActionMapBuilder::action(Action action) -> ActionMapBuilder& {
    current_action_ = action;
    return *this;
}

auto ActionMapBuilder::chord_scancode(SDL_Scancode scancode) -> ActionMapBuilder& {
    current_chord_ = scancode_input(scancode);
    return *this;
}

auto ActionMapBuilder::chord_keycode(SDL_Keycode keycode) -> ActionMapBuilder& {
    current_chord_ = keycode_input(keycode);
    return *this;
}

auto ActionMapBuilder::chord_mouse_button(std::uint8_t button) -> ActionMapBuilder& {
    current_chord_ = mouse_button_input(button);
    return *this;
}

auto ActionMapBuilder::clear_chord() -> ActionMapBuilder& {
    current_chord_.reset();
    return *this;
}

auto ActionMapBuilder::scancode(SDL_Scancode scancode, Press press) -> ActionMapBuilder& {
    if (current_action_.has_value()) {
        action_map_.map_press(press, scancode_input(scancode), current_chord_, *current_action_);
    }
    return *this;
}

auto ActionMapBuilder::keycode(SDL_Keycode keycode, Press press) -> ActionMapBuilder& {
    if (current_action_.has_value()) {
        action_map_.map_press(press, keycode_input(keycode), current_chord_, *current_action_);
    }
    return *this;
}

auto ActionMapBuilder::mouse_button(std::uint8_t button, Press press) -> ActionMapBuilder& {
    if (current_action_.has_value()) {
        action_map_.map_press(press, mouse_button_input(button), current_chord_, *current_action_);
    }
    return *this;
}

auto ActionMapBuilder::mouse_axis(MouseAxis axis) -> ActionMapBuilder& {
    if (current_action_.has_value()) {
        action_map_.map_axis(mouse_axis_input(axis), *current_action_);
    }
    return *this;
}

auto ActionMapBuilder::gamepad_button(InputId gamepad_id, SDL_GamepadButton button, Press press) -> ActionMapBuilder& {
    if (current_action_.has_value()) {
        action_map_.map_press(press, gamepad_button_input(gamepad_id, button), current_chord_, *current_action_);
    }
    return *this;
}

auto ActionMapBuilder::gamepad_axis(InputId gamepad_id, SDL_GamepadAxis axis) -> ActionMapBuilder& {
    if (current_action_.has_value()) {
        action_map_.map_axis(gamepad_axis_input(gamepad_id, axis), *current_action_);
    }
    return *this;
}

auto ActionMapBuilder::finish() const -> ActionMap {
    return action_map_;
}
