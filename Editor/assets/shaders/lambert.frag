#version 430 core

#include "common/vertex_inputs.glsl"
#include "common/material_uniforms.glsl"
#include "common/light_uniforms.glsl"
#include "common/texture_utils.glsl"
#include "lighting/lambert.glsl"

out vec4 fragColor;

void main() {
    // Sample base textures
    vec4 diffuseValue = sampleDiffuseTexture(vTexCoords);
    vec4 baseColor = applyVertexColors(diffuseValue, vColor);

    // Calculate surface normal
    vec3 normal = calculateSurfaceNormal(vTexCoords, vNormal, vTBN);

    // Calculate lighting
    vec3 result = calculateLambertLighting(normal, baseColor.rgb, vFragPos);

    // Apply transparency and gamma correction
    float finalAlpha = calculateFinalAlpha(baseColor.a);
    result = applyGammaCorrection(result);

    fragColor = vec4(result, finalAlpha);
}