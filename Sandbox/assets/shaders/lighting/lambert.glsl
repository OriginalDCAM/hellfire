vec3 calcDirectionalLight(DirectionalLight light, vec3 normal, vec3 diffuse) {
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(normal, lightDir), 0.0);
    return light.color * light.intensity * diff * diffuse;
}

vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 diffuse) {
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);

    float distance = length(light.position - fragPos);
    float attenuation = clamp(1.0 - (distance * distance) / (light.range * light.range), 0.0, 1.0);
    attenuation *= attenuation;

    return light.color * light.intensity * diff * diffuse * attenuation;
}

vec3 calculateLambertLighting(vec3 normal, vec3 baseColor, vec3 fragPos) {
    vec3 result = uAmbientColor * baseColor;

    // Add directional lights
    for (int i = 0; i < min(numDirectionalLights, MAX_DIRECTIONAL_LIGHTS); i++) {
        result += calcDirectionalLight(directionalLights[i], normal, baseColor);
    }

    // Add point lights
    for (int i = 0; i < min(numPointLights, MAX_POINT_LIGHTS); i++) {
        result += calcPointLight(pointLights[i], normal, fragPos, baseColor);
    }

    return result;
}

