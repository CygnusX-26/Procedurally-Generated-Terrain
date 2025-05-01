#version 330 core
layout(location = 0) in vec4 aPos;


uniform mat4 projection;
uniform mat4 view;
out float height;
out float biome;

void main() {
    height = aPos.y;
    biome = aPos.w;
    gl_Position = projection * view * vec4(aPos.x, aPos.y, aPos.z, 1.0);
    gl_PointSize = 1.0;
}