#version 430 core

// Inputs from vertex shader
in vec3 vColor;
in vec2 vTexCoords;
in vec3 vNormal;
in vec3 vFragPos;

// Output
out vec4 fragColor;

// Maximum number of lights
#define MAX_DIRECTIONAL_LIGHTS 4
#define MAX_POINT_LIGHTS 8

// Light structures
struct DirectionalLight {
    vec3 direction;
    vec3 color;
    float intensity;
};

struct PointLight {
    vec3 position;
    vec3 color;
    float intensity;
    float range;
    float attenuation;
};

// Light uniforms
uniform int numDirectionalLights;
uniform int numPointLights;
uniform DirectionalLight directionalLights[MAX_DIRECTIONAL_LIGHTS];
uniform PointLight pointLights[MAX_POINT_LIGHTS];

// Camera position for specular
uniform vec3 viewPos;

// Material uniforms
uniform sampler2D diffuse1;
uniform sampler2D diffuse2;
uniform sampler2D specular1;
uniform sampler2D normal1;
uniform sampler2D roughness1;
uniform sampler2D metalness1;
uniform sampler2D ao1;

// Texture usage flags
uniform bool useDiffuse1;
uniform bool useDiffuse2;
uniform bool useSpecular1;
uniform bool useNormalMap;
uniform float diffuseBlend;

// Material properties
uniform vec3 ambientColor;
uniform vec3 diffuseColor;
uniform vec3 specularColor;
uniform float shininess;
uniform int materialType; // 0 = Lambert, 1 = Phong, 2 = PBR

// Calculate lighting contribution from a directional light
vec3 calcDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir, vec3 diffuse, vec3 specular) {
    vec3 lightDir = normalize(-light.direction);

    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // Specular shading
    float spec = 0.0;
    // Phong and PBR shading
    if (materialType > 0 && materialType < 3) {
        vec3 reflectDir = reflect(-lightDir, normal);
        spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    }

    // Combine results
    vec3 diffuseResult = light.color * light.intensity * diff * diffuse;
    vec3 specularResult = light.color * light.intensity * spec * specular;

    return diffuseResult + specularResult;
}

// Calculate lighting contribution from a point light
vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 diffuse, vec3 specular) {
    vec3 lightDir = normalize(light.position - fragPos);

    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // Specular shading (if Phong or PBR)
    float spec = 0.0;
    if (materialType > 0) {
        vec3 reflectDir = reflect(-lightDir, normal);
        spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    }

    // Attenuation
    float distance = length(light.position - fragPos);
    float attenuation = clamp(1.0 - (distance * distance) / (light.range * light.range), 0.0, 1.0);
    attenuation *= attenuation;  // Smooth falloff

    // Combine results
    vec3 diffuseResult = light.color * light.intensity * diff * diffuse;
    vec3 specularResult = light.color * light.intensity * spec * specular;

    // Apply attenuation
    diffuseResult *= attenuation;
    specularResult *= attenuation;

    return diffuseResult + specularResult;
}

void main() {
    // Get base diffuse color
    vec4 diffuseValue;
    if (useDiffuse1 && useDiffuse2) {
        vec4 diff1 = texture(diffuse1, vTexCoords);
        vec4 diff2 = texture(diffuse2, vTexCoords);
        diffuseValue = mix(diff1, diff2, diffuseBlend);
    } else if (useDiffuse1) {
        diffuseValue = texture(diffuse1, vTexCoords);
    } else {
        diffuseValue = vec4(diffuseColor, 1.0);
    }

    // Apply vertex color
    vec4 baseColor;
    
    if (useDiffuse1) {
        baseColor = diffuseValue;
    } else {
        baseColor = diffuseValue * vec4(vColor, 1.0);
    }

    // Get specular value
    vec3 specularValue;
    if (useSpecular1) {
        specularValue = texture(specular1, vTexCoords).rgb;
    } else {
        specularValue = specularColor;
    }

    // Normalize the normal
    vec3 normal = normalize(vNormal);
    // Get view direction
    vec3 viewDir = normalize(viewPos - vFragPos);

    // Calculate lighting
    vec3 result = ambientColor * baseColor.rgb; // Ambient light

    // Add all directional lights
    for (int i = 0; i < numDirectionalLights; i++) {
        result += calcDirectionalLight(directionalLights[i], normal, viewDir, baseColor.rgb, specularValue);
    }

    // Add all point lights
    for (int i = 0; i < numPointLights; i++) {
        result += calcPointLight(pointLights[i], normal, vFragPos, viewDir, baseColor.rgb, specularValue);
    }
    
    fragColor = vec4(result, baseColor.a);


}