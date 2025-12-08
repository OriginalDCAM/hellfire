//
// Created by denzel on 08/12/2025.
//


#include <catch2/catch_test_macros.hpp>
#include "hellfire/serialization/Serializer.h"
#include <glm/gtc/epsilon.hpp>
#include <sstream>

#include "catch2/matchers/catch_matchers_floating_point.hpp"

using namespace hellfire;
using Catch::Matchers::WithinAbs;

// Helper to compare vec3 with tolerance
bool vec3_equal(const glm::vec3 &a, const glm::vec3 &b, float epsilon = 0.0001f) {
    return glm::all(glm::epsilonEqual(a, b, epsilon));
}

TEST_CASE("TransformComponent serialization", "[serialization][transform]") {
    SECTION("Round-trip preserves default values") {
        TransformComponent original;

        std::stringstream ss;
        REQUIRE(Serializer<TransformComponent>::serialize(ss, &original));

        TransformComponent loaded;
        REQUIRE(Serializer<TransformComponent>::deserialize(ss, &loaded));

        CHECK(vec3_equal(loaded.get_position(), glm::vec3(0, 0, 0)));
        CHECK(vec3_equal(loaded.get_rotation(), glm::vec3(0, 0, 0)));
        CHECK(vec3_equal(loaded.get_scale(), glm::vec3(1, 1, 1)));
    }

    SECTION("Round-trip preserves custom values") {
        TransformComponent original;
        original.set_position(1.5f, -25.6f, 100.0f);
        original.set_rotation(45.0f, 90.0f, 180.0f);
        original.set_scale(2.0f, 0.5f, 1.0f);

        std::stringstream ss;
        REQUIRE(Serializer<TransformComponent>::serialize(ss, &original));

        TransformComponent loaded;
        REQUIRE(Serializer<TransformComponent>::deserialize(ss, &loaded));

        CHECK(vec3_equal(loaded.get_position(), original.get_position()));
        CHECK(vec3_equal(loaded.get_rotation(), original.get_rotation()));
        CHECK(vec3_equal(loaded.get_scale(), original.get_scale()));
    }

    SECTION("Handles extreme values") {
        TransformComponent original;
        original.set_position(FLT_MAX, FLT_MIN, -FLT_MAX);
        original.set_rotation(360.0f, -360.0f, 720.0f);
        original.set_scale(0.000001f, 10000.0f, 1.0f);

        std::stringstream ss;
        REQUIRE(Serializer<TransformComponent>::serialize(ss, &original));

        TransformComponent loaded;
        REQUIRE(Serializer<TransformComponent>::deserialize(ss, &loaded));

        CHECK(vec3_equal(loaded.get_position(), original.get_position()));
        CHECK(vec3_equal(loaded.get_rotation(), original.get_rotation()));
        CHECK(vec3_equal(loaded.get_scale(), original.get_scale()));
    }

    SECTION("Handles negative values") {
        TransformComponent original;
        original.set_position(-10.0f, -20.0f, -30.0f);
        original.set_rotation(-45.0f, -90.0f, -180.0f);
        original.set_scale(-1.0f, -1.0f, -1.0f); // Negative scale for mirroring

        std::stringstream ss;
        REQUIRE(Serializer<TransformComponent>::serialize(ss, &original));

        TransformComponent loaded;
        REQUIRE(Serializer<TransformComponent>::deserialize(ss, &loaded));

        CHECK(vec3_equal(loaded.get_position(), original.get_position()));
        CHECK(vec3_equal(loaded.get_rotation(), original.get_rotation()));
        CHECK(vec3_equal(loaded.get_scale(), original.get_scale()));
    }

    SECTION("Preserves floating point precision") {
        TransformComponent original;
        original.set_position(0.123456789f, 3.14159265f, 0.987654321f);

        std::stringstream ss;
        REQUIRE(Serializer<TransformComponent>::serialize(ss, &original));

        TransformComponent loaded;
        REQUIRE(Serializer<TransformComponent>::deserialize(ss, &loaded));

        CHECK(vec3_equal(loaded.get_position(), original.get_position(), 0.00001f));
    }
}

