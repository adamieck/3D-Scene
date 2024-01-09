#version 410 core

layout(location = 0) in vec4 position;
out vec2 fragCoord;

void main() {
    gl_Position = position;
    fragCoord = position.xy;
}

