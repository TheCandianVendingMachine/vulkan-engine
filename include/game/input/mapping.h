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
        /// Return actions mapped to a pressed physical keyboard key.
        ///
        /// Scancodes represent physical key positions and are usually preferred for gameplay bindings.
        [[nodiscard]]
        auto action_from_scancode(SDL_Scancode scancode) const -> const std::vector<Action>&;

        /// Return actions mapped to a pressed logical keyboard key.
        ///
        /// Keycodes represent layout-aware key values and are usually preferred for text/UI shortcuts.
        [[nodiscard]]
        auto action_from_keycode(SDL_Keycode keycode) const -> const std::vector<Action>&;

        /// Return actions mapped to a pressed mouse button.
        [[nodiscard]]
        auto action_from_mouse_button(std::uint8_t button) const -> const std::vector<Action>&;

        /// Return actions mapped to mouse movement on the first non-zero axis.
        [[nodiscard]]
        auto action_from_mouse_motion(float x_axis, float y_axis) const -> const std::vector<Action>&;

        /// Return actions mapped to a pressed gamepad button for a specific gamepad/input id.
        [[nodiscard]]
        auto action_from_gamepad_button(InputId gamepad_id, SDL_GamepadButton button) const -> const std::vector<Action>&;

        /// Return actions mapped to gamepad axis movement for a specific gamepad/input id.
        [[nodiscard]]
        auto action_from_gamepad_axis(InputId gamepad_id, SDL_GamepadAxis axis) const -> const std::vector<Action>&;

        /// Add a default `Press::DOWN` scancode mapping.
        auto map_scancode(SDL_Scancode scancode, Action action) -> void;

        /// Add a default `Press::DOWN` keycode mapping.
        auto map_keycode(SDL_Keycode keycode, Action action) -> void;

        /// Add a default `Press::DOWN` mouse button mapping.
        auto map_mouse_button(std::uint8_t button, Action action) -> void;

        /// Add a mouse axis mapping.
        auto map_mouse_motion(MouseAxis axis, Action action) -> void;

        /// Add a default `Press::DOWN` gamepad button mapping for a specific gamepad/input id.
        auto map_gamepad_button(InputId gamepad_id, SDL_GamepadButton button, Action action) -> void;

        /// Add a gamepad axis mapping for a specific gamepad/input id.
        auto map_gamepad_axis(InputId gamepad_id, SDL_GamepadAxis axis, Action action) -> void;

        /// Start fluent construction of an ActionMap.
        static auto build() -> ActionMapBuilder;

        /// Load an ActionMap from a serialized mapping file.
        static auto load_from_file(engine::fileio::File& file) -> ActionMap;

        /// Serialize this ActionMap into a human-editable mapping file.
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
        /// Set the action that following input methods will map to.
        ///
        /// This remains active until the next call to action(), allowing several inputs to map to the same action.
        auto action(Action action) -> ActionMapBuilder&;

        /// Require a physical keyboard key chord for following press mappings.
        ///
        /// A chord is a secondary/modifier input that must be active with the primary input, like Ctrl+S.
        /// The chord remains active until clear_chord() or another chord_*() call.
        auto chord_scancode(SDL_Scancode scancode) -> ActionMapBuilder&;

        /// Require a logical keyboard key chord for following press mappings.
        ///
        /// Use this for layout-aware modifier-style combinations.
        auto chord_keycode(SDL_Keycode keycode) -> ActionMapBuilder&;

        /// Require a mouse button chord for following press mappings.
        auto chord_mouse_button(std::uint8_t button) -> ActionMapBuilder&;

        /// Remove the current chord so following mappings use only their primary input.
        auto clear_chord() -> ActionMapBuilder&;

        /// Map a physical keyboard key to the current action.
        auto scancode(SDL_Scancode scancode, Press press = Press::DOWN) -> ActionMapBuilder&;

        /// Map a logical keyboard key to the current action.
        auto keycode(SDL_Keycode keycode, Press press = Press::DOWN) -> ActionMapBuilder&;

        /// Map a mouse button to the current action.
        auto mouse_button(std::uint8_t button, Press press = Press::DOWN) -> ActionMapBuilder&;

        /// Map mouse movement on an axis to the current action.
        auto mouse_axis(MouseAxis axis) -> ActionMapBuilder&;

        /// Map a gamepad button for a specific gamepad/input id to the current action.
        auto gamepad_button(InputId gamepad_id, SDL_GamepadButton button, Press press = Press::DOWN) -> ActionMapBuilder&;

        /// Map a gamepad axis for a specific gamepad/input id to the current action.
        auto gamepad_axis(InputId gamepad_id, SDL_GamepadAxis axis) -> ActionMapBuilder&;

        /// Return the completed ActionMap.
        [[nodiscard]]
        auto finish() const -> ActionMap;

    private:
        ActionMap action_map_{};
        std::optional<Action> current_action_{};
        std::optional<Input> current_chord_{};
};
