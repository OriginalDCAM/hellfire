#version 430 core

// Inputs from vertexshader
in vec3 vColor;
in vec2 vTexCoords;

// Output color
out vec4 fragColor;

// Texture samplers
uniform sampler2D diffuse1;
uniform sampler2D specular1;
uniform bool useTexture;

// Material properties
uniform vec3 ambientColor;
uniform float shininess;

void main()
{
    vec4 result;

    if (useTexture) {
        // Get texture value
        vec4 texColor = texture(diffuse1, vTexCoords);

        // Combine with vertex color
//        result = texColor * vec4(vColor, 1.0);
        result = texColor;
    } else {
        // Just use the vertex color
        result = vec4(vColor, 1.0);
    }
    fragColor = result;
}
