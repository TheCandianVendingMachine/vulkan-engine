#include "engine/graphics/util.h"
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_vulkan.h>

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

