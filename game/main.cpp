#include <chrono>
#include <engine/assets/library.h>
#include <engine/engine.h>
#include <thread>

int main() {
    engine::Engine engine{};

    auto bytecode = engine::asset::BytecodeShader::load_from_file("./assets/shaders/engine/test.spv");

    engine.run();
    return 0;
}
