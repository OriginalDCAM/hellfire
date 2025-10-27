//
// Created by denzel on 27/10/2025.
//
#include <catch2/catch_test_macros.hpp>

#include "hellfire/graphics/geometry/Cube.h"
#include "hellfire/graphics/geometry/Quad.h"
#include "hellfire/graphics/geometry/Sphere.h"
#include "hellfire/scene/Scene.h"

TEST_CASE("Scenes can be created and destroyed") {
    auto test_scene = std::make_unique<hellfire::Scene>("Test Scene");

    REQUIRE(test_scene->get_name() == "Test Scene");
    REQUIRE(test_scene->get_entity_count() == 0);

    SECTION("adding one entity increases total count") {
        const hellfire::EntityID test_entity_id = test_scene->create_entity("Test Object");
        REQUIRE(test_scene->get_entity(test_entity_id)->get_name() == "Test Object");
        REQUIRE(test_scene->get_entity_count() == 1);
    }
    SECTION("removing one entity decreases total count") {
        const hellfire::EntityID entity_to_delete_id = test_scene->create_entity("Test Object To Delete");
        REQUIRE(test_scene->get_entity_count() == 1);

        test_scene->destroy_entity(entity_to_delete_id);
        const hellfire::Entity *deleted_entity = test_scene->get_entity(entity_to_delete_id);

        REQUIRE(test_scene->get_entity_count() == 0);
        REQUIRE(deleted_entity == nullptr);
    }
}

TEST_CASE("Scenes can be renamed") {
    const auto test_scene = std::make_unique<hellfire::Scene>("Test Scene");
    REQUIRE(test_scene->get_name() == "Test Scene");

    test_scene->set_name("Better Name V2");
    REQUIRE(test_scene->get_name() == "Better Name V2");
}


TEST_CASE("Scenes can have multiple entities") {
    const auto test_scene = new hellfire::Scene("Test Scene"); // Needs to be a raw pointer
    REQUIRE(test_scene->get_entity_count() == 0);

    test_scene->create_entity("Tree");
    REQUIRE(test_scene->get_entity_count() == 1);
    test_scene->create_entity("Car");
    REQUIRE(test_scene->get_entity_count() == 2);
}

TEST_CASE("Scene camera management") {
}


TEST_CASE("Finding entities in scene") {
}


TEST_CASE("Scene updates world matrices correctly") {
}

TEST_CASE("Scene can have complex hierarchies") {
}
