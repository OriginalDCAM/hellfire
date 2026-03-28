//
// Created by denzel on 09/12/2025.
//

#include "AssetExplorer.h"


#if WIN32
#define NOMINMAX
#include <windows.h>
#include <shellapi.h>
#endif

#include "hellfire/assets/models/ModelInstantiator.h"
#include "ui/ui.h"

namespace hellfire::editor {
    void AssetExplorer::render() {
        if (ui::Window window{"Asset Browser"}) {
            const auto asset_registry = ServiceLocator::get_service<AssetRegistry>();
            const auto asset_manager = ServiceLocator::get_service<AssetManager>();
            if (!asset_registry || !asset_manager) return;

            render_toolbar();
            ImGui::Separator();

            // Calculate grid layout
            float thumbnail_size = thumbnail_size_;
            float padding = 8;
            float cell_size = thumbnail_size + padding;
            float panel_width = ImGui::GetContentRegionAvail().x;
            int columns = std::max(1, static_cast<int>(panel_width / cell_size));

            auto assets = asset_registry->get_assets_by_type(AssetType::MODEL);
            ModelInstantiator instantiator(*asset_manager, *asset_registry);

            ImGui::Columns(columns, nullptr, false);

            for (auto &asset: assets) {
                if (asset.uuid == INVALID_ASSET_ID || asset.filepath.extension() != ".hfmodel") {
                    continue;
                }

                render_asset_tile(asset, thumbnail_size, instantiator);
                ImGui::NextColumn();
            }

            ImGui::Columns(1);
        }
    }

    void AssetExplorer::render_toolbar() {
    }

    void AssetExplorer::render_asset_tile(const AssetMetadata &asset, const float size,
                                          ModelInstantiator &instantiator) {
        ImGui::PushID(asset.uuid);

        ImGui::BeginGroup();

        ImVec2 cursor_pos = ImGui::GetCursorScreenPos();
        bool is_selected = (selected_asset_ == asset.uuid);
        bool is_hovered = false;

        if (ImGui::InvisibleButton("##tile", ImVec2(size, size + 20))) {
            selected_asset_ = asset.uuid;
        }
        is_hovered = ImGui::IsItemHovered();

        // Draw background
        ImDrawList *draw_list = ImGui::GetWindowDrawList();
        ImU32 bg_color = is_selected
                             ? IM_COL32(70, 130, 180, 255)
                             : is_hovered
                                   ? IM_COL32(60, 60, 60, 255)
                                   : IM_COL32(40, 40, 40, 255);
        draw_list->AddRectFilled(cursor_pos,
                                 ImVec2(cursor_pos.x + size, cursor_pos.y + size + 20),
                                 bg_color, 4.0f);

        // Draw thumbnail
        draw_list->AddImage(model_texture_->get_id(),
                            ImVec2(cursor_pos.x + 4, cursor_pos.y + 4),
                            ImVec2(cursor_pos.x + size - 4, cursor_pos.y + size - 4));

        // Draw label (truncated)
        std::string display_name = truncate_string(asset.name, 12);
        ImVec2 text_size = ImGui::CalcTextSize(display_name.c_str());
        float text_x = cursor_pos.x + (size - text_size.x) * 0.5f;
        draw_list->AddText(ImVec2(text_x, cursor_pos.y + size + 2),
                           IM_COL32(255, 255, 255, 255), display_name.c_str());

        ImGui::EndGroup();

        // Double-click to instantiate
        if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0) && context_->active_scene) {
            instantiator.instantiate(*context_->active_scene, asset.uuid);
        }

        // Drag source for drag-and-drop
        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
            ImGui::SetDragDropPayload("ASSET_MODEL", &asset.uuid, sizeof(AssetID));
            ImGui::Image(model_texture_->get_id(), ImVec2(32, 32));
            ImGui::SameLine();
            ImGui::Text("%s", asset.name.c_str());
            ImGui::EndDragDropSource();
        }

        // Context menu
        if (ImGui::BeginPopupContextItem(("##" + asset.filepath.string()).c_str())) {
            if (ImGui::MenuItem("Instantiate")) {
                instantiator.instantiate(*context_->active_scene, asset.uuid);
            }
            if (ImGui::MenuItem("Show in Explorer")) {
                open_in_explorer(asset.filepath.parent_path());
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Rename")) {
                // TODO: Implement this
            }
            if (ImGui::MenuItem("Delete", nullptr, false, false)) {
                // TODO: Implement this
            }
            ImGui::EndPopup();
        }

        // Tooltip on hover
        if (is_hovered && ImGui::BeginTooltip()) {
            ImGui::Text("%s", asset.name.c_str());
            ImGui::TextDisabled("Path: %s", asset.filepath.string().c_str());
            ImGui::EndTooltip();
        }

        ImGui::PopID();
    }

        std::string AssetExplorer::truncate_string(const std::string &name, int i) {
        return name.substr(0, i);
    }

    void AssetExplorer::open_in_explorer(const std::filesystem::path &path) {
        const auto absolute_path = context_->project_manager->get_current_project()->get_project_root() / path;
#if WIN32
        ShellExecuteW(NULL, L"open", absolute_path.c_str(), NULL, NULL,
                      SW_SHOW);
#endif
    }
} // hellfire
