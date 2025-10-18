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

inline bool TextureSlotWidget(
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

        // Thumbnail preview
        if (current_texture && current_texture->is_valid()) {
            ImGui::Image(
                current_texture->get_id(),
                ImVec2(64, 64)
            );
            ImGui::SameLine();
        }

        // Button column
        ImGui::BeginGroup();
        
        // Choose/Change button
        if (ImGui::Button(current_texture ? "Change##tex" : "Choose##tex", ImVec2(120, 0))) {
            Utility::FileFilter filter = {"Images", "*.png;*.jpg;*.jpeg;*.tga;*.bmp"};
            const std::string path = Utility::FileDialog::open_file({filter});
            
            if (!path.empty()) {
                TextureSlotManager::enable_slot(*material, slot, path);
                changed = true;
            }
        }

        // Clear button (only if texture exists)
        if (current_texture) {
            if (ImGui::Button("Clear##tex", ImVec2(120, 0))) {
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
