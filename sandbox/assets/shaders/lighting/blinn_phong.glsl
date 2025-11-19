vec3 calcDirectionalLight(DirectionalLight light, vec3 normal, vec3 fragPos, vec3 diffuse, vec3 specular) {
    vec3 lightDir = normalize(-light.direction);
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);

    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // Specular shading (Blinn-Phong)
    float spec = 0.0f;
    if (diff > 0.0f) {
       float shininess = max(uShininess, 8.0);
        spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);
    }

    // Combine results
    vec3 diffuseResult = light.color * light.intensity * diff * diffuse;
    vec3 specularResult = light.color * light.intensity * spec * specular;

    return diffuseResult + specularResult;
}

vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 diffuse, vec3 specular) {
    float distance = length(light.position - fragPos);

    // Early out if outside light range
    if (distance > light.range) return vec3(0.0);
    
    vec3 lightDir = normalize(light.position - fragPos);
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);

    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // Specular shading (Blinn-Phong)
    float spec = 0.0f;
    if (diff > 0.0f) {
        float shininess = max(uShininess, 8.0);
        spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);
    }

    // Attenuation
    float attenuation = clamp(1.0 - (distance * distance) / (light.range * light.range), 0.0, 1.0);
    attenuation *= attenuation;

    // Combine results
    vec3 diffuseResult = light.color * light.intensity * diff * diffuse;
    vec3 specularResult = light.color * light.intensity * spec * specular;

    // Apply attenuation
    return (diffuseResult + specularResult) * attenuation;
}

vec3 calculateBlinnPhongLighting(vec3 normal, vec3 baseColor, vec3 fragPos) {
    vec3 result = uAmbientLight * baseColor;

    vec3 materialDiffuse = uDiffuseColor * baseColor;
    // Add directional lights
    for (int i = 0; i < min(numDirectionalLights, MAX_DIRECTIONAL_LIGHTS); i++) {
        result += calcDirectionalLight(directionalLights[i], normal, fragPos, materialDiffuse, uSpecularColor);
    }

    // Add point lights
    for (int i = 0; i < min(numPointLights, MAX_POINT_LIGHTS); i++) {
        result += calcPointLight(pointLights[i], normal, fragPos, materialDiffuse, uSpecularColor);
    }

    return result;
}