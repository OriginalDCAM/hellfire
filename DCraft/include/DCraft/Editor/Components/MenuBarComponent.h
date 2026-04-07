#pragma once
#include <imgui.h>
#include <string>
#include <vector>
#include <memory>

#include "DCraft/Addons/ImportedModel3D.h"
#include "DCraft/Addons/ModelLoader.h"
#include "DCraft/Editor/Components/UIComponent.h"
#include "DCraft/Utility/FileDialog.h"

namespace DCraft {
    class SceneManager;
}

namespace DCraft {
    class MenuBarComponent : public UIComponent {
    public:
        MenuBarComponent() = default;

        void init(SceneManager &sm, std::vector<std::unique_ptr<EditorCommand> > &command_history,
                  int &current_command_index);

        void render() override;

    private:
        SceneManager *scene_manager_ = nullptr;
        std::vector<std::unique_ptr<EditorCommand> > *command_history_ = nullptr;
        int *current_command_index_ = nullptr;

        // Helper methods
        std::string open_file_dialog(std::vector<Utility::FileFilter> filters) const;

        std::string save_file_dialog(const std::string &default_name) const;

        void import_model(const std::string &filepath);

        void undo();

        void redo();
    };

    // Implementation
    inline void MenuBarComponent::init(SceneManager &sm,
                                       std::vector<std::unique_ptr<EditorCommand> > &command_history,
                                       int &current_command_index) {
        scene_manager_ = &sm;
        command_history_ = &command_history;
        current_command_index_ = &current_command_index;
    }

    inline void MenuBarComponent::render() {
        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("Import Model...")) {
                    std::vector<Utility::FileFilter> model_filters = {
                        {"Model Files", "*.obj;*.fbx;*.dae;*.gltf"},
                        {"All Files", "*.*"}
                    };
                    std::string filepath = open_file_dialog(model_filters);
                    if (!filepath.empty()) {
                        import_model(filepath);
                    }
                }
                ImGui::Separator();
                if (ImGui::MenuItem("Save Scene...")) {
                    std::string sceneName = scene_manager_->get_active_scene()->get_name();
                    std::string defaultFilename = sceneName + ".json";

                    std::string filepath = save_file_dialog(sceneName);
                    if (!filepath.empty()) {
                        if (scene_manager_->save_scene(filepath, nullptr)) {
                            std::cout << "Saved scene: " << sceneName << std::endl;
                        } else {
                            std::cerr << "FAILED TO SAVE SCENE" << std::endl;
                        }
                    }
                }

                if (ImGui::MenuItem("Load Scene...")) {
                    std::vector<Utility::FileFilter> scene_filters = {
                        {"Scene Files", "*.json;"},
                        {"All Files", "*.*"}
                    };
                    std::string filepath = open_file_dialog(scene_filters);
                    if (!filepath.empty()) {
                        if (auto* scene = scene_manager_->load_scene(filepath)) {
                            std::clog << "Scene loaded successfully from path:" << filepath << std::endl;
                        } else {
                            std::clog << "Failed to load Scene from path:" << filepath << std::endl;
                        }
                    }
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Edit")) {
                if (ImGui::MenuItem("Undo", "Ctrl+Z", false, *current_command_index_ >= 0)) {
                    undo();
                }
                if (ImGui::MenuItem("Redo", "Ctrl+Y", false,
                                    *current_command_index_ < command_history_->size() - 1)) {
                    redo();
                }
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }
    }

    inline std::string MenuBarComponent::open_file_dialog(std::vector<Utility::FileFilter> filters) const {
        return Utility::FileDialog::open_file(filters);
    }

    inline std::string MenuBarComponent::save_file_dialog(const std::string &default_name) const {
        std::vector<Utility::FileFilter> filters = {
            {"Scene Files", "*.json"},
            {"All Files", "*.*"}
        };
        return Utility::FileDialog::save_file(default_name, filters);
    }

    inline void MenuBarComponent::import_model(const std::string &filepath) {
        auto model = Addons::ModelLoader::load(filepath, scene_manager_->get_active_scene());

        if (model) {
            scene_manager_->get_active_scene()->add(model);
        }
    }

    inline void MenuBarComponent::undo() {
        if (*current_command_index_ >= 0) {
            (*command_history_)[*current_command_index_]->undo();
            (*current_command_index_)--;
        }
    }

    inline void MenuBarComponent::redo() {
        if (*current_command_index_ < command_history_->size() - 1) {
            (*current_command_index_)++;
            (*command_history_)[*current_command_index_]->execute();
        }
    }
} // namespace DCraft
