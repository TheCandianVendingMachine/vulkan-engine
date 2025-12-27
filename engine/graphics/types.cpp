#include "engine/graphics/types.h"

#include "engine/engine.h"

ENGINE_NS::ImageAllocation::~ImageAllocation() {
    if (!will_be_destroyed_) {
        g_ENGINE->logger.get(LogNamespaces::GRAPHICS)
            .error("This image has been allocated, but not queued to be destroyed. This memory will be lost");
    }
}

ENGINE_NS::BufferAllocation::~BufferAllocation() {
    if (!will_be_destroyed_) {
        g_ENGINE->logger.get(LogNamespaces::GRAPHICS)
            .error("This buffer has been allocated, but not queued to be destroyed. This memory will be lost");
    }
}
