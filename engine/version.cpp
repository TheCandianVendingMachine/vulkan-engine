#include "engine/version.h"
#include <fmt/format.h>
#include <type_traits>

ENGINE_NS::Version::Version(unsigned int major, unsigned int minor, unsigned int patch) : major_(major), minor_(minor), patch_(patch) {
}

auto ENGINE_NS::Version::as_string() const -> std::string {
    return fmt::format("v{}.{}.{}", major_, minor_, patch_);
}

ENGINE_NS::Version::Version(const Version& rhs) {
    *this = rhs;
}

ENGINE_NS::Version::Version(Version&& rhs) noexcept {
    *this = std::move(rhs);
}

auto ENGINE_NS::Version::operator=(const Version& rhs) -> Version& {
    if (&rhs != this) {
        this->major_ = rhs.major_;
        this->minor_ = rhs.minor_;
        this->patch_ = rhs.patch_;
    }
    return *this;
}

auto ENGINE_NS::Version::operator=(Version&& rhs) noexcept -> Version& {
    if (&rhs != this) {
        this->major_ = std::move(rhs.major_);
        this->minor_ = std::move(rhs.minor_);
        this->patch_ = std::move(rhs.patch_);
    }
    return *this;
}

auto ENGINE_NS::operator==(const Version& lhs, const Version& rhs) -> bool {
    return lhs.major_ == rhs.major_ && lhs.minor_ == rhs.minor_ && lhs.patch_ == rhs.patch_;
}
