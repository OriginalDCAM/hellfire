#version 430 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;
uniform vec3 tint = vec3(1.0);
uniform float exposure = 1.0;

void main() {
    vec3 flipped_coords = -TexCoords;
    
    vec3 color = texture(skybox, flipped_coords).rgb;
    // Apply tint and exposure
    color *= tint * exposure;
    
    FragColor = vec4(color, 1.0);
}