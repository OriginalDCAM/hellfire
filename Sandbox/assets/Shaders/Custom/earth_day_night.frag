#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;
in vec3 Color;

out vec4 FragColor;

// Your lighting system uniforms (automatically provided)
uniform int numDirectionalLights;
uniform int numPointLights;
uniform vec3 viewPos;

// Earth-specific uniforms (added by EarthDayNightController)
uniform vec3 uSunPosition;
uniform float uTime;

// Textures
uniform sampler2D uDiffuseTexture;  // Day texture
// Note: You might need to extend your material system for multiple textures
// For now, we'll create the night effect procedurally

void main() {
    // Calculate direction from surface to sun
    vec3 sunDirection = normalize(uSunPosition - FragPos);

    // Calculate how much this fragment faces the sun
    float sunDot = dot(normalize(Normal), sunDirection);

    // Create smooth day/night transition
    float dayFactor = smoothstep(-0.1, 0.1, sunDot);

    // Sample day texture
    vec4 dayColor = texture(uDiffuseTexture, TexCoord);

    // Create procedural night texture (city lights)
    vec4 nightColor = dayColor * 0.1; // Darken the day texture

    // Add city lights based on land areas (green/brown in day texture)
    float landMask = step(0.3, dayColor.g + dayColor.r - dayColor.b * 1.5);

    // Create city light pattern
    vec2 cityUV = TexCoord * 50.0; // Scale for city grid
    float cityNoise = sin(cityUV.x * 3.14159) * sin(cityUV.y * 3.14159);
    cityNoise = step(0.7, cityNoise); // Threshold for city lights

    // Add city lights to night side
    vec3 cityLights = vec3(1.0, 0.9, 0.6) * cityNoise * landMask * 0.5;
    nightColor.rgb += cityLights;

    // Blend day and night
    vec4 finalColor = mix(nightColor, dayColor, dayFactor);

    // Apply basic lighting (your existing lighting system will handle this too)
    float ambientStrength = 0.1;
    float lightStrength = max(sunDot, 0.0) * 0.8 + ambientStrength;
    finalColor.rgb *= lightStrength;

    // Add atmospheric glow at terminator
    float terminator = abs(sunDot);
    terminator = 1.0 - smoothstep(0.0, 0.3, terminator);
    vec3 atmosphereGlow = vec3(0.3, 0.6, 1.0) * terminator * 0.3;
    finalColor.rgb += atmosphereGlow;

    FragColor = finalColor;
}