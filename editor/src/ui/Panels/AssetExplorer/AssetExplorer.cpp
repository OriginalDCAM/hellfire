//
// Created by denzel on 09/12/2025.
//

#include "AssetExplorer.h"



#if WIN32
#include <windows.h>
#include <shellapi.h>
#endif

#include "hellfire/assets/models/ModelInstantiator.h"
#include "ui/ui.h"

namespace hellfire::editor {
    void AssetExplorer::render() {
        if (ui::Window window {"Asset Browser"}) {
            if (auto asset_registry = ServiceLocator::get_service<AssetRegistry>(); auto asset_manager = ServiceLocator::get_service<AssetManager>()) {
                auto assets = asset_registry->get_assets_by_type(AssetType::MODEL);
                ModelInstantiator instantiator(*asset_manager, *asset_registry);
                for (auto& asset : assets) {
                    if (asset.uuid == INVALID_ASSET_ID || asset.filepath.extension() != ".hfmodel") continue;
                    
                    ImGui::PushID(asset.uuid);
                    ImGui::Text(asset.name.c_str());
                    ImGui::Text("Model");
                    if (ImGui::Button("Instantiate")) {
                        instantiator.instantiate(*context_->active_scene, asset.uuid);
                    };
                    
                    
                    if (ImGui::BeginPopupContextWindow("##")) {
                        if (ImGui::MenuItem("Open in Explorer")) {
#if WIN32
                            ShellExecuteW(NULL, L"open", asset_registry->get_project_root().c_str(), NULL, NULL, SW_SHOW);
#endif
                        }
                        ImGui::EndPopup();
                    }
                    ImGui::PopID();
                    
                }
            }
        }
    }

} // hellfire