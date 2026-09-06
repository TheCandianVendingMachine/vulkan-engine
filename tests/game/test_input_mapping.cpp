#include "game/input/mapping.h"

#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_scancode.h>
#include <engine/fileio/file.h>

#include <catch2/catch_test_macros.hpp>

#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>

namespace {
    auto action_text(Action action) -> const char* {
        return static_cast<const char* const&>(action);
    }

    auto read_all(const std::filesystem::path& path) -> std::string {
        auto stream = std::ifstream(path, std::ios::binary);
        auto buffer = std::ostringstream{};
        buffer << stream.rdbuf();
        return buffer.str();
    }
}

TEST_CASE("ActionMap builder maps inputs to actions", "[input]") {
    auto action_map = ActionMap::build()
                          .action(Action{"jump"})
                          .scancode(SDL_SCANCODE_SPACE)
                          .keycode(SDLK_J)
                          .mouse_button(SDL_BUTTON_LEFT)
                          .mouse_axis(MouseAxis::X)
                          .gamepad_button(InputId{1}, SDL_GAMEPAD_BUTTON_SOUTH)
                          .gamepad_axis(InputId{1}, SDL_GAMEPAD_AXIS_LEFTX)
                          .finish();

    REQUIRE(action_map.action_from_scancode(SDL_SCANCODE_SPACE).size() == 1);
    CHECK(std::string{action_text(action_map.action_from_scancode(SDL_SCANCODE_SPACE).front())} == "jump");

    REQUIRE(action_map.action_from_keycode(SDLK_J).size() == 1);
    CHECK(std::string{action_text(action_map.action_from_keycode(SDLK_J).front())} == "jump");

    REQUIRE(action_map.action_from_mouse_button(SDL_BUTTON_LEFT).size() == 1);
    CHECK(std::string{action_text(action_map.action_from_mouse_button(SDL_BUTTON_LEFT).front())} == "jump");

    REQUIRE(action_map.action_from_mouse_motion(1.f, 0.f).size() == 1);
    CHECK(std::string{action_text(action_map.action_from_mouse_motion(1.f, 0.f).front())} == "jump");

    REQUIRE(action_map.action_from_gamepad_button(InputId{1}, SDL_GAMEPAD_BUTTON_SOUTH).size() == 1);
    CHECK(std::string{action_text(action_map.action_from_gamepad_button(InputId{1}, SDL_GAMEPAD_BUTTON_SOUTH).front())} == "jump");

    REQUIRE(action_map.action_from_gamepad_axis(InputId{1}, SDL_GAMEPAD_AXIS_LEFTX).size() == 1);
    CHECK(std::string{action_text(action_map.action_from_gamepad_axis(InputId{1}, SDL_GAMEPAD_AXIS_LEFTX).front())} == "jump");
}

TEST_CASE("ActionMap serializes mappings as editable TOML", "[input]") {
    auto action_map = ActionMap::build()
                          .action(Action{"jump"})
                          .scancode(SDL_SCANCODE_SPACE)
                          .keycode(SDLK_J, Press::HELD)
                          .action(Action{"move_left"})
                          .mouse_button(SDL_BUTTON_LEFT)
                          .mouse_axis(MouseAxis::X)
                          .gamepad_button(InputId{2}, SDL_GAMEPAD_BUTTON_WEST, Press::DOUBLE)
                          .gamepad_axis(InputId{2}, SDL_GAMEPAD_AXIS_LEFTX)
                          .finish();

    auto path = std::filesystem::temp_directory_path() / "vulkan_engine_input_mapping_test.toml";
    auto file = engine::fileio::File::open(path, engine::fileio::OpenMode::PLAIN, engine::fileio::IoMode::WRITE);
    REQUIRE(file.has_value());
    action_map.save_to_file(file.value());
    REQUIRE(file->close().has_value());

    const auto serialized = read_all(path);
    CHECK(serialized.find("[[action.jump.keyboard]]") != std::string::npos);
    CHECK(serialized.find("scancodes = [ { id = \"Space\", state = \"down\" } ]") != std::string::npos);
    CHECK(serialized.find("keycodes = [ { id = \"J\", state = \"held\" } ]") != std::string::npos);
    CHECK(serialized.find("[[action.move_left.mouse]]") != std::string::npos);
    CHECK(serialized.find("button = [ { id = \"Left\", state = \"down\" } ]") != std::string::npos);
    CHECK(serialized.find("axis = [ { id = \"X\" } ]") != std::string::npos);
    CHECK(serialized.find("[[action.move_left.gamepad]]") != std::string::npos);
    CHECK(serialized.find("state = \"double\"") != std::string::npos);

    std::filesystem::remove(path);
}
