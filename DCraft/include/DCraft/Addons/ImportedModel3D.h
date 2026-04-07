//
// Created by denzel on 10/04/2025.
//

#pragma once
#include "DCraft/Graphics/Primitives/Shape3D.h"

namespace DCraft {
class ImportedModel3D : public Shape3D {
public:
    explicit ImportedModel3D(const std::string &name) : Shape3D(name) {}
    std::string& get_file_path() { return filepath_; }
    void set_file_path(const std::string& path) { filepath_ = path; }

    json to_json() override;
private:
    std::string filepath_;
    
};

inline json ImportedModel3D::to_json() {
    json j = Object3D::to_json();
    j["type"] = "model";
    j["path"] = get_file_path();
    return j;
}
}
    
