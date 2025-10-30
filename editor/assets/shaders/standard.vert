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
out vec3 vColor;
out vec2 vTexCoords;
out vec3 vNormal;
out vec3 vFragPos;
out mat3 vTBN;

void main()
{
    vColor = aColor;
    vTexCoords = aTexCoords;

    mat3 normalMatrix = transpose(inverse(mat3(model)));
    vec3 N = normalize(normalMatrix * aNormal);
    vec3 T = normalize(normalMatrix * aTangent);
    vec3 B = normalize(cross(N, T));
    
    // (Tangent, Bitangent, Normal)
    vTBN = mat3(T, B, N);
    
    vNormal = N; 

    // Transform vertex position to world space for lighting calculations
    vFragPos = vec3(model * vec4(aPosition, 1.0));

    gl_Position = MVP * vec4(aPosition, 1.0);
}
