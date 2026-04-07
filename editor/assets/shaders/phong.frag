#version 430 core

#include "common/vertex_inputs.glsl"
#include "common/material_uniforms.glsl"
#include "common/light_uniforms.glsl"
#include "common/texture_utils.glsl"
#include "lighting/blinn_phong.glsl"

layout(location=0) out vec4 fragColor;
layout(location=1) out uint objectID;

uniform uint uObjectID;

void main() {
    // Sample base textures
    vec4 diffuseValue = sampleDiffuseTexture(fs_in.TexCoords);
    vec4 baseColor = applyVertexColors(diffuseValue, fs_in.Color);

    // Calculate surface normal
    vec3 normal = calculateSurfaceNormal(fs_in.TexCoords, fs_in.Normal, fs_in.TBN);
    // Calculate lighting
    vec3 result = calculateBlinnPhongLighting(normal, baseColor.rgb, fs_in.FragPos);

    fragColor = vec4(result, uOpacity);
    objectID = uObjectID;
}