//
// Created by denzel on 17/10/2025.
//

#pragma once
#include <string>

#include "imgui.h"
#include "Components/Inspector/TextureSlotManager.h"
#include "glm/detail/type_vec.hpp"
#include "glm/detail/type_vec2.hpp"
#include "glm/detail/type_vec3.hpp"
#include "glm/detail/type_vec4.hpp"
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
    inline bool float_input(const std::string &label, float *value, float speed = 1.0f, float min = 0.0f,
                           float max = FLT_MAX) {
        return Property(label, value, [&](const char *id, float *val) {
            return ImGui::DragFloat(id, val, speed, min, max);
        });
    }

    inline bool vec2_input(const std::string &label, glm::vec2 *value, float speed = 1.0f, float min = 0.0f,
                          float max = FLT_MAX) {
        return Property(label, value, [&](const char *id, glm::vec2 *val) {
            return ImGui::DragFloat2(id, &(*val)[0], speed, min, max);
        });
    }

    inline bool vec3_input(const std::string &label, glm::vec3 *value, float speed = 1.0f, float min = 0.0f,
                          float max = FLT_MAX) {
        return Property(label, value, [&](const char *id, glm::vec3 *val) {
            return ImGui::DragFloat3(id, &(*val)[0], speed, min, max);
        });
    }

    inline bool combo_box_int(const std::string &label, const char *labels, int* value) {
        return Property(label, value, [&](const char *id, int* val) {
            return ImGui::Combo(id, val, labels);
        });
    }

    inline bool color_picker_rgb_input(const std::string &label, glm::vec3 *value) {
        return Property(label, value, [&](const char *id, glm::vec3 *val) {
            return ImGui::ColorEdit3(id, &(*val)[0]);
        });
    }

    inline bool color_picker_rgba_input(const std::string &label, glm::vec4 *value) {
        return Property(label, value, [&](const char *id, glm::vec4 *val) {
            return ImGui::ColorEdit4(id, &(*val)[0]);
        });
    }

    inline bool bool_input(const std::string &label, bool *value) {
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

inline bool texture_slot_widget(
    Material* material, 
    const TextureSlotManager::SlotInfo& slot
) {
    ImGui::PushID(slot.uniform_name);
    bool changed = false;

    // Get current state
    const bool is_enabled = TextureSlotManager::is_slot_enabled(*material, slot);
    Texture* current_texture = TextureSlotManager::get_slot_texture(*material, slot);

    // Checkbox to enable/disable
    bool enabled_copy = is_enabled;
    if (ImGui::Checkbox(slot.display_name, &enabled_copy)) {
        material->set_property(slot.flag_name, enabled_copy);
        
        if (!enabled_copy && current_texture) {
            // User disabled - clear texture
            TextureSlotManager::disable_slot(*material, slot);
        }
        changed = true;
    }

    // Show texture controls when enabled
    if (enabled_copy) {
        ImGui::Indent();
        ImGui::Separator();

        // Thumbnail preview
        if (current_texture && current_texture->is_valid()) {
            ImGui::Image(
                current_texture->get_id(),
                ImVec2(24, 24), ImVec2(0, 1), ImVec2(1, 0)
            );
            ImGui::SameLine();
        }

        // Button column
        ImGui::BeginGroup();
        
        // Choose/Change button
        if (ImGui::Button(current_texture ? "Change##tex" : "Choose##tex", ImVec2(120, 24))) {
            Utility::FileFilter filter = {"Images", "*.png;*.jpg;*.jpeg;*.tga;*.bmp"};
            const std::string path = Utility::FileDialog::open_file({filter});
            
            if (!path.empty()) {
                TextureSlotManager::enable_slot(*material, slot, path);
                changed = true;
            }
        }

        ImGui::SameLine();

        // Clear button (only if texture exists)
        if (current_texture) {
            if (ImGui::Button("Clear##tex", ImVec2(120, 24))) {
                TextureSlotManager::disable_slot(*material, slot,  true);
                changed = true;
            }
        }
        
        ImGui::EndGroup();
        ImGui::Unindent();
    }

    ImGui::PopID();
    return changed;
}
}
