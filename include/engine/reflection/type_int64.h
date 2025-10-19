template <>
struct ENGINE_NS::reflection::Type<std::uint64_t> : ENGINE_NS::reflection::Atom<std::uint64_t> {
        using Inner = std::uint64_t;

        static constexpr auto name() -> const char* {
            return "uint64";
        }
        static auto as_string(const Inner& var) -> std::string {
            return fmt::format("{}", var);
        }
        static auto as_human_string(const Inner& var) -> std::string {
            return fmt::format("{}_u64", var);
        }

        static auto construct() -> Inner {
            return Inner();
        }
        static auto construct(std::uint8_t&& arg) -> Inner {
            return static_cast<Inner>(arg);
        }
        static auto construct(std::uint16_t&& arg) -> Inner {
            return static_cast<Inner>(arg);
        }
        static auto construct(std::uint32_t&& arg) -> Inner {
            return static_cast<Inner>(arg);
        }
        static auto construct(std::uint64_t&& arg) -> Inner {
            return static_cast<Inner>(arg);
        }
        static auto construct(std::int8_t&& arg) -> Inner {
            return arg;
        }
        static auto construct(std::int16_t&& arg) -> Inner {
            return static_cast<Inner>(arg);
        }
        static auto construct(std::int32_t&& arg) -> Inner {
            return static_cast<Inner>(arg);
        }
        static auto construct(std::int64_t&& arg) -> Inner {
            return static_cast<Inner>(arg);
        }

        static auto cast(std::uint8_t& arg) -> Inner {
            return arg;
        }
        static auto cast(std::uint16_t& arg) -> Inner {
            return static_cast<Inner>(arg);
        }
        static auto cast(std::uint32_t& arg) -> Inner {
            return static_cast<Inner>(arg);
        }
        static auto cast(std::uint64_t& arg) -> Inner {
            return static_cast<Inner>(arg);
        }
        static auto cast(std::int8_t& arg) -> Inner {
            return arg;
        }
        static auto cast(std::int16_t& arg) -> Inner {
            return static_cast<Inner>(arg);
        }
        static auto cast(std::int32_t& arg) -> Inner {
            return static_cast<Inner>(arg);
        }
        static auto cast(std::int64_t& arg) -> Inner {
            return static_cast<Inner>(arg);
        }

        static auto cast_from_ptr(void* arg) -> Inner& {
            return *reinterpret_cast<Inner*>(arg);
        }
};

template <>
struct ENGINE_NS::reflection::Type<std::int64_t> : ENGINE_NS::reflection::Atom<std::uint64_t> {
        using Inner = std::int64_t;

        static constexpr auto name() -> const char* {
            return "int64";
        }
        static auto as_string(const Inner& var) -> std::string {
            return fmt::format("{}", var);
        }
        static auto as_human_string(const Inner& var) -> std::string {
            return fmt::format("{}_i64", var);
        }

        static auto construct() -> Inner {
            return Inner();
        }
        static auto construct(std::uint8_t&& arg) -> Inner {
            return static_cast<Inner>(arg);
        }
        static auto construct(std::uint16_t&& arg) -> Inner {
            return static_cast<Inner>(arg);
        }
        static auto construct(std::uint32_t&& arg) -> Inner {
            return static_cast<Inner>(arg);
        }
        static auto construct(std::uint64_t&& arg) -> Inner {
            return static_cast<Inner>(arg);
        }
        static auto construct(std::int8_t&& arg) -> Inner {
            return arg;
        }
        static auto construct(std::int16_t&& arg) -> Inner {
            return static_cast<Inner>(arg);
        }
        static auto construct(std::int32_t&& arg) -> Inner {
            return static_cast<Inner>(arg);
        }
        static auto construct(std::int64_t&& arg) -> Inner {
            return static_cast<Inner>(arg);
        }

        static auto cast(std::uint8_t&& arg) -> Inner {
            return arg;
        }
        static auto cast(std::uint16_t&& arg) -> Inner {
            return static_cast<Inner>(arg);
        }
        static auto cast(std::uint32_t&& arg) -> Inner {
            return static_cast<Inner>(arg);
        }
        static auto cast(std::uint64_t&& arg) -> Inner {
            return static_cast<Inner>(arg);
        }
        static auto cast(std::int8_t&& arg) -> Inner {
            return arg;
        }
        static auto cast(std::int16_t&& arg) -> Inner {
            return static_cast<Inner>(arg);
        }
        static auto cast(std::int32_t&& arg) -> Inner {
            return static_cast<Inner>(arg);
        }
        static auto cast(std::int64_t&& arg) -> Inner {
            return static_cast<Inner>(arg);
        }

        static auto cast_from_ptr(void* arg) -> Inner {
            return *reinterpret_cast<Inner*>(arg);
        }
};
