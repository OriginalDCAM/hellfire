#version 430 core

// Inputs from vertex shader
in vec3 vColor;
in vec2 vTexCoords;
in vec3 vNormal;
in vec3 vFragPos;
in mat3 vTBN;

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

// Material uniforms
uniform sampler2D uDiffuseTexture;
uniform sampler2D uNormalTexture;
uniform sampler2D uSpecularTexture;
uniform sampler2D uRoughnessTexture;
uniform sampler2D uMetallicTexture;
uniform sampler2D uAOTexture;
uniform sampler2D uEmissiveTexture;

// Texture usage flags
uniform bool useUDiffuseTexture; 
uniform bool useUNormalTexture;
uniform bool useUSpecularTexture;
uniform bool useURoughnessTexture;
uniform bool useUMetallicTexture;
uniform bool useUAOTexture;
uniform bool useUEmissiveTexture;

// Material properties
uniform vec3 ambientColor;
uniform vec3 diffuseColor;

// Uv controls
uniform vec2 uvTiling = vec2(1.0);
uniform vec2 uvOffset = vec2(0.0);
uniform float uvRotation = 0.0;

// Texture filtering mode
uniform int textureWrapMode = 0; // 0=repeat, 1=clamp, 2=mirror

// Transparency uniforms
uniform float alpha = 1.0;
uniform float transparency = 1.0;
uniform bool useTransparency = false;

vec2 transformUV(vec2 uv) {
    // Apply offset first
    uv += uvOffset;

    // Apply tiling
    uv *= uvTiling;

    // Apply rotation if needed
    if (uvRotation != 0.0) {
        float cos_r = cos(uvRotation);
        float sin_r = sin(uvRotation);
        mat2 rotation = mat2(cos_r, -sin_r, sin_r, cos_r);
        uv = rotation * (uv - 0.5) + 0.5;
    }

    return uv;
}

// Texture sampling with wrapping modes
vec4 sampleTexture(sampler2D tex, vec2 uv) {
    vec2 transformedUV = transformUV(uv);

    // Apply different wrapping modes if needed
    if (textureWrapMode == 1) {
        // Clamp to edge
        transformedUV = clamp(transformedUV, 0.0, 1.0);
    } else if (textureWrapMode == 2) {
        // Mirror repeat
        transformedUV = abs(mod(transformedUV, 2.0) - 1.0);
    }

    return texture(tex, transformedUV);
}

// Calculate lighting contribution from a directional light 
vec3 calcDirectionalLight(DirectionalLight light, vec3 normal, vec3 diffuse) {
    vec3 lightDir = normalize(-light.direction);

    // Lambert diffuse shading only
    float diff = max(dot(normal, lightDir), 0.0);

    // Combine results (no specular for Lambert)
    vec3 diffuseResult = light.color * light.intensity * diff * diffuse;

    return diffuseResult;
}

// Calculate lighting contribution from a point light 
vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 diffuse) {
    vec3 lightDir = normalize(light.position - fragPos);

    // Lambert diffuse shading only
    float diff = max(dot(normal, lightDir), 0.0);

    // Attenuation
    float distance = length(light.position - fragPos);
    float attenuation = clamp(1.0 - (distance * distance) / (light.range * light.range), 0.0, 1.0);
    attenuation *= attenuation;  // Smooth falloff

    // Combine results (no specular for Lambert)
    vec3 diffuseResult = light.color * light.intensity * diff * diffuse;

    // Apply attenuation
    diffuseResult *= attenuation;

    return diffuseResult;
}

void main() {
    vec4 diffuseValue = sampleTexture(uDiffuseTexture, vTexCoords);


    // Apply vertex colors if present
    vec4 baseColor;
    if (length(vColor) > 0.001) {
        baseColor = diffuseValue * vec4(vColor, 1.0);
    } else {
        baseColor = diffuseValue;
    }

    vec3 normal;
    // Normalize the normal
    if (useUNormalTexture) {
        vec3 normalMap = texture(uNormalTexture, vTexCoords).rgb;
        normalMap = normalize(normalMap * 2.0 - 1.0); // Convert from [0, 1] to [-1, 1]
        
        normal = normalize(vTBN * normalMap);
        
    } else {
        normal = normalize(vNormal);
    }

    // Calculate lighting - Lambert model (ambient + diffuse only)
    vec3 result = ambientColor * baseColor.rgb; // Ambient light

    // Add all directional lights
    for (int i = 0; i < min(numDirectionalLights, MAX_DIRECTIONAL_LIGHTS); i++) {
        result += calcDirectionalLight(directionalLights[i], normal, baseColor.rgb);
    }

    // Add all point lights
    for (int i = 0; i < min(numDirectionalLights, MAX_DIRECTIONAL_LIGHTS); i++) {
        result += calcPointLight(pointLights[i], normal, vFragPos, baseColor.rgb);
    }

    // Calculate final alpha
    float finalAlpha = baseColor.a;
    if (useTransparency) {
        finalAlpha *= alpha * transparency;
    }

    // Gamma correction
    result = pow(result, vec3(1.0/2.2));

    fragColor = vec4(result, finalAlpha);
}