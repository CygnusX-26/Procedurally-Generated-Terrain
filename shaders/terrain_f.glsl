#version 330 core
out vec4 FragColor;
in float height;
in float biome;

vec3 getHighDesert(float h) {
    float t = (h - 0.95) / 0.05;
    return mix(vec3(0.9, 0.8, 0.4), vec3(1.0), t);
}

vec3 getHighForest(float h) {
    float t = (h - 0.95) / 0.05;
    return mix(vec3(0.4, 0.8, 0.4), vec3(1.0), t);
}

vec3 getMidDesert(float h) {
    float t = (h - 0.5) / 0.5;
    return mix(vec3(0.76, 0.58, 0.42), vec3(0.9, 0.8, 0.4), t);
}

vec3 getMidForest(float h) {
    float t = (h - 0.5) / (0.9);
    return mix(vec3(0.2, 0.6, 0.2), vec3(0.4, 0.8, 0.4), t);
}

vec3 getLowDesert(float h) {
    float t = (h - 0.35) / 0.15;
    return mix(vec3(0.5, 0.4, 0.2), vec3(0.76, 0.58, 0.42), t);
}

vec3 getLowForest(float h) {
    float t = (h - 0.35) / (0.15);
    return mix(vec3(0.4, 0.26, 0.13), vec3(0.2, 0.5, 0.2), t);
}

float vary(float x) {
    return fract(sin(x * 43758.5453) * 15731.951);
}
void main() {
    float h = clamp(height / 100.0, 0.0, 1.0);
    float biomeT = clamp(biome, 0.0, 1.0); 
    vec3 color;
    vec3 highColor = mix(getHighDesert(h), getHighForest(h), biomeT);
    vec3 midColor = mix(getMidDesert(h), getMidForest(h), biomeT);
    vec3 lowColor = mix(getLowDesert(h), getLowForest(h), biomeT);

    if (h > 0.95) {
        color = highColor;
    }
    else if (h > 0.5) {
        color = midColor;
    }
    else if (h > 0.35) {
        color = lowColor;
    }
    else {
        float t = h / 0.35;
        color = mix(vec3(0.0, 0.1, 0.4), vec3(0.0, 0.4, 0.8), t);
    }
    FragColor = vec4(color, 1.0);
}