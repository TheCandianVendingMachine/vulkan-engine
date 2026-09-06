#pragma once
#include "game/input/types.h"

#include <SDL3/SDL_events.h>
#include <engine/fileio/file.h>
#include <robin_map.h>

#include <cstddef>
#include <cstdint>
#include <functional>
#include <optional>
#include <tuple>
#include <vector>

using InputPair      = std::tuple<Input, std::optional<Input>>;
using PressInputPair = std::tuple<Press, InputPair>;

struct PressInputPairHash {
        auto operator()(const PressInputPair& mapping) const noexcept -> std::size_t;
};

class ActionMapBuilder;

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

        static auto build() -> ActionMapBuilder;
        static auto load_from_file(engine::fileio::File& file) -> ActionMap;
        auto save_to_file(engine::fileio::File& file) const -> void;

    private:
        auto map_axis(Input input, Action action) -> void;
        auto map_press(Press press, Input input, std::optional<Input> chord, Action action) -> void;

        tsl::robin_map<Input, std::vector<Action>> axis_mappings_;
        tsl::robin_map<PressInputPair, std::vector<Action>, PressInputPairHash> press_mappings_;

        friend class ActionMapBuilder;
};

class ActionMapBuilder {
    public:
        auto action(Action action) -> ActionMapBuilder&;
        auto chord_scancode(SDL_Scancode scancode) -> ActionMapBuilder&;
        auto chord_keycode(SDL_Keycode keycode) -> ActionMapBuilder&;
        auto chord_mouse_button(std::uint8_t button) -> ActionMapBuilder&;
        auto clear_chord() -> ActionMapBuilder&;

        auto scancode(SDL_Scancode scancode, Press press = Press::DOWN) -> ActionMapBuilder&;
        auto keycode(SDL_Keycode keycode, Press press = Press::DOWN) -> ActionMapBuilder&;
        auto mouse_button(std::uint8_t button, Press press = Press::DOWN) -> ActionMapBuilder&;
        auto mouse_axis(MouseAxis axis) -> ActionMapBuilder&;
        auto gamepad_button(InputId gamepad_id, SDL_GamepadButton button, Press press = Press::DOWN) -> ActionMapBuilder&;
        auto gamepad_axis(InputId gamepad_id, SDL_GamepadAxis axis) -> ActionMapBuilder&;

        [[nodiscard]]
        auto finish() const -> ActionMap;

    private:
        ActionMap action_map_{};
        std::optional<Action> current_action_{};
        std::optional<Input> current_chord_{};
};
