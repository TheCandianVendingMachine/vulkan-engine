#include "engine/graphics/types.h"

#include "engine/engine.h"

#include <type_traits>

ENGINE_NS::ImageAllocation::ImageAllocation(ImageAllocation&& rhs) noexcept :
    image(std::move(rhs.image)), view(std::move(rhs.view)), allocation(std::move(rhs.allocation)), extent(std::move(rhs.extent)),
    format(std::move(rhs.format)), will_be_destroyed_(std::move(rhs.will_be_destroyed_)) {
    rhs.moved_ = true;
}

auto ENGINE_NS::ImageAllocation::operator=(ImageAllocation&& rhs) noexcept -> ImageAllocation& {
    if (&rhs != this) {
        image              = std::move(rhs.image);
        view               = std::move(rhs.view);
        allocation         = std::move(rhs.allocation);
        extent             = std::move(rhs.extent);
        format             = std::move(rhs.format);
        will_be_destroyed_ = std::move(rhs.will_be_destroyed_);
        rhs.moved_         = true;
    }
    return *this;
}

ENGINE_NS::ImageAllocation::~ImageAllocation() {
    if (moved_) {
        return;
    }
    if (!will_be_destroyed_) {
        auto logger = g_ENGINE->logger.get(LogNamespaces::GRAPHICS);
        logger.get().error("This image has been allocated, but not queued to be destroyed. This memory will be lost");
    }
}

ENGINE_NS::BufferAllocation::BufferAllocation(BufferAllocation&& rhs) noexcept :
    buffer(std::move(rhs.buffer)), allocation(std::move(rhs.allocation)), info(std::move(rhs.info)),
    will_be_destroyed_(std::move(rhs.will_be_destroyed_)) {
    rhs.moved_ = true;
}

auto ENGINE_NS::BufferAllocation::operator=(BufferAllocation&& rhs) noexcept -> BufferAllocation& {
    if (&rhs != this) {
        buffer             = std::move(rhs.buffer);
        allocation         = std::move(rhs.allocation);
        info               = std::move(rhs.info);
        will_be_destroyed_ = std::move(rhs.will_be_destroyed_);
        rhs.moved_         = true;
    }
    return *this;
}

ENGINE_NS::BufferAllocation::~BufferAllocation() {
    if (moved_) {
        return;
    }
    if (!will_be_destroyed_) {
        auto logger = g_ENGINE->logger.get(LogNamespaces::GRAPHICS);

        logger.get().error("This buffer has been allocated, but not queued to be destroyed. This memory will be lost");
    }
}
