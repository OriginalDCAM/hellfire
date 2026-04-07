#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

out vec4 FragColor;

uniform vec3 uSunPosition;
uniform sampler2D uDayTexture;
uniform sampler2D uNightTexture;

void main() {
    // Sun direction and lighting calculation
    vec3 sunDirection = normalize(uSunPosition - FragPos);
    float sunDot = dot(normalize(Normal), sunDirection);

    // Day/night factor
    float dayFactor = smoothstep(-0.1, 0.1, sunDot);

    // Sample textures
    vec4 dayColor = texture(uDayTexture, TexCoord);
        vec4 nightColor = texture(uNightTexture
        , TexCoord);

    // Simple blend
    vec4 result = mix(nightColor, dayColor, dayFactor);

    // Basic lighting
    result.rgb *= max(0.2, sunDot * 0.8 + 0.2);

    FragColor = result;

}