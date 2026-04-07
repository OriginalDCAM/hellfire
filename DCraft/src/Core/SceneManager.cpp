#include "DCraft/Addons/SceneManager.h"

#include <fstream>

#include "DCraft/Addons/ImportedModel3D.h"
#include "DCraft/Addons/ModelLoader.h"
#include "DCraft/Addons/PerspectiveCamera.h"
#include "DCraft/Graphics/Lights/DirectionalLight.h"
#include "DCraft/Graphics/Lights/PointLight.h"
#include "DCraft/Graphics/Materials/LambertMaterial.h"
#include "DCraft/Graphics/Materials/PhongMaterial.h"
#include "DCraft/Structs/Scene.h"
#include "DCraft/Graphics/Primitives/Cube.h"
#include "DCraft/Graphics/Primitives/Plane.h"

namespace DCraft {
    SceneManager::SceneManager() : active_scene_(nullptr) {
        root_node_ = new Object3D();
        root_node_->set_name("Root");
    }

    SceneManager::~SceneManager() {
        clear();
        delete root_node_;
    }

    void SceneManager::create_default_scene() {
        clear();

        root_node_ = new Object3D();
        root_node_->set_name("Root");

        auto *camera_obj = create_object("MainCamera");
        auto *camera = new PerspectiveCamera("PerspectiveCamera", 45.0f, 16.0f / 9.0f, 0.1f, 100.0f);
        camera_obj->add(camera);
        camera_obj->set_position(glm::vec3(0, 0, 10));

        set_active_camera(camera);

        auto *cube_obj = create_object("Cube");
        Cube *cube = new Cube("Cube");
        cube_obj->add(cube);
    }

    bool SceneManager::load_scene(const std::string &filename) {
        // Open and parse the JSON file
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "FILE::ERROR Failed to open scene file: " << filename << std::endl;
            return false;
        }

        json scene_data;
        try {
            file >> scene_data;
        } catch (const std::exception &e) {
            std::cerr << "FILE::ERROR Failed to parse scene file: " << e.what() << std::endl;
            return false;
        }

        // Create a new scene
        std::string scene_name = scene_data.contains("name") ? scene_data["name"] : "Untitled Scene";
        Scene *scene = create_scene(scene_name);

        // Load all objects into the scene
        if (scene_data.contains("objects") && scene_data["objects"].is_array()) {
            for (const auto &obj_data: scene_data["objects"]) {
                Object3D *obj = deserialize_node(obj_data);
                if (obj) {
                    scene->add(obj);
                }
            }
        }

        // Set as active scene
        set_active_scene(scene);

        // Find the Main Camera by name and set it as active
        PerspectiveCamera *main_camera = dynamic_cast<PerspectiveCamera *>(
            scene->find_object_by_name("Main Camera"));

        if (main_camera) {
            scene->set_active_camera(main_camera);
        } else {
            std::cerr << "SCENE::WARNING No 'Main Camera' found in scene, rendering may not work properly." <<
                    std::endl;
        }

