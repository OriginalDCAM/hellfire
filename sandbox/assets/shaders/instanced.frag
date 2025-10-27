#version 430 core

// Inputs from vertex shader
in vec3 vColor;
in vec3 vInstanceColor;
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

// Material uniforms
uniform sampler2D uDiffuseTexture;

// Texture usage flags
uniform bool useUDiffuseTexture;

// Material properties
uniform vec3 uAmbientColor;
uniform vec3 uDiffuseColor;

// Uv controls
uniform vec2 uvTiling = vec2(1.0);
uniform vec2 uvOffset = vec2(0.0);
uniform float uvRotation = 0.0;

// Texture filtering mode
uniform int textureWrapMode = 0; // 0=repeat, 1=clamp, 2=mirror

// Transparency uniforms
uniform float uAlpha = 1.0;
uniform float uTransparency = 1.0;
uniform bool useTransparency = false;

uniform vec3 baseColor = vec3(1.0);
uniform float roughness = 0.5;
uniform float metallic = 0.0;
uniform vec3 emissive = vec3(0.0);
uniform float specularStrength = 0.5;

vec3 calcDirectionalLightWithSpecular(DirectionalLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 diffuse) {
    vec3 lightDir = normalize(-light.direction);

    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuseResult = light.color * light.intensity * diff * diffuse;

    // Simple specular for metallic materials
    if (metallic > 0.1) {
        vec3 reflectDir = reflect(-lightDir, normal);
        float shininess = mix(32.0, 128.0, 1.0 - roughness); // Roughness affects shininess
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
        vec3 specularResult = light.color * light.intensity * spec * specularStrength * metallic;
        diffuseResult += specularResult;
    }

    return diffuseResult;
}

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
        uv = rotation * (uv - 0.5) + 0.5; // Rotate around center
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
    // textureWrapMode == 0 uses default repeat behavior

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

    // Combine results 
    vec3 diffuseResult = light.color * light.intensity * diff * diffuse;

    // Apply attenuation
    diffuseResult *= attenuation;

    return diffuseResult;
}

float near = 0.1;
float far = 100.0;

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));
}

void main() {
    vec4 diffuseValue;
    if (useUDiffuseTexture) {
        diffuseValue = sampleTexture(uDiffuseTexture, vTexCoords);
    } else {
        diffuseValue = vec4(uDiffuseColor, 1.0);
    }

    vec4 materialColor = diffuseValue * vec4(baseColor, 1.0);

    // Apply instance colors for variation
    vec4 finalBaseColor;
    if (vInstanceColor.r != 0.0 || vInstanceColor.g != 0.0 || vInstanceColor.b != 0.0) {
        finalBaseColor = materialColor * vec4(vInstanceColor, 1.0);
    } else {
        finalBaseColor = materialColor;
    }

    // Add emissive component
    vec3 emissiveContribution = emissive;

    vec3 normal = normalize(vNormal);
    vec3 viewDir = normalize(-vFragPos); 

    // Calculate lighting
    vec3 result = uAmbientColor * finalBaseColor.rgb + emissiveContribution;

    for (int i = 0; i < numDirectionalLights; i++) {
        result += calcDirectionalLightWithSpecular(directionalLights[i], normal, vFragPos, viewDir, finalBaseColor.rgb);
    }

    for (int i = 0; i < numPointLights; i++) {
        result += calcPointLight(pointLights[i], normal, vFragPos, finalBaseColor.rgb);
    }

    // Calculate final alpha
    float finalAlpha = finalBaseColor.a;
    if (useTransparency) {
        finalAlpha *= uAlpha * uTransparency;
    }

    fragColor = vec4(result, finalAlpha);
}