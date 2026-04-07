//
// Created by denzel on 09/12/2025.
//

#include "AssetExplorer.h"

#include "ui/ui.h"

namespace hellfire::editor {
    void AssetExplorer::render() {
        if (ui::Window window {"Asset Browser"}) {
            if (auto asset_registry = ServiceLocator::get_service<AssetRegistry>()) {
                auto assets = asset_registry->get_all_assets();

                for (auto asset : assets) {
                    ImGui::Text(asset.name.c_str());
                    ImGui::SameLine();
                    ImGui::Text("%u", asset.type);
                }
            }
        }
    }

} // hellfire