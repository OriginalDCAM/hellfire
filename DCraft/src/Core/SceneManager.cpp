#include "DCraft/Addons/SceneManager.h"

#include <fstream>

#include "DCraft/Addons/ImportedModel3D.h"
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
        return true;
    }

    bool SceneManager::save_scene(const std::string &filepath, Scene* scene = nullptr) {
        if (!scene) scene = get_active_scene();
        
        json scene_data = serialize_node(scene);
        scene_data["name"] = scene->get_name();

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

        json node_data;

        if (auto* cube = dynamic_cast<Cube*>(node)) {
            node_data = cube->to_json();
        } else if (auto* plane = dynamic_cast<Plane*>(node)) {
            node_data = plane->to_json();
        } else if (auto* imported_model = dynamic_cast<ImportedModel3D*>(node)) {
            node_data = imported_model->to_json();
        } else if (auto* camera = dynamic_cast<PerspectiveCamera*>(node)) {
            node_data = camera->to_json();
        } else if (auto* dir_light = dynamic_cast<DirectionalLight*>(node)) {
            node_data = dir_light->to_json();
        } else if (auto* point_light = dynamic_cast<PointLight*>(node)) {
            node_data = point_light->to_json();
        }

        if (auto* shape = dynamic_cast<Shape3D*>(node)) {
            if (Material* material = shape->get_material()) {
                node_data["material"] = serialize_material(material);
            }
        }

        // Recursively serialize children
        json children = json::array();
        for (auto* child : node->get_children()) {
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

        if (auto* lambert = dynamic_cast<LambertMaterial*>(material)) {
            mat_data["type"] = "lambert";
            mat_data["ambient"] = {lambert->get_ambient_color().r, lambert->get_ambient_color().g, lambert->get_ambient_color().b};
            mat_data["diffuse"] = {lambert->get_diffuse_color().r, lambert->get_diffuse_color().g, lambert->get_diffuse_color().b};
        } else if (auto* phong = dynamic_cast<PhongMaterial*>(material)) {
            mat_data["type"] = "phong";
            mat_data["ambient"] = {phong->get_ambient_color().r, phong->get_ambient_color().b, phong->get_ambient_color().b};
            mat_data["diffuse"] = {phong->get_diffuse_color().r, phong->get_diffuse_color().g, phong->get_diffuse_color().b};
            mat_data["specular"] = {phong->get_specular_color().r, phong->get_specular_color().g, phong->get_specular_color().b};
            mat_data["shininess"] = phong->get_shininess();
        }

        json textures = json::array();
        for (const auto& texture : material->get_textures()) {
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
            // TODO: Change this to a debug macro 
#if 1
            std::cout << "Debugging the active scene: " << active_scene_->get_name() << '\n';

            for (auto* obj : active_scene_->get_children()) {
                std::cout << obj->get_name()  << " is registered" << '\n';
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
