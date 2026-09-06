#pragma once
#include "game/input/mapping.h"
#include "game/input/types.h"

class Register {
    public:
        void handle_keyboard_down_event();
        void handle_keyboard_up_event();

        void handle_mouse_button_down_event();
        void handle_mouse_button_up_event();
        void handle_mouse_axis_event();

        void handle_gamepad_added_event();
        void handle_gamepad_removed_event();
        void handle_gamepad_axis_event();
        void handle_gamepad_button_down_event();
        void handle_gamepad_button_up_event();

        ActionMap action_map;

    private:
        InputId next_id_{};
};
