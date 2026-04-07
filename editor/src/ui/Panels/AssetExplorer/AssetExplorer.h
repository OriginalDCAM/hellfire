//
// Created by denzel on 09/12/2025.
//
#pragma once

#include "hellfire/assets/models/ModelInstantiator.h"
#include "hellfire/graphics/managers/TextureManager.h"
#include "ui/Panels/EditorPanel.h"

namespace hellfire::editor {
class AssetExplorer : public EditorPanel {
public:
     AssetExplorer() {
        model_texture_ = TextureCache::load("assets/thumbnails/model_thumbnail.png");
    }
    
    void render() override;



 private:
     std::shared_ptr<Texture> model_texture_ = nullptr;
    AssetID selected_asset_ = -1;
    float thumbnail_size_ = 80.0f;
    
     void render_toolbar();
    void render_asset_tile(const AssetMetadata &asset, float size, ModelInstantiator &instantiator);

     static std::string truncate_string(const std::string & name, int i);


     void open_in_explorer(const std::filesystem::path &path);
};

} // hellfire