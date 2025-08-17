#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 color;
layout(location = 3) in vec2 texCoords;

uniform mat4 MVP;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normalMatrix;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;
out vec3 Color;

void main() {
    // World position
    FragPos = vec3(model * vec4(position, 1.0));

    // Transform normal
    Normal = normalMatrix * normal;

    // Pass through texture coordinates and color
    TexCoord = texCoords;
    Color = color;

    // Final position
    gl_Position = MVP * vec4(position, 1.0);
}