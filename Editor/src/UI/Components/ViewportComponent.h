//
// Created by denzel on 13/10/2025.
//

#pragma once
#include "EditorComponent.h"

namespace hellfire::editor {
    
class ViewportComponent final : public EditorComponent {
public:
    void render() override;

    void render_viewport_stats_overlay();
};
    
}
