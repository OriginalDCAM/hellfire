#version 430 core

#include "common/vertex_inputs.glsl"
#include "common/material_uniforms.glsl"
#include "common/light_uniforms.glsl"
#include "common/texture_utils.glsl"
#include "lighting/blinn_phong.glsl"

layout(location=0) out vec4 fragColor;
layout(location=1) out uint objectID;

uniform uint uObjectID;

float calculate_shadow(int light_index, vec3 frag_pos) {
    // Transform fragmenet position to light space
    vec4 frag_pos_light_space = uLightSpaceMatrix[light_index] * vec4(frag_pos, 1.0);
    
    // Perspective divide
    vec3 proj_coords = frag_pos_light_space.xyz / frag_pos_light_space.w;
    
    // Transform to [0,1] range for depth map sampling
    proj_coords = proj_coords * 0.5 + 0.5;
    
    if (proj_coords.z > 1.0) return 0.0;
    
    float closest_depth = texture(uShadowMap[light_index], proj_coords.xy).r;
    
    // Get depth of current fragment from light's perspective
    float current_depth = proj_coords.z;
    
    // Bias to prevent shadow acne (object's casting shadows onto itself)
        float bias = 0.001;
    float shadow = current_depth - bias > closest_depth ? 1.0 : 0.0;
    
    return shadow;
}

void main() {
    vec4 diffuseValue = sampleDiffuseTexture(fs_in.TexCoords);
    vec4 baseColor = applyVertexColors(diffuseValue, fs_in.Color);
    vec3 normal = calculateSurfaceNormal(fs_in.TexCoords, fs_in.Normal, fs_in.TBN);
    vec3 result = calculateBlinnPhongLighting(normal, baseColor.rgb, fs_in.FragPos);

    float shadow_factor = 0.0;
    if (numDirectionalLights > 0) {
        shadow_factor = calculate_shadow(0, fs_in.FragPos);
    }

    float ambient_strength = 0.2;
    result *= (ambient_strength + (1.0 - ambient_strength) * (1.0 - shadow_factor));

    fragColor = vec4(result, uOpacity);
    objectID = uObjectID;
}