#version 430 core

layout (location = 0) in vec3 aPos;

// Uniform inputs
uniform mat4 MVP;       // Combined model-view-projection matrix
uniform mat4 model;     // Model matrix only - needed for world space calculations

// Per-vertex inputs
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 color;
layout(location = 3) in vec2 texCoords;

// Outputs
out vec3 vColor;
out vec2 vTexCoords;
out vec3 vNormal;
out vec3 vFragPos;

void main() {
    // Calculate clip space position
    gl_Position = MVP * vec4(position, 1.0);

    // Pass color and texture coordinates directly
    vColor = color;
    vTexCoords = texCoords;

    mat3 normalMatrix = transpose(inverse(mat3(model)));
    vNormal = normalize(normalMatrix * normal);

    // Transform vertex position to world space for lighting calculations
    vFragPos = vec3(model * vec4(position, 1.0));

}