#pragma once
#include "engine/meta_defines.h"
#include <string>

namespace ENGINE_NS {
    class Version {
        public:
            Version() = default;
            Version(unsigned int major, unsigned int minor, unsigned int patch);
            auto as_string() const -> std::string;

            Version(const Version& rhs);
            Version(Version&& rhs) noexcept;

            auto operator=(const Version& rhs) -> Version&;
            auto operator=(Version&& rhs) noexcept -> Version&;

            friend auto operator==(const Version& lhs, const Version& rhs) -> bool;

            const unsigned int& major = major_;
            const unsigned int& minor = minor_;
            const unsigned int& patch = patch_;

        private:
            unsigned int major_ = 0;
            unsigned int minor_ = 0;
            unsigned int patch_ = 0;
    };
} // namespace ENGINE_NS
