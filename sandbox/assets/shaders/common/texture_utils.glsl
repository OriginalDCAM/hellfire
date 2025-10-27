vec2 transformUV(vec2 uv) {
    uv += uvOffset;
    uv *= uvTiling;

    if (uvRotation != 0.0) {
        float cos_r = cos(uvRotation);
        float sin_r = sin(uvRotation);
        mat2 rotation = mat2(cos_r, -sin_r, sin_r, cos_r);
        uv = rotation * (uv - 0.5) + 0.5;
    }

    return uv;
}

vec4 sampleTexture(sampler2D tex, vec2 uv) {
    vec2 transformedUV = transformUV(uv);

    if (textureWrapMode == 1) {
        transformedUV = clamp(transformedUV, 0.0, 1.0);
    } else if (textureWrapMode == 2) {
        transformedUV = abs(mod(transformedUV, 2.0) - 1.0);
    }

    return texture(tex, transformedUV);
}

vec4 sampleDiffuseTexture(vec2 texCoords) {
    return useUDiffuseTexture ? sampleTexture(uDiffuseTexture, texCoords) : vec4(uDiffuseColor, 1.0);
}

vec4 applyVertexColors(vec4 diffuseValue, vec3 vertexColor) {
    return length(vertexColor) > 0.001 ? diffuseValue * vec4(vertexColor, 1.0) : diffuseValue;
}

vec3 calculateSurfaceNormal(vec2 texCoords, vec3 vertexNormal, mat3 tbn) {
    if (useUNormalTexture) {
        vec3 normalMap = texture(uNormalTexture, texCoords).rgb;
        normalMap = normalize(normalMap * 2.0 - 1.0);
        return normalize(tbn * normalMap);
    }
    return normalize(vertexNormal);
}

float calculateFinalAlpha(float baseAlpha) {
    return useTransparency ? baseAlpha * uAlpha * uTransparency : baseAlpha;
}

vec3 applyGammaCorrection(vec3 color) {
    return pow(color, vec3(1.0/2.2));
}

