#version 430 core

// Uniform inputs
uniform mat4 view;       // View matrix
uniform mat4 projection; // Projection matrix
uniform float time;      // Time for animations

// Per-vertex inputs
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 color;
layout(location = 3) in vec2 texCoords;

// Per-instance inputs
layout(location = 4) in mat4 instanceTransform; // Instance transform matrix (uses locations 4,5,6,7)
layout(location = 8) in vec3 instanceColor;     // Instance color variation
layout(location = 9) in float instanceScale;   // Instance scale

// Outputs to fragment shader
out vec3 vColor;
out vec2 vTexCoords;
out vec3 vNormal;
out vec3 vFragPos;
out vec3 vInstanceColor;

void main()
{
    // Use instance transform as model matrix
    mat4 model = instanceTransform;

    // Calculate MVP matrix
    mat4 MVP = projection * view * model;

    // Transform vertex to clip space
    gl_Position = MVP * vec4(position, 1.0);

    // Pass through texture coordinates
    vTexCoords = texCoords;

    // Calculate world space position for lighting
    vFragPos = vec3(model * vec4(position, 1.0));

    // Transform normal to world space
    mat3 normalMatrix = transpose(inverse(mat3(model)));
    vNormal = normalize(normalMatrix * normal);

    // Combine vertex color with instance color for variation
    vColor = color;
    vInstanceColor = instanceColor;
}