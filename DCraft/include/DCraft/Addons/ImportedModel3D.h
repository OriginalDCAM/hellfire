//
// Created by denzel on 10/04/2025.
//

#pragma once
#include "DCraft/Graphics/Primitives/MeshRenderer.h"

namespace DCraft {
class ImportedModel3D : public MeshRenderer {
public:
    ImportedModel3D() = default;
    explicit ImportedModel3D(const std::string &name) : MeshRenderer(name) {}
    std::filesystem::path& get_file_path() { return filepath_; }
    void set_file_path(const std::string& path) { filepath_ = path; }

    json to_json() override;
private:
    std::filesystem::path filepath_;
    
};

inline json ImportedModel3D::to_json() {
    json j = Object3D::to_json();
    j["type"] = "Model";
    j["path"] = get_file_path();
    return j;
}
}
    
