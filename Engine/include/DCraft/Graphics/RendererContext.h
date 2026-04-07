//
// Created by denzel on 09/08/2025.
//
#pragma once

#include <cstdint>

// Forward declarations
namespace hellfire {
    class Entity;
    class CameraComponent;
}

namespace hellfire {
    class OGLRendererContext {
    public:
        uint32_t default_shader_program;
    
        // Entity pointers for lights
        Entity* directional_light_entities[4];  // MAX_DIRECTIONAL_LIGHTS = 4
        Entity* point_light_entities[8];        // MAX_POINT_LIGHTS = 8
        int num_directional_lights = 0;
        int num_point_lights = 0;
    
        CameraComponent* camera_component = nullptr;
    
        // Constructor to initialize arrays
        OGLRendererContext() : default_shader_program(0) {
            for (int i = 0; i < 4; i++) directional_light_entities[i] = nullptr;
            for (int i = 0; i < 8; i++) point_light_entities[i] = nullptr;
        }
    };
}