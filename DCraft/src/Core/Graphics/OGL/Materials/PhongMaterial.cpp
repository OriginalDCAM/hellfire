//
// Created by denzel on 07/04/2025.
//

#include "DCraft/Graphics/Materials/PhongMaterial.h"

#include <GL/glew.h>
#include <glm/gtc/type_ptr.inl>

namespace DCraft {
    void PhongMaterial::bind(void *renderer_context) {
        StandardMaterial::bind(renderer_context);
        // Set Phong-specific uniforms

        uint32_t shader_program = get_shader();
        
        glUniform3fv(glGetUniformLocation(shader_program, "ambientColor"), 1, glm::value_ptr(ambient_color_));
        glUniform3fv(glGetUniformLocation(shader_program, "diffuseColor"), 1, glm::value_ptr(diffuse_color_));
        glUniform3fv(glGetUniformLocation(shader_program, "specularColor"), 1, glm::value_ptr(specular_color_));
        glUniform1f(glGetUniformLocation(shader_program, "shininess"), shininess_);
        glUniform1i(glGetUniformLocation(shader_program, "materialType"), 1); // 1 = Phong

        if (has_diffuse_texture_) {
            Texture* diffuseTexture = get_diffuse_texture();
            if (diffuseTexture) {
                diffuseTexture->bind(0);
                glUniform1i(glGetUniformLocation(shader_program, "diffuse1"), 0);
            }
        }
        
    }
}
