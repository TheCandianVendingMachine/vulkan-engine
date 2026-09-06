#include "game/input/mapping.h"

#include <SDL3/SDL_gamepad.h>
#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_scancode.h>

#include <string>
#include <string_view>

namespace {
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
