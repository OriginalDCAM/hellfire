// Material uniforms
uniform sampler2D uDiffuseTexture;
uniform sampler2D uNormalTexture;
uniform sampler2D uSpecularTexture;
uniform sampler2D uRoughnessTexture;
uniform sampler2D uMetallicTexture;
uniform sampler2D uAOTexture;
uniform sampler2D uEmissiveTexture;

// Texture usage flags
uniform bool useUDiffuseTexture;
uniform bool useUNormalTexture;
uniform bool useUSpecularTexture;
uniform bool useURoughnessTexture;
uniform bool useUMetallicTexture;
uniform bool useUAOTexture;
uniform bool useUEmissiveTexture;

// Material properties
uniform vec3 uAmbientColor;
uniform vec3 uDiffuseColor;
uniform vec3 uSpecularColor; 
uniform float uShininess;

// Camera position for specular calculations
uniform vec3 viewPos;

// UV controls
uniform vec2 uvTiling;
uniform vec2 uvOffset;
uniform float uvRotation;
uniform int textureWrapMode;

// Transparency uniforms
uniform float uAlpha;
uniform float uTransparency;
uniform bool useTransparency;

