//
// Created by denzel on 09/12/2025.
//
#pragma once

#include "hellfire/graphics/managers/TextureManager.h"
#include "ui/Panels/EditorPanel.h"

namespace hellfire::editor {
class AssetExplorer : public EditorPanel {
public:
    void render() override;
};

} // hellfire