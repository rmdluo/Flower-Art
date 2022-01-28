#version 330 core
layout (location = 0) in vec2 coords;
layout (location = 1) in vec3 color;

out vec4 InColor;

uniform float timeFactor;

void main() {
    gl_Position = vec4(timeFactor * coords, 0.0f, 1.0f);
    InColor = vec4(color, 1.0f);
}