        return true;
    }

    Object3D *SceneManager::deserialize_node(const json &node_data) {
        if (!node_data.contains("type")) {
            std::cerr << "NODE::ERROR Missing type for object" << std::endl;
            return nullptr;
        }

        Object3D *obj = nullptr;
        std::string type = node_data["type"];

        // Create the appropriate object based on type
        if (type == "Shape3D") {
            if (node_data.contains("geometry")) {
                std::string geometry = node_data["geometry"];
                if (geometry == "cube") {
                    obj = new Cube("Default cube");
                } else if (geometry == "plane") {
                    obj = new Plane("Default plane");
                } else {
                    std::cerr << "GEOMETRY::ERROR Unknown geometry type: " << geometry << std::endl;
                    return nullptr;
                }
            } else {
                obj = new Shape3D();
            }
        } else if (type == "Camera") {
            // Create camera
            std::string camera_type = node_data.contains("camera_type") ? node_data["camera_type"] : "perspective";

            if (camera_type == "perspective") {
                // Get the current scene to create the camera
                Scene *current_scene = get_active_scene();

                // Extract camera properties from JSON
                std::string name = node_data.contains("name") ? node_data["name"].get<std::string>() : "Default Camera";
                float fov = node_data.contains("fov") ? node_data["fov"].get<float>() : 70.0f;
                float aspect = node_data.contains("aspect_ratio") ? node_data["aspect_ratio"].get<float>() : 1.0f;
                float near_plane = node_data.contains("near_plane") ? node_data["near_plane"].get<float>() : 0.1f;
                float far_plane = node_data.contains("far_plane") ? node_data["far_plane"].get<float>() : 400.0f;

                // Create the camera using the scene's create_camera method
                PerspectiveCamera *camera = current_scene->create_camera<PerspectiveCamera>(
                    name, fov, aspect, near_plane, far_plane);

                // Set other camera properties
                if (node_data.contains("transform") && node_data["transform"].contains("position") &&
                    node_data["transform"]["position"].is_array() && node_data["transform"]["position"].size() == 3) {
                    camera->set_position(
                        node_data["transform"]["position"][0],
                        node_data["transform"]["position"][1],
                        node_data["transform"]["position"][2]
                    );
                }

                // Set target if available
                if (node_data.contains("target") && node_data["target"].is_array() && node_data["target"].size() == 3) {
                    camera->set_target(
                        node_data["target"][0],
                        node_data["target"][1],
                        node_data["target"][2]
                    );
                }

                // Set other camera-specific properties
                if (node_data.contains("front") && node_data["front"].is_array() && node_data["front"].size() == 3) {
                    glm::vec3 front(
                        node_data["front"][0],
                        node_data["front"][1],
                        node_data["front"][2]
                    );
                    camera->set_front(front);
                }

                if (node_data.contains("up") && node_data["up"].is_array() && node_data["up"].size() == 3) {
                    glm::vec3 up(
                        node_data["up"][0],
                        node_data["up"][1],
                        node_data["up"][2]
                    );
                    camera->set_up(up);
                }

                if (node_data.contains("yaw")) camera->set_yaw(node_data["yaw"]);
                if (node_data.contains("pitch")) camera->set_pitch(node_data["pitch"]);
                if (node_data.contains("movement_speed")) camera->set_movement_speed(node_data["movement_speed"]);
                if (node_data.contains("mouse_sensitivity"))
                    camera->set_mouse_sensitivity(
                        node_data["mouse_sensitivity"]);

                obj = camera;
            } else {
                std::cerr << "CAMERA::ERROR Unknown camera type: " << camera_type << std::endl;
                return nullptr;
            }
        } else if (type == "Model") {
            // Check if it's a model with a path
            if (node_data.contains("path") && !node_data["path"].get<std::string>().empty()) {
                std::string path = node_data["path"];
                // Get current scene to pass to the model loader
                Scene *current_scene = get_active_scene();

                // Create model with ModelLoader
                obj = Addons::ModelLoader::load(path, current_scene);

                // If we successfully loaded the model, set its name and transform
                if (obj) {
                    if (node_data.contains("name")) {
                        obj->set_name(node_data["name"]);
                    }

                    // Set transform
                    if (node_data.contains("transform")) {
                        const auto &transform = node_data["transform"];

                        // Set position
                        if (transform.contains("position") && transform["position"].is_array() && transform["position"].
                            size() == 3) {
                            glm::vec3 position(
                                transform["position"][0],
                                transform["position"][1],
                                transform["position"][2]
                            );
                            obj->set_position(position);
                        }

                        // Set rotation
                        if (transform.contains("rotation") && transform["rotation"].is_array() && transform["rotation"].
                            size() == 3) {
                            glm::vec3 rotation(
                                transform["rotation"][0],
                                transform["rotation"][1],
                                transform["rotation"][2]
                            );
                            obj->set_rotation(rotation);
                        }

                        // Set scale
                        if (transform.contains("scale") && transform["scale"].is_array() && transform["scale"].size() ==
                            3) {
                            glm::vec3 scale(
                                transform["scale"][0],
                                transform["scale"][1],
                                transform["scale"][2]
                            );
                            obj->set_scale(scale);
                        }
                    }

                    if (node_data.contains("material")) {
                    Material* material = deserialize_material(node_data["material"]);
                        if (auto* model = dynamic_cast<ImportedModel3D*>(obj)) {
                            model->set_material(material);
                        }
                    }


                    // Important: Return here to skip processing children for loaded models
                    return obj;
                }
            } else {
                // It's a child model component without its own path
                obj = new ImportedModel3D();
                if (node_data.contains("name")) {
                    obj->set_name(node_data["name"]);
                }
            }
        } else if (type == "Light") {
            // Check if it's a light
            if (node_data.contains("light_type")) {
                std::string light_type = node_data["light_type"];

                if (light_type == "PointLight") {
                    PointLight *light = new PointLight("PointLight");

                    // Set light properties
                    if (node_data.contains("intensity")) light->set_intensity(node_data["intensity"]);
                    if (node_data.contains("range")) light->set_range(node_data["range"]);
                    if (node_data.contains("attenuation")) light->set_attenuation(node_data["attenuation"]);

                    // Set color if present
                    if (node_data.contains("color") && node_data["color"].is_array() && node_data["color"].size() ==
                        3) {
                        glm::vec3 color(
                            node_data["color"][0],
                            node_data["color"][1],
                            node_data["color"][2]
                        );
                        light->set_color(color);
                    }

                    obj = light;
                } else if (light_type == "DirectionalLight") {
                    DirectionalLight *light = new DirectionalLight("DirectionalLight");

                    // Set light properties
                    if (node_data.contains("intensity")) light->set_intensity(node_data["intensity"]);

                    // Set color if present
                    if (node_data.contains("color") && node_data["color"].is_array() && node_data["color"].size() ==
                        3) {
                        glm::vec3 color(
                            node_data["color"][0],
                            node_data["color"][1],
                            node_data["color"][2]
                        );
                        light->set_color(color);
                    }

                    obj = light;
                } else {
                    std::cerr << "LIGHT::ERROR Unknown light type: " << light_type << std::endl;
                    return nullptr;
                }
            } else {
                // Generic Object3D
                obj = new Object3D();
            }
        } else {
            std::cerr << "NODE::ERROR Unknown node type: " << type << std::endl;
            return nullptr;
        }

        // Set common properties
        if (node_data.contains("name")) {
            obj->set_name(node_data["name"]);
        }

        // Set transform
        if (node_data.contains("transform")) {
            const auto &transform = node_data["transform"];

            // Set position
            if (transform.contains("position") && transform["position"].is_array() && transform["position"].size() ==
                3) {
                glm::vec3 position(
                    transform["position"][0],
                    transform["position"][1],
                    transform["position"][2]
                );
                obj->set_position(position);
            }

            // Set rotation
            if (transform.contains("rotation") && transform["rotation"].is_array() && transform["rotation"].size() ==
                3) {
                glm::vec3 rotation(
                    transform["rotation"][0],
                    transform["rotation"][1],
                    transform["rotation"][2]
                );
                obj->set_rotation(rotation);
            }

            // Set scale
            if (transform.contains("scale") && transform["scale"].is_array() && transform["scale"].size() == 3) {
                glm::vec3 scale(
                    transform["scale"][0],
                    transform["scale"][1],
                    transform["scale"][2]
                );
                obj->set_scale(scale);
            }
        }

        // Set material for Shape3D objects
        if (auto *shape = dynamic_cast<Shape3D *>(obj)) {
            if (node_data.contains("material")) {
                Material *material = deserialize_material(node_data["material"]);
                if (material) {
                    shape->set_material(material);
                }
            }
        }

        // Process child nodes
        if (node_data.contains("children") && node_data["children"].is_array()) {
            // Skip processing children for models that were loaded from files
            bool skipChildren = (type == "Model" && node_data.contains("path") && 
                                !node_data["path"].get<std::string>().empty());
    
            if (!skipChildren) {
                for (const auto &child_data: node_data["children"]) {
                    Object3D *child = deserialize_node(child_data);
                    if (child) {
                        obj->add(child);
                    }
                }
            }
        }

        return obj;
    }

    Material *SceneManager::deserialize_material(const json &material_data) {
        if (!material_data.contains("type")) {
            std::cerr << "MATERIAL::ERROR Missing type for material" << std::endl;
            return nullptr;
        }

        std::string type = material_data["type"];
        Material *material = nullptr;

        if (type == "phong") {
            PhongMaterial *phong = new PhongMaterial("PhongMaterial");

            // Set specular and shininess properties specific to PhongMaterial
            if (material_data.contains("specular") && material_data["specular"].is_array() && material_data["specular"].
                size() == 3) {
                glm::vec3 specular(
                    material_data["specular"][0],
                    material_data["specular"][1],
                    material_data["specular"][2]
                );
                phong->set_specular_color(specular);
            }

            if (material_data.contains("shininess")) {
                phong->set_shininess(material_data["shininess"]);
            }

            material = phong;
        } else if (type == "lambert") {
            // Create Lambert material
            material = new LambertMaterial("LambertMaterial");
        } else {
            std::cerr << "MATERIAL::ERROR Unknown material type: " << type << std::endl;
            return nullptr;
        }

        // Set common material properties
        if (material_data.contains("name")) {
            material->set_name(material_data["name"]);
        }

        if (material_data.contains("ambient") && material_data["ambient"].is_array() && material_data["ambient"].size()
            == 3) {
            glm::vec3 ambient(
                material_data["ambient"][0],
                material_data["ambient"][1],
                material_data["ambient"][2]
            );
            material->set_ambient_color(ambient);
        }

        if (material_data.contains("diffuse") && material_data["diffuse"].is_array() && material_data["diffuse"].size()
            == 3) {
            glm::vec3 diffuse(
                material_data["diffuse"][0],
                material_data["diffuse"][1],
                material_data["diffuse"][2]
            );
            material->set_diffuse_color(diffuse);
        }

        // Load textures
        if (material_data.contains("textures") && material_data["textures"].is_array()) {
            for (const auto &texture_data: material_data["textures"]) {
                if (texture_data.contains("path") && texture_data.contains("type")) {
                    std::string path = texture_data["path"];
                    int type = texture_data["type"];
                    material->set_texture(path, static_cast<TextureType>(type));
                }
            }
        }

        return material;
    }

    bool SceneManager::save_scene(const std::string &filepath, Scene *scene = nullptr) {
        if (!scene) scene = get_active_scene();

        json scene_data;
        scene_data["name"] = scene->get_name();
        scene_data["objects"] = json::array();

        // Add all top-level nodes to the objects array
        for (auto *child: scene->get_children()) {
            scene_data["objects"].push_back(serialize_node(child));
        }

        std::ofstream file(filepath);
        if (!file) {
            std::cerr << "FILE::ERROR Something went wrong creating file the file!" << std::endl;
            return false;
        }

        file << std::setw(4) << scene_data << std::endl;
        return true;
    }

    json SceneManager::serialize_node(Object3D *node) {
        if (!node) return json(); // Return empty JSON

        // Get the object's JSON representation using its to_json method
        json node_data;

        if (auto *cube = dynamic_cast<Cube *>(node)) {
            node_data = cube->to_json();
        } else if (auto *plane = dynamic_cast<Plane *>(node)) {
            node_data = plane->to_json();
        } else if (auto *imported_model = dynamic_cast<ImportedModel3D *>(node)) {
            node_data = imported_model->to_json();
        } else if (auto *camera = dynamic_cast<PerspectiveCamera *>(node)) {
            node_data = camera->to_json();
        } else if (auto *dir_light = dynamic_cast<DirectionalLight *>(node)) {
            node_data = dir_light->to_json();
        } else if (auto *point_light = dynamic_cast<PointLight *>(node)) {
            node_data = point_light->to_json();
        } else {
            // Generic Object3D
            node_data = node->to_json();
        }

        // Add material if the object is a shape
        if (auto *shape = dynamic_cast<Shape3D *>(node)) {
            if (Material *material = shape->get_material()) {
                node_data["material"] = serialize_material(material);
            }
        }

        // Recursively serialize children
        json children = json::array();
        for (auto *child: node->get_children()) {
            children.push_back(serialize_node(child));
        }
        if (!children.empty()) {
            node_data["children"] = children;
        }

        return node_data;
    }

    json SceneManager::serialize_material(Material *material) {
        json mat_data;

        mat_data["name"] = material->get_name();

        if (auto *lambert = dynamic_cast<LambertMaterial *>(material)) {
            mat_data["type"] = "lambert";
            mat_data["ambient"] = {
                lambert->get_ambient_color().r, lambert->get_ambient_color().g, lambert->get_ambient_color().b
            };
            mat_data["diffuse"] = {
                lambert->get_diffuse_color().r, lambert->get_diffuse_color().g, lambert->get_diffuse_color().b
            };
        } else if (auto *phong = dynamic_cast<PhongMaterial *>(material)) {
            mat_data["type"] = "phong";
            mat_data["ambient"] = {
                phong->get_ambient_color().r, phong->get_ambient_color().b, phong->get_ambient_color().b
            };
            mat_data["diffuse"] = {
                phong->get_diffuse_color().r, phong->get_diffuse_color().g, phong->get_diffuse_color().b
            };
            mat_data["specular"] = {
                phong->get_specular_color().r, phong->get_specular_color().g, phong->get_specular_color().b
            };
            mat_data["shininess"] = phong->get_shininess();
        }

        json textures = json::array();
        for (const auto &texture: material->get_textures()) {
            json tex_data;
            tex_data["path"] = texture->get_path();
            tex_data["type"] = texture->get_type();
            textures.push_back(tex_data);
        }

        if (!textures.empty()) {
            mat_data["textures"] = textures;
        }

        return mat_data;
    }

    void SceneManager::update(float delta_time) {
        for (auto *obj: objects_) {
            obj->update(delta_time);
        }

        root_node_->update_world_matrix();
    }

    void SceneManager::clear() {
        Object3D *root = root_node_;

        std::clog << "Clearing " << root->get_name() << " Node" << '\n';

        // Clear all children of the root node
        while (!root->get_children().empty()) {
            Object3D *child = root->get_children()[0];
            destroy_object(child);
        }

        objects_.clear();

        active_scene_ = nullptr;

        // Re-register the root node
        objects_.push_back(root);
    }

    Object3D *SceneManager::create_object(const std::string &name) {
        Object3D *obj = new Object3D();
        obj->set_name(name);
        root_node_->add(obj);
        register_object(obj);
        return obj;
    }

    Scene *SceneManager::create_scene(const std::string &name) {
        Scene *scene = new Scene();
        scene->set_name(name);
        root_node_->add(scene);
        register_object(scene);
        return scene;
    }


    Object3D *SceneManager::find_object_by_name(const std::string &name) {
        for (auto *obj: objects_) {
            if (obj->get_name() == name) return obj;

            Object3D *found = find_object_by_name_recursive(obj, name);
            if (found) return found;
        }
        return nullptr;
    }

    Object3D *SceneManager::find_object_by_name_recursive(Object3D *parent, const std::string &name) {
        for (auto *child: parent->get_children()) {
            if (child->get_name() == name) return child;

            Object3D *found = find_object_by_name_recursive(child, name);
            if (found) return found;
        }
        return nullptr;
    }


    void SceneManager::destroy_object(Object3D *object) {
        if (!object) return;

        // If the object has a parent, remove that object from its parent
        if (object->get_parent()) {
            object->get_parent()->remove(object);
        }

        unregister_object(object);

        if (active_scene_->get_active_camera() && objects_contains_camera_component(
                object, active_scene_->get_active_camera())) {
            active_scene_->remove(object);
        }

        delete object;
    }


    void SceneManager::set_active_scene(Scene *scene) {
        if (std::find(objects_.begin(), objects_.end(), scene) != objects_.end()) {
            active_scene_ = scene;
            scene->set_active(true);
#if 0
            std::cout << "Debugging the active scene: " << active_scene_->get_name() << '\n';

            for (auto *obj: active_scene_->get_children()) {
                std::cout << obj->get_name() << " is registered" << '\n';
            }

#endif


            if (scene->get_active_camera()) {
                set_active_camera(scene->get_active_camera());
            }
        }
    }

    void SceneManager::register_object(Object3D *object) {
        // Add to tracked objects
        objects_.push_back(object);

        // also register children
        for (auto *child: object->get_children()) {
            register_object(child);
        }
    }

    void SceneManager::unregister_object(Object3D *object) {
        for (auto *child: object->get_children()) {
            unregister_object(child);
        }

        auto iterator = std::find(objects_.begin(), objects_.end(), object);
        if (iterator != objects_.end()) {
            objects_.erase(iterator);
        }
    }

    bool SceneManager::objects_contains_camera_component(Object3D *object, Camera *target_camera) {
        if (dynamic_cast<Camera *>(object) == target_camera) {
            return true;
        }

        // Check children recursively whether the object contains a camera
        for (auto *child: object->get_children()) {
            if (objects_contains_camera_component(child, target_camera)) {
                return true;
            }
        }

        return false;
    }
}
