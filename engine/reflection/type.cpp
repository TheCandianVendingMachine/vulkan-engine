#include "engine/reflection/type.h"

namespace ENGINE_NS {
    namespace reflection {
        template<>
        static auto RuntimeType::instance<std::int8_t>() -> std::shared_ptr<RuntimeType> {
            static auto instance = std::make_shared<RuntimeTypeInt8>();
            return instance;
        }
        template<>
        static auto RuntimeType::instance<std::int16_t>() -> std::shared_ptr<RuntimeType> {
            static auto instance = std::make_shared<RuntimeTypeInt16>();
            return instance;
        }
        template<>
        static auto RuntimeType::instance<std::int32_t>() -> std::shared_ptr<RuntimeType> {
            static auto instance = std::make_shared<RuntimeTypeInt32>();
            return instance;
        }
        template<>
        static auto RuntimeType::instance<std::int64_t>() -> std::shared_ptr<RuntimeType> {
            static auto instance = std::make_shared<RuntimeTypeInt64>();
            return instance;
        }

        template<>
        static auto RuntimeType::instance<std::uint8_t>() -> std::shared_ptr<RuntimeType> {
            static auto instance = std::make_shared<RuntimeTypeUInt8>();
            return instance;
        }
        template<>
        static auto RuntimeType::instance<std::uint16_t>() -> std::shared_ptr<RuntimeType> {
            static auto instance = std::make_shared<RuntimeTypeUInt16>();
            return instance;
        }
        template<>
        static auto RuntimeType::instance<std::uint32_t>() -> std::shared_ptr<RuntimeType> {
            static auto instance = std::make_shared<RuntimeTypeUInt32>();
            return instance;
        }
        template<>
        static auto RuntimeType::instance<std::uint64_t>() -> std::shared_ptr<RuntimeType> {
            static auto instance = std::make_shared<RuntimeTypeUInt64>();
            return instance;
        }

        template<>
        static auto RuntimeType::instance<float>() -> std::shared_ptr<RuntimeType> {
            static auto instance = std::make_shared<RuntimeTypeFloat32>();
            return instance;
        }
        template<>
        static auto RuntimeType::instance<double>() -> std::shared_ptr<RuntimeType> {
            static auto instance = std::make_shared<RuntimeTypeFloat64>();
            return instance;
        }


        auto Member::into_runtime(const void* owner) const -> RuntimeMember {
            return RuntimeMember {
                *this,
                owner
            };
        }

        auto RuntimeMember::to_string() const -> std::string {
            auto base = static_cast<const std::uint8_t*>(owner_);
            return this->meta.type_info->to_string(base + this->meta.offset_);
        }
        auto RuntimeMember::to_human_string() const -> std::string {
            auto base = static_cast<const std::uint8_t*>(owner_);
            return this->meta.type_info->to_human_string(base + this->meta.offset_);
        }
    }
}
