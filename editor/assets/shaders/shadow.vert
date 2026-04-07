#version 430 core
layout (location = 0) in vec3 aPos;

uniform mat4 uLightViewProjMatrix;
uniform mat4 uModelMatrix;

void main() {
    gl_Position = uLightViewProjMatrix * uModelMatrix * vec4(aPos, 1.0);
}