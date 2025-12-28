target_sources(engine PRIVATE
    "${ENGINE_HEADER_PATH}/graphics/descriptor.h"
    "${ENGINE_HEADER_PATH}/graphics/graphics.h"
    "${ENGINE_HEADER_PATH}/graphics/pipeline.h"
    "${ENGINE_HEADER_PATH}/graphics/types.h"
    "${ENGINE_HEADER_PATH}/graphics/util.h"
    "${ENGINE_HEADER_PATH}/graphics/vulkan.h"
)
target_sources(engine PRIVATE
    descriptor.cpp
    graphics.cpp
    pipeline.cpp
    types.cpp
    util.cpp
    vulkan.cpp
    vulkan_device.cpp
    vulkan_image.cpp
    vulkan_instance.cpp
    vulkan_physical_device.cpp
    vulkan_queue.cpp
    vulkan_surface.cpp
    vulkan_swapchain.cpp
)
add_subdirectory(scene)
