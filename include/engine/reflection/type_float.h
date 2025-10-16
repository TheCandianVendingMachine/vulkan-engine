template<>
struct ENGINE_NS::reflection::Type<float>: ENGINE_NS::reflection::Atom<float> {
	using Inner = float;

	static constexpr auto name() -> const char* {
		return "float32";
	}
	static auto as_string(const Inner& var) -> std::string {
		return fmt::format("{}", var);
	}
	static auto as_human_string(const Inner& var) -> std::string {
		return fmt::format("{:.6f}", var);
	}

	static auto construct() -> Inner {
		return Inner();
	}
	static auto construct(float&& arg) -> Inner {
		return static_cast<Inner>(arg);
	}
	static auto construct(double&& arg) -> Inner {
		return static_cast<Inner>(arg);
	}

	static auto cast(float& arg) -> Inner {
		return static_cast<Inner>(arg);
	}
	static auto cast(double& arg) -> Inner {
		return static_cast<Inner>(arg);
	}

	static auto cast_from_ptr(void* arg) -> Inner& {
		return *reinterpret_cast<Inner*>(arg);
	}
};

template<>
struct ENGINE_NS::reflection::Type<double>: ENGINE_NS::reflection::Atom<double> {
	using Inner = double;

	static constexpr auto name() -> const char* {
		return "float64";
	}
	static auto as_string(const Inner& var) -> std::string {
		return fmt::format("{}", var);
	}
	static auto as_human_string(const Inner& var) -> std::string {
		return fmt::format("{:.6f}", var);
	}

	static auto construct() -> Inner {
		return Inner();
	}
	static auto construct(float&& arg) -> Inner {
		return static_cast<Inner>(arg);
	}
	static auto construct(double&& arg) -> Inner {
		return static_cast<Inner>(arg);
	}

	static auto cast(float& arg) -> Inner {
		return static_cast<Inner>(arg);
	}
	static auto cast(double& arg) -> Inner {
		return static_cast<Inner>(arg);
	}

	static auto cast_from_ptr(void* arg) -> Inner& {
		return *reinterpret_cast<Inner*>(arg);
	}
};
