#define MAX_DIRECTIONAL_LIGHTS 4
#define MAX_POINT_LIGHTS 8

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

uniform int numDirectionalLights;
uniform int numPointLights;
uniform DirectionalLight directionalLights[MAX_DIRECTIONAL_LIGHTS];
uniform PointLight pointLights[MAX_POINT_LIGHTS];

