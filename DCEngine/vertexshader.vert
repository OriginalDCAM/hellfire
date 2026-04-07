#version 430 core

// Uniform inputs
uniform mat4 mvp;

// Per-vertex inputs
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 color;
layout(location = 3) in vec2 texCoords;

// Outputs
out vec3 vColor;

void main()
{
    gl_Position = mvp * vec4(position, 1.0);
    vColor = color;
}
