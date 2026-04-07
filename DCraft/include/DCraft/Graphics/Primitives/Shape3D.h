#pragma once
#include "DCraft/Graphics/Materials/Material.h"
#include "DCraft/Structs/Object3D.h"

namespace DCraft
{

    class Shape3D : public Object3D
    {
    public:
        Shape3D(const std::string& name) : Object3D(name) {}
        // Texture Management
        void set_texture(const std::string& path, TextureType type);
        void set_texture(Texture* texture, bool take_ownsership = false);
        Texture* get_texture() const;
        bool has_texture() const;

        // Material properties
        void set_material(Material* material);
        Material* get_material();

        // Geometry Access & modification
        const std::vector<float>& get_vertices() const;
        const std::vector<float>& get_colors() const;
        const std::vector<float>& get_indices() const;

        // Utility methods
        void set_wireframe_mode(bool enable);
        bool is_wireframe_mode() const;

        // Collision detection helpers
        bool intersects(const Shape3D& other) const;
        float get_bounding_radius();

        // Rebuild the Geometry
        void rebuild();

        void draw_self(const glm::mat4 &view, const glm::mat4 &projection, uint32_t shader_program, void* renderer_context) override;
    
    protected:
        virtual void set_vertices();
        virtual void set_colors();
        virtual void set_uvs();

        bool owns_texture_ = false;

        std::vector<float >vertices_;
        std::vector<float> colors_;
        std::vector<unsigned int> indices_;
        Texture* texture_ = nullptr;
        bool wireframe_mode_ = false;
    };
}

