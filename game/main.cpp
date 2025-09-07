#include <iostream>
#include <engine/engine.h>

int main() {
    std::cout << "Hello, world!" << std::endl;
    Engine::Status status = Engine::status();
    if (status == Engine::Status::OK) {
        std::cout << "Engine status: OK" << std::endl;
    } else {
        std::cout << "Engine status: ERROR" << std::endl;
    }
    return 0;
}