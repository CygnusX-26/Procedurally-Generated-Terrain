#define NOMINMAX
#include <fstream>
#include <sstream>
#include <iostream>
#include "glstate.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include "util.hpp"
#include "mesh.hpp"

// Constructor
GLState::GLState() :
	width(1), height(1),
	camRotating(false)
	{
    	camCoords = glm::vec3(0.0f, 50.0f, 512.0f);
	}

// Destructor
GLState::~GLState() {
	// Release OpenGL resources
	if (shader)	glDeleteProgram(shader);
	if (depthShader) glDeleteProgram(depthShader);
}

// Called when OpenGL context is created (some time after construction)
void GLState::initializeGL() {
	// General settings
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_PROGRAM_POINT_SIZE);
	glPointSize(5.0f);
	

	// Initialize OpenGL state
	initShaders();

	// Create lights
	lights.resize(Light::MAX_LIGHTS);

	// Set initialized state
	init = true;
}

GLuint vao, vbo;

void GLState::setupBuffers(const std::vector<glm::vec3>& points) {
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(glm::vec3), points.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);
}

// Called when window requests a screen redraw
void GLState::paintGL() {
	glEnableVertexAttribArray(0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(terrainShader);
	glBindVertexArray(vao);

	glm::mat4 terrain_proj = glm::perspective(glm::radians(45.0f), (float) (width / (height * 1.0)), 64.0f, 2048.0f);

	glm::mat4 terrain_view = glm::lookAt(camCoords, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glUniformMatrix4fv(glGetUniformLocation(terrainShader, "projection"), 1, GL_FALSE, glm::value_ptr(terrain_proj));
    glUniformMatrix4fv(glGetUniformLocation(terrainShader, "view"), 1, GL_FALSE, glm::value_ptr(terrain_view));

    glDrawArrays(GL_POINTS, 0, 512 * 512);

	glCullFace(GL_BACK);  // Reset
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(0);
}

// Called when window is resized
void GLState::resizeGL(int w, int h) {
	// Tell OpenGL the new dimensions of the window
	width = w;
	height = h;
	glViewport(0, 0, w, h);
}
// Start rotating the camera (click + drag)
void GLState::beginCameraRotate(glm::vec2 mousePos) {
	camRotating = true;
	float radius = glm::length(camCoords);
    float yaw = glm::atan(camCoords.y, camCoords.x);
    float pitch = glm::asin(camCoords.y / radius);
    
    initCamRot = glm::vec2(yaw, pitch);
	initMousePos = mousePos;
}

// Stop rotating the camera (mouse button is released)
void GLState::endCameraRotate() {
	camRotating = false;
}

// Moves the camera toward / away from the origin (scroll wheel)
void GLState::offsetCamera(float offset) {
    float radius = glm::clamp(glm::length(camCoords) + offset, 512.0f, 1024.0f);
    float theta = glm::atan(camCoords.y, camCoords.x);
    float r = glm::length(camCoords);
    float phi = glm::acos(camCoords.z / r);
    camCoords = glm::vec3(
        radius * glm::sin(phi) * glm::cos(theta),
        radius * glm::sin(phi) * glm::sin(theta),
        radius * glm::cos(phi)
    );
}

// Use mouse delta to determine new camera rotation
void GLState::rotateCamera(glm::vec2 mousePos) {
    if (camRotating) {
        // Adjust for the scale based on screen dimensions
        float rotScale = glm::min(width / 450.0f, height / 270.0f);
        glm::vec2 mouseDelta = (mousePos - initMousePos) / rotScale;
		initMousePos = mousePos;
    
        if (glm::length(mouseDelta - glm::vec2(camCoords)) > FLT_EPSILON) {
			yaw -= mouseDelta.x;
			pitch += mouseDelta.y;
			pitch = glm::clamp(pitch, -89.0f, 89.0f);
            float yawR = glm::radians(yaw);
			float pitchR = glm::radians(pitch);

			float radius = glm::length(camCoords);

			camCoords = glm::vec3(
				radius * cos(pitchR) * sin(yawR),
				radius * sin(pitchR),
				radius * cos(pitchR) * cos(yawR)
			);
        }
    }
}


void GLState::update_time(float time) {
	cur_time = time;
}

// Create shaders and associated state
void GLState::initShaders() {
	std::vector<GLuint> terrainShaders;
	terrainShaders.push_back(compileShader(GL_VERTEX_SHADER, "shaders/terrain_v.glsl"));
	terrainShaders.push_back(compileShader(GL_FRAGMENT_SHADER, "shaders/terrain_f.glsl"));
	terrainShader = linkProgram(terrainShaders);
	for (auto s : terrainShaders)
		glDeleteShader(s);
	terrainShaders.clear();
}
