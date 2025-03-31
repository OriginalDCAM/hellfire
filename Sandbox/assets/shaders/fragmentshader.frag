#version 430 core

// Inputs from vertexshader
in vec3 vColor;

// Output color
out vec4 fragColor;

void main()
{
    fragColor = vec4(vColor, 1.0);
}
