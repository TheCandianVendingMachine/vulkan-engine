#include "engine/assets/library.h"
#include "engine/engine_utils.h"
#include "engine/fileio/file.h"
#include "engine/graphics/vulkan.h"
#include <Volk/volk.h>
#include <cstdint>
#include <cstdio>
#include <filesystem>
#include <optional>

ENGINE_NS::asset::BytecodeShader::BytecodeShader(const BytecodeShader& rhs) : spirv_(rhs.spirv_), metadata_(rhs.metadata_) {
}

ENGINE_NS::asset::BytecodeShader::BytecodeShader(BytecodeShader&& rhs) noexcept :
    spirv_(std::move(rhs.spirv_)), metadata_(std::move(rhs.metadata_)) {
}

auto ENGINE_NS::asset::BytecodeShader::operator=(const BytecodeShader& rhs) -> BytecodeShader& {
    if (&rhs != this) {
        spirv_    = rhs.spirv_;
        metadata_ = rhs.metadata_;
    }
    return *this;
}

auto ENGINE_NS::asset::BytecodeShader::operator=(BytecodeShader&& rhs) noexcept -> BytecodeShader& {
    if (&rhs != this) {
        spirv_    = std::move(rhs.spirv_);
        metadata_ = std::move(rhs.metadata_);
    }
    return *this;
}

auto ENGINE_NS::asset::BytecodeShader::compile(VulkanDevice& device) -> std::optional<CompiledShader> {
    VkShaderModuleCreateInfo create_info{};
    create_info.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    create_info.pNext    = nullptr;
    create_info.codeSize = spirv_.size() * sizeof(std::uint32_t);
    create_info.pCode    = spirv_.data();

    VkShaderModule shader;
    if (vkCreateShaderModule(device.device, &create_info, nullptr, &shader) != VK_SUCCESS) {
        return std::nullopt;
    }
    return CompiledShader(shader, metadata_);
}

auto ENGINE_NS::asset::BytecodeShader::load_from_file(const std::filesystem::path& path) -> BytecodeShader {
    auto file_result = fileio::File::open(path, fileio::OpenMode::BINARY, fileio::IoMode::READ);
    if (!file_result) {
        crash(ErrorCode::CANNOT_READ_FILE, __LINE__, __func__, __FILE__, file_result.error().reason);
    }
    auto file            = std::move(file_result.value());
    std::uint64_t length = 0;
    if (auto err = file.length(); !err) {
        crash(ErrorCode::CANNOT_READ_FILE, __LINE__, __func__, __FILE__, file_result.error().reason);
    } else {
        length = err.value();
    }
    std::vector<std::uint32_t> spirv(length);
    if (auto err = file.read_into(spirv); !err) {
        crash(ErrorCode::CANNOT_READ_FILE, __LINE__, __func__, __FILE__, file_result.error().reason);
    }

    ShaderMetadata metadata{};
    metadata.file_info = file.metadata;

    return BytecodeShader(std::move(spirv), metadata);
}

ENGINE_NS::asset::BytecodeShader::BytecodeShader(std::vector<std::uint32_t>&& spirv, ShaderMetadata metadata) :
    spirv_(std::move(spirv)), metadata_(metadata) {
}

ENGINE_NS::asset::CompiledShader::CompiledShader(VkShaderModule handle, ShaderMetadata metadata) : metadata_(metadata), shader_(handle) {
}
