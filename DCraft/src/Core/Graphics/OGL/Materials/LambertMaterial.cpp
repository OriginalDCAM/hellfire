//
// Created by denzel on 07/04/2025.
//

#include "DCraft/Graphics/Materials/LambertMaterial.h"

#include <GL/glew.h>
#include <glm/gtc/type_ptr.inl>


namespace DCraft {
    void LambertMaterial::bind(void *renderer_context) {
        // Call parent first to setup shader and textures
        StandardMaterial::bind(renderer_context);

        uint32_t shader_program = get_shader();

        glUniform3fv(glGetUniformLocation(shader_program, "ambientColor"), 1, glm::value_ptr(ambient_color_));
        glUniform3fv(glGetUniformLocation(shader_program, "diffuseColor"), 1, glm::value_ptr(diffuse_color_));
        glUniform1i(glGetUniformLocation(shader_program, "materialType"), 0); // 0 Lambert
        glUniform1i(glGetUniformLocation(shader_program, "useDiffuse1"), has_diffuse_texture_);
    
        if (has_diffuse_texture_) {
            Texture* diffuseTexture = get_diffuse_texture();
            if (diffuseTexture) {
                diffuseTexture->bind(0);
                glUniform1i(glGetUniformLocation(shader_program, "diffuse1"), 0);
            }
        }

    }
}
