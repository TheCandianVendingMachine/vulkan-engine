#include <catch2/catch_test_macros.hpp>

#include "engine/reflection/type.h"
#include "engine/utilities/transform.h"
#include "linalg/vector.h"

#include <string>

TEST_CASE("linalg vectors expose composite runtime members") {
    const auto value = linalg::Vector3<double>{1.0, 2.0, 3.0};
    const auto type = engine::reflection::RuntimeType::instance<linalg::Vector3<double>>();

    REQUIRE(type->is_object());
    REQUIRE(std::string{type->name()} == "Vector3");

    const auto members = type->members(&value);
    REQUIRE(members.size() == 3);
    REQUIRE(members[0].meta.name == "x");
    REQUIRE(members[1].meta.name == "y");
    REQUIRE(members[2].meta.name == "z");
    REQUIRE(members[0].to_string() == "1");
    REQUIRE(members[1].to_string() == "2");
    REQUIRE(members[2].to_string() == "3");

    const auto z = type->member(&value, "z");
    REQUIRE(z.has_value());
    REQUIRE(z->to_string() == "3");
}

TEST_CASE("transform reflection traverses nested vector members") {
    auto transform = engine::Transform{};
    transform.set_translate({10.0, 20.0, 30.0});
    transform.set_scale({2.0, 3.0, 4.0});
    const auto type = engine::reflection::RuntimeType::instance<engine::Transform>();

    REQUIRE(type->is_object());

    const auto position = type->member(&transform, "position_");
    REQUIRE(position.has_value());
    REQUIRE(position->has_members());

    const auto x = position->get("x");
    REQUIRE(x.has_value());
    REQUIRE(x->to_string() == "10");

    const auto scale = type->member(&transform, "scale_");
    REQUIRE(scale.has_value());
    const auto scale_z = scale->get("z");
    REQUIRE(scale_z.has_value());
    REQUIRE(scale_z->to_string() == "4");
}
