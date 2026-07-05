#pragma once
#include "engine/meta_defines.h"
#include "engine/engine_utils.h"

namespace ENGINE_NS {
    using LibraryHandle = void*;

    class SharedLibrary {
        public:
            static auto load(const char* path) -> SharedLibrary;
            SharedLibrary() = default;
            ~SharedLibrary();

            template<typename TFunc>
            auto get_symbol(TFunc& function, const char* name) const -> void {
                function = reinterpret_cast<TFunc>(get_symbol(name));
                if (function == nullptr) {
                    engine::crash(ErrorCode::CANNOT_LOAD_LINEAR_ALGEBRA_FUNCTION, __LINE__, __func__, __FILE__);
                }
            }
            auto get_symbol(const char* name) const -> void*;

            [[nodiscard]]
            auto loaded() const -> bool {
                return handle_ != nullptr;
            }

        private:
            LibraryHandle handle_ = nullptr;
    };
}

