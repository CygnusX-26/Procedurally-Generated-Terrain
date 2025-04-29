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
	treePoints(0),
	treeMesh(nullptr),
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

void GLState::setupTerrain(const std::vector<glm::vec3>& points, int width, int height) {
	terrainWidth = width;
	terrainHeight = height;
    glGenVertexArrays(1, &terrainVao);
    glGenBuffers(1, &terrainVbo);

    glBindVertexArray(terrainVao);
    glBindBuffer(GL_ARRAY_BUFFER, terrainVbo);
    glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(glm::vec3), points.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);


    std::vector<unsigned int> indices;
    for (unsigned int y = 0; y < width-1; y++) {
        for (unsigned int x = 0; x < height-1; x++) {
            indices.push_back(y * width + x);
            indices.push_back((y + 1) * width + x);
            indices.push_back(y * width + (x + 1));
            
            indices.push_back((y + 1) * width + x);
            indices.push_back((y + 1) * width + (x + 1));
            indices.push_back(y * width + (x + 1));
        }
    }
    glGenBuffers(1, &terrainEbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrainEbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
    terrainIndexCount = indices.size();

    glBindVertexArray(0);
}

void GLState::setupTrees(const std::vector<glm::vec4>& points, float height, float width, float cheight, float cwidth) {
	treePoints = points;
	treeMesh = std::unique_ptr<TreeMesh>(new TreeMesh(height, width, cheight, cwidth, 12));
}

void GLState::setupRocks(const std::vector<glm::vec3>& points, float radius, float roughness = 0.2f, int slices = 12, int seed = 0) {
	rockPoints = points;
	rockMesh = std::unique_ptr<RockMesh>(new RockMesh(radius, roughness, slices, seed));
}

// Called when window requests a screen redraw
void GLState::paintGL() {
	glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float) (width / (height * 1.0)), 64.0f, 2048.0f);
	glm::mat4 view = glm::lookAt(camCoords, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glEnableVertexAttribArray(0);

	//Render terrain
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(terrainShader);
	glBindVertexArray(terrainVao);

    glUniformMatrix4fv(glGetUniformLocation(terrainShader, "projection"), 1, GL_FALSE, glm::value_ptr(proj));
    glUniformMatrix4fv(glGetUniformLocation(terrainShader, "view"), 1, GL_FALSE, glm::value_ptr(view));

    glDisable(GL_CULL_FACE);
	glDrawElements(GL_TRIANGLES, terrainIndexCount, GL_UNSIGNED_INT, 0);
	glEnable(GL_CULL_FACE);

	glCullFace(GL_BACK);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(0);

    //Render trees
    glUseProgram(treeShader);
	glBindVertexArray(treeMesh->getVao());

    glUniformMatrix4fv(glGetUniformLocation(treeShader, "projection"), 1, GL_FALSE, glm::value_ptr(proj));
    glUniformMatrix4fv(glGetUniformLocation(treeShader, "view"), 1, GL_FALSE, glm::value_ptr(view));

	glDisable(GL_CULL_FACE);

	for (auto& point : treePoints) {
		glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(point.x, point.y, point.z));
		glUniformMatrix4fv(glGetUniformLocation(treeShader, "model"), 1, GL_FALSE, glm::value_ptr(model));
	
		glDrawElements(GL_TRIANGLES, treeMesh->getIndexCount(), GL_UNSIGNED_INT, 0);
	}

	//Render rocks
	glUseProgram(rockShader);
	glBindVertexArray(rockMesh->getVao());

    glUniformMatrix4fv(glGetUniformLocation(rockShader, "projection"), 1, GL_FALSE, glm::value_ptr(proj));
    glUniformMatrix4fv(glGetUniformLocation(rockShader, "view"), 1, GL_FALSE, glm::value_ptr(view));

	for (auto& point : rockPoints) {
		glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(point.x, point.y, point.z));
		glUniformMatrix4fv(glGetUniformLocation(rockShader, "model"), 1, GL_FALSE, glm::value_ptr(model));
	
		glDrawElements(GL_TRIANGLES, rockMesh->getIndexCount(), GL_UNSIGNED_INT, 0);
	}

	glEnable(GL_CULL_FACE);


	

	glCullFace(GL_BACK);
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
	std::vector<GLuint> treeShaders;
	std::vector<GLuint> rockShaders;
	terrainShaders.push_back(compileShader(GL_VERTEX_SHADER, "shaders/terrain_v.glsl"));
	terrainShaders.push_back(compileShader(GL_FRAGMENT_SHADER, "shaders/terrain_f.glsl"));
	terrainShader = linkProgram(terrainShaders);

	treeShaders.push_back(compileShader(GL_VERTEX_SHADER, "shaders/trees_v.glsl"));
	treeShaders.push_back(compileShader(GL_FRAGMENT_SHADER, "shaders/trees_f.glsl"));
	treeShader = linkProgram(treeShaders);

	rockShaders.push_back(compileShader(GL_VERTEX_SHADER, "shaders/rocks_v.glsl"));
	rockShaders.push_back(compileShader(GL_FRAGMENT_SHADER, "shaders/rocks_f.glsl"));
	rockShader = linkProgram(rockShaders);

	for (auto s : terrainShaders)
		glDeleteShader(s);
	terrainShaders.clear();
	for (auto s : treeShaders)
		glDeleteShader(s);
	treeShaders.clear();
	for (auto s : rockShaders)
		glDeleteShader(s);
	rockShaders.clear();
}
