//
// Created by denzel on 05/12/2025.
//

#pragma once
#include <concepts>
#include <memory>
#include <vector>

#include "Panels/EditorPanel.h"

namespace hellfire::editor {
    template<typename  T>
    concept IsPanel = std::derived_from<T, EditorPanel>;
    
    class PanelManager {
    public:
        template<IsPanel T, typename... Args>
        T* add_panel(Args&&... args) {
            auto panel = std::make_unique<T>(std::forward<Args>(args)...);
            T* ptr = panel.get();
            panels_.push_back(std::move(panel));
            return ptr;
        }

        void set_context(EditorContext* ctx) {
            for (auto& panel : panels_) {
                panel->set_context(ctx);
            }
        }

        void render_all() const {
            for (const auto& panel : panels_) {
                panel->render();
            }
        }

    private:
        std::vector<std::unique_ptr<EditorPanel>> panels_;
    };
}
