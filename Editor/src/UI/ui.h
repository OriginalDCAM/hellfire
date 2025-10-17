//
// Created by denzel on 17/10/2025.
//

#pragma once
#include <string>

#include "imgui.h"
#include "glm/detail/type_vec.hpp"
#include "glm/detail/type_vec2.hpp"
#include "glm/detail/type_vec3.hpp"
#include "glm/detail/type_vec4.hpp"
#include "hellfire/graphics/managers/TextureManager.h"
#include "hellfire/utilities/FileDialog.h"

namespace hellfire::editor::ui {
    // Method for making labels
    inline std::string make_label(const std::string &label) {
        return "##" + label;
    }

    // Generic property drawer with minimal overhead
    template<typename T, typename DrawFunc>
    bool Property(const std::string &label, T *value, DrawFunc draw_func) {
        ImGui::Text("%s", label.c_str());
        ImGui::SameLine(180);
        return draw_func(make_label(label).c_str(), value);
    }

    // Specific type handlers
    inline bool FloatInput(const std::string &label, float *value, float speed = 1.0f, float min = 0.0f,
                           float max = FLT_MAX) {
        return Property(label, value, [&](const char *id, float *val) {
            return ImGui::DragFloat(id, val, speed, min, max);
        });
    }

    inline bool Vec2Input(const std::string &label, glm::vec2 *value, float speed = 1.0f, float min = 0.0f,
                          float max = FLT_MAX) {
        return Property(label, value, [&](const char *id, glm::vec2 *val) {
            return ImGui::DragFloat2(id, &(*val)[0], speed, min, max);
        });
    }

    inline bool Vec3Input(const std::string &label, glm::vec3 *value, float speed = 1.0f, float min = 0.0f,
                          float max = FLT_MAX) {
        return Property(label, value, [&](const char *id, glm::vec3 *val) {
            return ImGui::DragFloat3(id, &(*val)[0], speed, min, max);
        });
    }

    inline bool ColorPickerRGBInput(const std::string &label, glm::vec3 *value) {
        return Property(label, value, [&](const char *id, glm::vec3 *val) {
            return ImGui::ColorEdit3(id, &(*val)[0]);
        });
    }

    inline bool ColorPickerRGBAInput(const std::string &label, glm::vec4 *value) {
        return Property(label, value, [&](const char *id, glm::vec4 *val) {
            return ImGui::ColorEdit4(id, &(*val)[0]);
        });
    }

    inline bool BoolInput(const std::string &label, bool *value) {
        return Property(label, value, [&](const char *id, bool *val) {
            return ImGui::Checkbox(id, val);
        });
    }

    
    inline std::string capitalize_first(const std::string &str) {
        if (str.empty()) return str;
        std::string result = str;
        result[0] = std::toupper(result[0]);
        return result;
    }

    inline bool TexturePropertyInput(const std::string &label, Texture *&texture,  bool& property_enabled, Material *material = nullptr) {
        // Push unique ID to prevent conflicts
        ImGui::PushID(label.c_str());
        bool value_changed = false;

        // Checkbox to enable/disable texture
        if (BoolInput(label, &property_enabled)) {
            if (!property_enabled) {
                texture = nullptr;
            }
            value_changed = true;
        }

        // If the texture is enabled, show additional controls
        if (property_enabled) {
            ImGui::Indent();

            // Display current texture thumbnail
            if (texture) {
                ImGui::Text("Current Texture");
                ImGui::SameLine(120);
                ImGui::Image(texture->get_id(), ImVec2(64, 64));
            } else {
                ImGui::Text("No Texture Selected");
            }

            // Button to change texture
            ImGui::Text("Texture");
            ImGui::SameLine(120);
            if (ImGui::Button("Choose Texture")) {
                Utility::FileFilter image_ext_filter = {"Image files", "*.png;*.jpg;*.tga;"};
                const std::string selected_path = Utility::FileDialog::open_file({image_ext_filter});
                if (!selected_path.empty()) {
                    // Load new texture
                    const auto new_texture = new Texture(selected_path);
                    texture = new_texture;
                    value_changed = true;
                    property_enabled = true;
                }
            }
            ImGui::Unindent();
        }
        ImGui::PopID();

        return value_changed;
    }

    template<typename... Ts>
    bool render_property(const std::string &name, std::variant<Ts...> &value) {
        return std::visit([&](auto &arg) {
            if constexpr (std::is_same_v<decltype(arg), float>) {
                return ui::FloatInput(name, &arg);
            } else if constexpr (std::is_same_v<decltype(arg), glm::vec2>) {
                return ui::Vec2Input(name, &arg);
            } else if constexpr (std::is_same_v<decltype(arg), glm::vec3>) {
                return ui::ColorPickerRGBInput(name, &arg);
            } else if constexpr (std::is_same_v<decltype(arg), bool>) {
                return ui::BoolInput(name, &arg);
            }
            return false;
        }, value);
    }
}
