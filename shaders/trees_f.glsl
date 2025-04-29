#version 330 core

in float vType;
out vec4 FragColor;

void main() {
    vec3 color;
    if (vType == 0) {
        color = vec3(0.55, 0.27, 0.07); // Trunk color
    } else {
        color = vec3(0.0, 0.3, 0.0); // Leaves color
    }
    FragColor = vec4(color, 1.0);
}
