    #version 430 core

// Uniform inputs
uniform mat4 MVP;       // Combined model-view-projection matrix
uniform mat4 model;     // Model matrix only - needed for world space calculations

// Per-vertex inputs
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec3 aColor;
layout(location = 3) in vec2 aTexCoords;
layout(location = 4) in vec3 aTangent;
layout(location = 5) in vec3 aBitangent;

// Outputs
    out VS_OUT {
        vec3 Color;
        vec2 TexCoords;
        vec3 Normal;
        vec3 FragPos;
        mat3 TBN;
    } vs_out;

void main()
{
    vs_out.Color = aColor;
    vs_out.TexCoords = aTexCoords;

    mat3 normalMatrix = transpose(inverse(mat3(model)));
    vec3 N = normalize(normalMatrix * aNormal);
    vec3 T = normalize(normalMatrix * aTangent);
    vec3 B = normalize(cross(N, T));
    
    // (Tangent, Bitangent, Normal)
    vs_out.TBN = mat3(T, B, N);
    
    vs_out.Normal = N; 

    // Transform vertex position to world space for lighting calculations
    vs_out.FragPos = vec3(model * vec4(aPosition, 1.0));

    gl_Position = MVP * vec4(aPosition, 1.0);
}
