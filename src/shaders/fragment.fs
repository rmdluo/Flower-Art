#version 330 core

in vec4 InColor;

out vec4 FragColor;

uniform float color1;
uniform float color2;
uniform float color3;

void main() {
    FragColor = vec4(color1, color2, color3, 1.0f);
}