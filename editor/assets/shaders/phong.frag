#version 430 core

#include "common/vertex_inputs.glsl"
#include "common/material_uniforms.glsl"
#include "common/light_uniforms.glsl"
#include "common/texture_utils.glsl"
#include "lighting/blinn_phong.glsl"

layout(location=0) out vec4 fragColor;
layout(location=1) out uint objectID;

uniform uint uObjectID;

float calculate_shadow(int light_index, vec3 frag_pos, vec3 normal, vec3 light_dir) {
    vec4 frag_pos_light_space = uLightSpaceMatrix[light_index] * vec4(frag_pos, 1.0);
    vec3 proj_coords = frag_pos_light_space.xyz / frag_pos_light_space.w;
    proj_coords = proj_coords * 0.5 + 0.5;

    if (proj_coords.z > 1.0) return 0.0;

    float current_depth = proj_coords.z;
    float bias = 0.005;

    // PCF: sample a 3x3 area
    float shadow = 0.0;
    vec2 texel_size = 1.0 / textureSize(uShadowMap[light_index], 0);

    for (int x = -1; x <= 1; ++x) {
        for (int y = -1; y <= 1; ++y) {
            float pcf_depth = texture(uShadowMap[light_index], proj_coords.xy + vec2(x, y) * texel_size).r;
            shadow += current_depth - bias > pcf_depth ? 1.0 : 0.0;
        }
    }

    return shadow / 9.0;
}

void main() {
    vec4 diffuseValue = sampleDiffuseTexture(fs_in.TexCoords);
    vec4 baseColor = applyVertexColors(diffuseValue, fs_in.Color);
    vec3 normal = calculateSurfaceNormal(fs_in.TexCoords, fs_in.Normal, fs_in.TBN);
    vec3 ambient = uAmbientLight * baseColor.rgb;
    
    // Calculate direct lighting (diffuse + specular)
    vec3 direct = calculateBlinnPhongLighting(normal, baseColor.rgb, fs_in.FragPos);

    float shadow_factor = 0.0;
    if (numDirectionalLights > 0) {
        shadow_factor = calculate_shadow(0, fs_in.FragPos, normal, -directionalLights[0].direction);
    }

    vec3 result = ambient + direct * (1.0 - shadow_factor);

    fragColor = vec4(result, uOpacity);
    objectID = uObjectID;
}