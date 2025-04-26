#version 330 core
out vec4 FragColor;
in float height;

float hash(float p) {
    return fract(sin(dot(vec2(p, p) ,vec2(127.1, 311.7))) * 43758.5453123);
}
void main() {
    float h = clamp(height / 100.0, 0.0, 1.0);
    vec3 color;
    if (h > 0.95) {
        float t = (h - 0.95) / 0.05;
        color = mix(vec3(0.3, 0.6, 0.3), vec3(1.0), t);
    }
    else if (h > 0.5) {
        float t = (h - 0.5) / (0.95 - 0.5);
        float noise = hash(h * 10.0);

        vec3 greenA = vec3(0.2, 0.5, 0.2);
        vec3 greenB = vec3(0.4, 0.8, 0.4);
        vec3 baseGreen = mix(greenA, greenB, noise);

        color = baseGreen;
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