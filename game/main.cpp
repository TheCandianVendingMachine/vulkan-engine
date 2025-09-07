#include <iostream>
#include <engine/engine.h>
#include <engine/logger.h>
#include <fmt/format.h>

int main() {
    Logger logger;
    logger.info("Number {}", 5);
    logger.debug("Oh No! {}", false);
    logger.warning("Watch out {}", 45.4442);
    logger.error("Fuck! {}", "oh no");
    logger.info("1");
    logger.info("2");
    logger.info("3");
    logger.info("4");
    std::cout << "Hello, world!" << std::endl;
    Engine::Status status = Engine::status();
    if (status == Engine::Status::OK) {
        std::cout << "Engine status: OK" << std::endl;
    } else {
        std::cout << "Engine status: ERROR" << std::endl;
    }
    std::cout << "hi" << std::endl;
    for (auto &entry : logger.last_entries_of(1, logger::Level::DEBUG)) {
        std::cout << fmt::format("{}", *entry) << std::endl;
    }
    std::cout << "bye" << std::endl;
    return 0;
}