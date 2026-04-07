#version 430 core

in vec3 vColor;
in vec2 vTexCoords;
in vec3 vNormal;
in vec3 vFragPos;

out vec4 FragColor;


uniform sampler2D diffuseTexture;

void main() {
    FragColor = texture(diffuseTexture, vTexCoords);
}