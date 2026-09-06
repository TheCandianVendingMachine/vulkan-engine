#pragma once
#include "game/input/types.h"

#include <SDL3/SDL_events.h>
#include <engine/fileio/file.h>
#include <robin_map.h>

#include <cstdint>
#include <vector>

class ActionMap {
    public:
        [[nodiscard]]
        auto action_from_scancode(SDL_Scancode scancode) const -> const std::vector<Action>&;
        [[nodiscard]]
        auto action_from_keycode(SDL_Keycode keycode) const -> const std::vector<Action>&;
        [[nodiscard]]
        auto action_from_mouse_button(std::uint8_t button) const -> const std::vector<Action>&;
        [[nodiscard]]
        auto action_from_mouse_motion(float x_axis, float y_axis) const -> const std::vector<Action>&;
        [[nodiscard]]
        auto action_from_gamepad_button(InputId gamepad_id, SDL_GamepadButton button) const -> const std::vector<Action>&;
        [[nodiscard]]
        auto action_from_gamepad_axis(InputId gamepad_id, SDL_GamepadAxis axis) const -> const std::vector<Action>&;

        auto map_scancode(SDL_Scancode scancode, Action action) -> void;
        auto map_keycode(SDL_Keycode keycode, Action action) -> void;
        auto map_mouse_button(std::uint8_t button, Action action) -> void;
        auto map_mouse_motion(MouseAxis axis, Action action) -> void;
        auto map_gamepad_button(InputId gamepad_id, SDL_GamepadButton button, Action action) -> void;
        auto map_gamepad_axis(InputId gamepad_id, SDL_GamepadAxis axis, Action action) -> void;

        static auto load_from_file(engine::fileio::File& file) -> ActionMap;
        auto save_to_file(engine::fileio::File& file) -> void;

    private:
        using InputPair = std::tuple<Input, std::optional<Input>>;

        tsl::robin_map<Input, std::vector<Action>> axis_mappings_;
        tsl::robin_map<std::tuple<Press, InputPair>, std::vector<Action>> press_mappings_;
};