//
// Created by denzel on 01/10/2025.
//
#include "MenuBarComponent.h"

#include <iostream>
#include "imgui.h"
#include "hellfire/core/Project.h"
#include "hellfire/ecs/ComponentRegistration.h"
#include "hellfire/scene/CameraFactory.h"
#include "hellfire/scene/Scene.h"
#include "hellfire/scene/SceneManager.h"
#include "hellfire/utilities/FileDialog.h"
#include "hellfire/utilities/ServiceLocator.h"
#include "scenes/DefaultScene.h"
#include "events/WindowEvents.h"

namespace hellfire::editor {
    void MenuBarComponent::render() {
        if (ImGui::BeginMenuBar()) {
            render_file_menu();
            render_scene_menu();
            ImGui::EndMenuBar();
        }
    }

    void MenuBarComponent::render_file_menu() {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("New Project")) {
                context_->event_bus.dispatch<OpenNewProjectWindowEvent>();
            }
            if (ImGui::MenuItem("Save Project")) {
                context_->project_manager->get_current_project()->save();
            }
            ImGui::EndMenu();
        }
    }

    void MenuBarComponent::render_scene_menu() {
        if (ImGui::BeginMenu("Scene")) {
            handle_new_scene();
            handle_open_scene();

            ImGui::Separator();

            render_scene_list();
            ImGui::EndMenu();
        }
    }

    void MenuBarComponent::handle_new_scene() {
        if (ImGui::MenuItem("New Scene")) {
            create_default_scene();
        }
    }

    void MenuBarComponent::handle_open_scene() const {
        if (ImGui::MenuItem("Open Scene")) {
            const auto sm = ServiceLocator::get_service<SceneManager>();
            Utility::FileFilter scene_ext_filter = {"Hellfire Scene", "*.hfscene"};
            const std::string filepath = Utility::FileDialog::open_file({scene_ext_filter});
            if (!filepath.empty()) {
                const auto scene = sm->load_scene(filepath);
                sm->set_active_scene(scene);

                if (context_) {
                    context_->set_window_title(scene->get_name());
                }
            }
        }
    }

    void MenuBarComponent::render_scene_list() const {
        const auto sm = ServiceLocator::get_service<SceneManager>();
        for (const auto element: sm->get_scenes()) {
            const bool is_selected = element == sm->get_active_scene();
            if (ImGui::MenuItem(element->get_name().c_str(), nullptr, is_selected)) {
                if (!is_selected) {
                    sm->set_active_scene(element);

                    if (context_) {
                        context_->set_window_title(element->get_name());
                    }
                }
            }
        }
    }
}
