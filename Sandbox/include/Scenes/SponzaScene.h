//
// Created by denzel on 21/08/2025.
//

#pragma once


namespace DCraft {
    class Scene;
    class ShaderManager;
    struct WindowInfo;
    class SceneManager;
}

DCraft::Scene* load_sponza_scene(DCraft::SceneManager& scene_manager, DCraft::WindowInfo window, DCraft::ShaderManager& shader_manager);
