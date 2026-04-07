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
    
    if (useLuminanceAsAlpha) {
        // Calculate perceived luminance using Rec. 709 coefficients:
        // Red: 0.299 - Human eye is moderately sensitive to red
        // Green: 0.587 - Human eye is most sensitive to green (largest weight)
        // Blue: 0.114 - Human eye is least sensitive to blue
        float luminance = dot(diffuseValue.rgb, vec3(0.299, 0.587, 0.114));
        diffuseValue.a = luminance;
    }
    
    vec4 baseColor = applyVertexColors(diffuseValue, vColor);
    
    // Calculate surface normal
    vec3 normal = calculateSurfaceNormal(vTexCoords, vNormal, vTBN);

    // Calculate lighting
    vec3 result = calculateLambertLighting(normal, baseColor.rgb, vFragPos);

    // Apply transparency and gamma correction
    float finalAlpha = calculateFinalAlpha(baseColor.a);
    result = applyGammaCorrection(result);

    fragColor = vec4(normal, finalAlpha);
}