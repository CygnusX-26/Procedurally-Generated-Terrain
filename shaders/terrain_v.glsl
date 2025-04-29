#version 330 core
layout(location = 0) in vec3 aPos;

uniform mat4 projection;
uniform mat4 view;
out float height;

void main() {
    height = aPos.y;
    gl_Position = projection * view * vec4(aPos, 1.0);
    gl_PointSize = 1.0;
}