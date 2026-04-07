//
// Created by denzel on 01/10/2025.
//
#include "MenuBarComponent.h"

#include <iostream>
#include "imgui.h"
#include "hellfire/scene/Scene.h"
#include "hellfire/scene/SceneManager.h"
#include "hellfire/utilities/FileDialog.h"
#include "hellfire/utilities/ServiceLocator.h"

namespace hellfire::editor {
    void MenuBarComponent::render() {
        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("New Project")) {
                    std::cout << "New Project" << std::endl;
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Scene")) {
                if (ImGui::MenuItem("New Scene")) {
                    auto sm = ServiceLocator::get_service<SceneManager>();
                    Utility::FileFilter scene_ext_filter = {"Hellfire Scene", "*.hfscene"};
                    std::string scene_name = "";
                    std::string save_path = Utility::FileDialog::save_file(scene_name, "Untitled", {scene_ext_filter});
                    if (scene_name != "") {
                        auto new_scene = sm->create_scene(scene_name);

                        sm->save_scene(save_path, new_scene);
                    }
                }

                if (ImGui::MenuItem("Open Scene")) {
                    auto sm = ServiceLocator::get_service<SceneManager>();
                    Utility::FileFilter scene_ext_filter = {"Hellfire Scene", "*.hfscene"};
                    std::string filepath = Utility::FileDialog::open_file({scene_ext_filter});
                    if (filepath != "") {
                       auto scene = sm->load_scene(filepath);
                        sm->set_active_scene(scene);
                    }
                }

                ImGui::Separator();

                for (auto element: ServiceLocator::get_service<SceneManager>()->get_scenes()) {
                    bool is_selected = (element == ServiceLocator::get_service<SceneManager>()->get_active_scene());
                    if (ImGui::MenuItem(element->get_name().c_str(), nullptr, is_selected)) {
                        if (!is_selected) {
                            ServiceLocator::get_service<SceneManager>()->set_active_scene(element);
                        }
                    }
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }
    }
}
