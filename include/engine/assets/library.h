#pragma once
#include "engine/fileio/file.h"
#include "engine/graphics/vulkan.h"
#include "engine/meta_defines.h"
#include "engine/pool.h"

#include <vulkan/vulkan_core.h>

#include <cstdint>
#include <filesystem>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace ENGINE_NS {
    namespace asset {
        class BytecodeShader;
        struct ShaderMetadata {
                std::optional<fileio::FileMetadata> file_info{};
                std::string entrypoint = "main";
        };
        class CompiledShader {
            public:
                const VkShaderModule& shader   = shader_;
                const ShaderMetadata& metadata = metadata_;

                CompiledShader(const CompiledShader& rhs)     = default;
                CompiledShader(CompiledShader&& rhs) noexcept = default;

                auto operator=(const CompiledShader& rhs) -> CompiledShader&;
                auto operator=(CompiledShader&& rhs) noexcept -> CompiledShader&;

            private:
                CompiledShader(VkShaderModule handle, ShaderMetadata metadata);
                ShaderMetadata metadata_{};
                VkShaderModule shader_ = VK_NULL_HANDLE;
                friend class BytecodeShader;
        };
        class BytecodeShader {
            public:
                BytecodeShader(const BytecodeShader& rhs)     = default;
                BytecodeShader(BytecodeShader&& rhs) noexcept = default;

                auto operator=(const BytecodeShader& rhs) -> BytecodeShader&     = default;
                auto operator=(BytecodeShader&& rhs) noexcept -> BytecodeShader& = default;

                auto compile(VulkanDevice& device) -> std::optional<CompiledShader>;

                static auto load_from_file(const std::filesystem::path& path) -> BytecodeShader;

            private:
                BytecodeShader(std::vector<std::uint32_t>&& spirv, ShaderMetadata metadata);
                ShaderMetadata metadata_{};
                std::vector<std::uint32_t> spirv_{};
        };
    } // namespace asset
} // namespace ENGINE_NS
