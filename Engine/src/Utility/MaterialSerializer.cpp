//
// Created by denzel on 11/04/2025.
//

#include "DCraft/Utility/MaterialSerializer.h"

using namespace hellfire;


json MaterialSerializer::serialize_material(Material *material) {
//     if (!material) return json();
//
//     json mat_data;
//     mat_data["name"] = material->get_name();
//
//     if (auto *lambert = dynamic_cast<LambertMaterial *>(material)) {
//         mat_data["type"] = "lambert";
//         mat_data["ambient"] = {
//             lambert->get_ambient_color().r,
//             lambert->get_ambient_color().g,
//             lambert->get_ambient_color().b
//         };
//         mat_data["diffuse"] = {
//             lambert->get_diffuse_color().r,
//             lambert->get_diffuse_color().g,
//             lambert->get_diffuse_color().b
//         };
//     } else if (auto *phong = dynamic_cast<PhongMaterial *>(material)) {
//         mat_data["type"] = "phong";
//         mat_data["ambient"] = {
//             phong->get_ambient_color().r,
//             phong->get_ambient_color().g,
//             phong->get_ambient_color().b
//         };
//         mat_data["diffuse"] = {
//             phong->get_diffuse_color().r,
//             phong->get_diffuse_color().g,
//             phong->get_diffuse_color().b
//         };
//         mat_data["specular"] = {
//             phong->get_specular_color().r,
//             phong->get_specular_color().g,
//             phong->get_specular_color().b
//         };
//         mat_data["shininess"] = phong->get_shininess();
//     }
//
//     // Serialize textures
//     json textures = json::array();
//     for (const auto &texture: material->get_textures()) {
//         json tex_data;
//         tex_data["path"] = texture->get_path();
//         tex_data["type"] = texture->get_type();
//         textures.push_back(tex_data);
//     }
//
//     if (!textures.empty()) {
//         mat_data["textures"] = textures;
//     }
//
//     return mat_data;
    return json::array();
}
//
Material *MaterialSerializer::deserialize_material(const json &data) {
//     if (!data.contains("type")) {
//         return nullptr;
//     }
//
//     std::string type = data["type"];
//     Material *material = nullptr;
//
//     if (type == "lambert") {
//         material = new LambertMaterial("Lambert Material");
//     } else if (type == "phong") {
//         material = new PhongMaterial("Phong Material");
//     } else {
//         return nullptr;
//     }
//
//     // Set common properties
//     if (data.contains("name")) {
//         material->set_name(data["name"]);
//     }
//
//     // Set type-specific properties
//     if (type == "lambert" || type == "phong") {
//         if (data.contains("ambient") && data["ambient"].is_array() && data["ambient"].size() == 3) {
//             glm::vec3 ambient(
//                 data["ambient"][0],
//                 data["ambient"][1],
//                 data["ambient"][2]
//             );
//             material->set_ambient_color(ambient);
//         }
//
//         if (data.contains("diffuse") && data["diffuse"].is_array() && data["diffuse"].size() == 3) {
//             glm::vec3 diffuse(
//                 data["diffuse"][0],
//                 data["diffuse"][1],
//                 data["diffuse"][2]
//             );
//             material->set_diffuse_color(diffuse);
//         }
//     }
//
//     if (type == "phong") {
//         auto *phong = static_cast<PhongMaterial *>(material);
//
//         if (data.contains("specular") && data["specular"].is_array() && data["specular"].size() == 3) {
//             glm::vec3 specular(
//                 data["specular"][0],
//                 data["specular"][1],
//                 data["specular"][2]
//             );
//             phong->set_specular_color(specular);
//         }
//
//         if (data.contains("shininess")) {
//             phong->set_shininess(data["shininess"]);
//         }
//     }
//
//     // Load textures
//     if (data.contains("textures") && data["textures"].is_array()) {
//         for (const auto &texture_data: data["textures"]) {
//             if (texture_data.contains("path") && texture_data.contains("type")) {
//                 std::string path = texture_data["path"];
//                 int type = texture_data["type"];
//                 material->set_texture(path, static_cast<TextureType>(type));
//             }
//         }
//     }
//
//     return material;
    // TODO: FIX this soon
    return nullptr;
}