TEST_CASE("TransformComponent serialization error handling", "[serialization][transform]") {
    SECTION("Serialize returns false for nullptr") {
        std::stringstream ss;
        REQUIRE_FALSE(Serializer<TransformComponent>::serialize(ss, nullptr));
    }

    SECTION("Deserialize returns false for nullptr") {
        std::stringstream ss;
        ss << R"({"position":[0,0,0],"rotation":[0,0,0],"scale":[1,1,1]})";
        REQUIRE_FALSE(Serializer<TransformComponent>::deserialize(ss, nullptr));
    }

    SECTION("Deserialize returns false for empty input") {
        std::stringstream ss;
        TransformComponent comp;
        REQUIRE_FALSE(Serializer<TransformComponent>::deserialize(ss, &comp));
    }

    SECTION("Deserialize returns false for invalid JSON") {
        std::stringstream ss;
        ss << "not valid json {{{";

        TransformComponent comp;
        REQUIRE_FALSE(Serializer<TransformComponent>::deserialize(ss, &comp));
    }

    SECTION("Deserialize returns false for missing position field") {
        std::stringstream ss;
        ss << R"({"rotation":[0,0,0],"scale":[1,1,1]})";

        TransformComponent comp;
        REQUIRE_FALSE(Serializer<TransformComponent>::deserialize(ss, &comp));
    }

    SECTION("Deserialize returns false for missing rotation field") {
        std::stringstream ss;
        ss << R"({"position":[0,0,0],"scale":[1,1,1]})";

        TransformComponent comp;
        REQUIRE_FALSE(Serializer<TransformComponent>::deserialize(ss, &comp));
    }

    SECTION("Deserialize returns false for missing scale field") {
        std::stringstream ss;
        ss << R"({"position":[0,0,0],"rotation":[0,0,0]})";

        TransformComponent comp;
        REQUIRE_FALSE(Serializer<TransformComponent>::deserialize(ss, &comp));
    }

    SECTION("Deserialize returns false for wrong type in position") {
        std::stringstream ss;
        ss << R"({"position":"not an array","rotation":[0,0,0],"scale":[1,1,1]})";

        TransformComponent comp;
        REQUIRE_FALSE(Serializer<TransformComponent>::deserialize(ss, &comp));
    }

    SECTION("Deserialize returns false for wrong array size") {
        std::stringstream ss;
        ss << R"({"position":[0,0],"rotation":[0,0,0],"scale":[1,1,1]})"; // Only 2 elements

        TransformComponent comp;
        REQUIRE_FALSE(Serializer<TransformComponent>::deserialize(ss, &comp));
    }
}

TEST_CASE("TransformComponent JSON output format", "[serialization][transform]") {
    SECTION("Output is valid JSON") {
        TransformComponent comp;
        comp.set_rotation(1, 2, 3);

        std::stringstream ss;
        REQUIRE(Serializer<TransformComponent>::serialize(ss, &comp));

        // Verify it parses as valid json
        nlohmann::json j;
        REQUIRE_NOTHROW(j = nlohmann::json::parse(ss.str()));
    }

    SECTION("Output contains expected keys") {
        TransformComponent comp;

        std::stringstream ss;
        Serializer<TransformComponent>::serialize(ss, &comp);

        nlohmann::json j = nlohmann::json::parse(ss.str());

        CHECK(j.contains("position"));
        CHECK(j.contains("rotation"));
        CHECK(j.contains("scale"));
    }

    SECTION("Position is serialized as array") {
        TransformComponent comp;
        comp.set_position(1, 2, 3);

        std::stringstream ss;
        Serializer<TransformComponent>::serialize(ss, &comp);

        nlohmann::json j = nlohmann::json::parse(ss.str());

        REQUIRE(j["position"].is_array());
        REQUIRE(j["position"].size() == 3);

        REQUIRE(j["position"][0] == 1.0f);
        REQUIRE(j["position"][1] == 2.0f);
        REQUIRE(j["position"][2] == 3.0f);
    }
}

TEST_CASE("TransformComponent deserialization from manual JSON", "[serialization][transform]") {
    SECTION("Can load hand-written JSON") {
        std::stringstream ss;
        ss << R"({
              "position": [10.0, 20.0, 30.0],
              "rotation": [0.0, 90.0, 180.0],
              "scale": [1.0, 1.0, 1.0]    
        })";

        TransformComponent comp;
        REQUIRE(Serializer<TransformComponent>::deserialize(ss, &comp));
        
        CHECK(vec3_equal(comp.get_position(), glm::vec3(10, 20, 30)));
        CHECK(vec3_equal(comp.get_rotation(), glm::vec3(0, 90, 180)));
        CHECK(vec3_equal(comp.get_scale(), glm::vec3(1, 1, 1)));
    }

    SECTION("Ignore extra fields") {
        std::stringstream ss;
        ss << R"({
              "position": [1.0, 2.0, 3.0],
              "rotation": [0.0, 0.0, 0.0],
              "scale": [1.0, 1.0, 1.0],
              "some_random_field": "should be ignored"   
        })";

        TransformComponent comp;
        REQUIRE(Serializer<TransformComponent>::deserialize(ss, &comp));
        CHECK(vec3_equal(comp.get_position(), glm::vec3(1, 2, 3)));
    }

    SECTION("Handles integer values in JSON") {
        std::stringstream ss;
        ss << R"({"position":[1.0,2.0,3.0],"rotation":[0.0,0.0,0.0],"scale":[1.0,1.0,1.0]})";
        
        TransformComponent comp;
        REQUIRE(Serializer<TransformComponent>::deserialize(ss, &comp));
        CHECK(vec3_equal(comp.get_position(), glm::vec3(1, 2, 3)));
    }
}
