#version 430 core

#include "common/vertex_inputs.glsl"
#include "common/material_uniforms.glsl"
#include "common/light_uniforms.glsl"
#include "common/texture_utils.glsl"
#include "lighting/blinn_phong.glsl"

out vec4 fragColor;

void main() {
    // Sample base textures
    vec4 diffuseValue = sampleDiffuseTexture(vTexCoords);
    vec4 baseColor = applyVertexColors(diffuseValue, vColor);

    // Calculate surface normal
    vec3 normal = normalize(vNormal);

    // Calculate lighting
    vec3 result = calculateBlinnPhongLighting(normal, baseColor.rgb, vFragPos);

    fragColor = vec4(result, uOpacity);
}