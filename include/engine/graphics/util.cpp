#include "util.h"
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_vulkan.h>

void ENGINE_NS::graphics::ImGui::new_frame() {
    for (auto& event : events) {
        ImGui_ImplSDL3_ProcessEvent(&event);
    }
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ::ImGui::NewFrame();
}
