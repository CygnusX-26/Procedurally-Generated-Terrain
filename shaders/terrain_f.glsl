#version 330 core
out vec4 FragColor;
in float height;

void main() {
    float h = clamp(height / 100.0, 0.0, 1.0);
    vec3 color;
    if (h > 0.95) {
        float t = (h - 0.95) / 0.05;
        color = mix(vec3(0.3, 0.6, 0.3), vec3(1.0), t);
    }
    else if (h > 0.5) {
        float t = (h - 0.5) / (0.9);
        color = mix(vec3(0.2, 0.6, 0.2), vec3(0.4, 0.8, 0.4), t);
    }
    else if (h > 0.35) {
        float t = (h - 0.35) / (0.15);
        color = mix(vec3(0.4, 0.26, 0.13), vec3(0.2, 0.5, 0.2), t);
    }
    else {
        float t = h / 0.35;
        color = mix(vec3(0.0, 0.1, 0.4), vec3(0.0, 0.4, 0.8), t);
    }
        FragColor = vec4(color, 1.0);
}