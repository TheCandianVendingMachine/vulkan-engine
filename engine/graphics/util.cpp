#include "engine/graphics/util.h"

#include "engine/graphics/vulkan.h"

#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_vulkan.h>
#include <volk.h>
#include <vulkan/vulkan_core.h>

#include <cstdint>
// clang-format off
#include <Tracy/TracyVulkan.hpp>
// clang-format on

void ENGINE_NS::graphics::ImGui::start_frame() {
    if (frame_ != -1) {
        if (frame_ == ::ImGui::GetFrameCount()) {
            ::ImGui::EndFrame();
        }
    }
    for (auto& event : events) {
        ImGui_ImplSDL3_ProcessEvent(&event);
    }
    events.clear();
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ::ImGui::NewFrame();
    frame_ = ::ImGui::GetFrameCount();
}

auto ENGINE_NS::graphics::Immediate::setup(VkDevice device) const -> void {
    VK_CHECK(vkResetFences(device, 1, &fence));
    VK_CHECK(vkResetCommandBuffer(command_buffer, 0));

    VkCommandBuffer cmd = command_buffer;

    VkCommandBufferBeginInfo begin_info = command_buffer_begin_info(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
    VK_CHECK(vkBeginCommandBuffer(command_buffer, &begin_info));
    TracyVkCollect(tracy_context, cmd);
}

auto ENGINE_NS::graphics::Immediate::teardown(VkDevice device) const -> void {
    VK_CHECK(vkEndCommandBuffer(command_buffer));

    VkCommandBufferSubmitInfo command_info = command_buffer_submit_info(command_buffer);
    VkSubmitInfo2 subit_info               = submit_info(&command_info, nullptr, nullptr);

    constexpr std::uint64_t TIMEOUT = 10'000'000'000;
    VK_CHECK(vkQueueSubmit2(queue, 1, &subit_info, fence));
    VK_CHECK(vkWaitForFences(device, 1, &fence, VK_TRUE, TIMEOUT));
}
