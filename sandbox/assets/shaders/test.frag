#version 430 core 

out vec4 fragColor;

void main() {
if(gl_FragCoord.x < 400)
fragColor = vec4(1.0, 0.0, 0.0, 1.0);
else
fragColor = vec4(0.0, 1.0, 0.0, 1.0);
}