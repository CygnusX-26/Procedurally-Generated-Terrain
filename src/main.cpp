#define NOMINMAX
#include <iostream>
#include <memory>
#include <filesystem>
#include <algorithm>
#include <chrono>
#include "glstate.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GL/freeglut.h>

#include "perlin.hpp"
#include "tree.hpp"
#include "rock.hpp"
#include "config.hpp"

#define HEIGHT (512)
#define WIDTH (512)

namespace fs = std::filesystem;

// Menu identifiers
const int MENU_OBJBASE = 64;				// Select object to view
const int MENU_SHADING_PHONG = 4;			// Shading mode
const int MENU_SHADING_NORMALS = 6;
const int MENU_EXIT = 1;					// Exit application

// perlin noise state



// OpenGL state
int width, height;
std::unique_ptr<GLState> glState;
unsigned int activeLight = 0;

// Initialization functions
void initGLUT(int* argc, char** argv);
void initMenu();

// Callback functions
void display();
void reshape(GLint width, GLint height);
void keyPress(unsigned char key, int x, int y);
void keyRelease(unsigned char key, int x, int y);
void mouseBtn(int button, int state, int x, int y);
void mouseMove(int x, int y);
void idle();
void menu(int cmd);
void cleanup();

// Program entry point
int main(int argc, char** argv) {
	Config configuration("terrainConfig.txt");
	std::vector<glm::vec3> perlinNoise;
	std::vector<glm::vec4> trees;
	std::vector<glm::vec3> rocks;
	PerlinNoise perlin(configuration.getConfigPerlin());
	TreeNoise treeNoise(configuration.getConfigTree());
	RockNoise rockNoise(configuration.getConfigRock());
	float frequency = configuration.getConfigPerlinFreq();
	for (int y = 0; y < HEIGHT; ++y) {
		for (int x = 0; x < WIDTH; ++x) {
			double fx = x / (WIDTH * 1.0);
			double fy = y / (HEIGHT * 1.0);
			double noise = perlin.noise(fx, fy, frequency) * 100.0f;
			float centeredX = x - WIDTH / 2.0f;
        	float centeredY = y - HEIGHT / 2.0f;
			perlinNoise.emplace_back(centeredX, noise, centeredY);
			if (treeNoise.hasTree(noise, 95, 35, configuration.getConfigTreeFreq())) {
				trees.emplace_back(centeredX, noise, centeredY, treeNoise.treeHeight());
			}
			else if (rockNoise.hasRock(noise, 100, 0, configuration.getConfigRockFreq())) {
				rocks.emplace_back(centeredX, noise, centeredY);
			}
		}
	}
	// tree vec is x, y, z, tree height

	try {
		// Create the window and menu
		initGLUT(&argc, argv);
		initMenu();
		std::vector<float> treeProps = configuration.getConfigTreeProp();
		// Initialize OpenGL (buffers, shaders, etc.)
		glState = std::unique_ptr<GLState>(new GLState());
		glState->initializeGL();
		glState->setupTerrain(perlinNoise, WIDTH, HEIGHT);
		glState->setupTrees(trees, treeProps[0], treeProps[1], treeProps[2], treeProps[3]);

	} catch (const std::exception& e) {
		// Handle any errors
		std::cerr << "Fatal error: " << e.what() << std::endl;
		cleanup();
		return -1;
	}

	// Execute main loop
	glutMainLoop();

	return 0;
}

void initGLUT(int* argc, char** argv) {
	width = 800; height = 600;
	glutInit(argc, argv);
	glutInitWindowSize(width, height);
	glutInitContextVersion(3, 3);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	glutCreateWindow("Procedural Terrain");

	// GLUT callbacks
	glutDisplayFunc(display); //triggered when window is redrawn
	glutReshapeFunc(reshape); // triggered when window is resized
	glutKeyboardFunc(keyPress); // triggered when key pressed
	glutKeyboardUpFunc(keyRelease); // triggered when key released
	glutMouseFunc(mouseBtn); // triggered when mouse button is clicked
	glutMotionFunc(mouseMove); //  triggered when mouse is moved with button down
	glutIdleFunc(idle); // triggered when nothing is happening
	glutCloseFunc(cleanup); // triggered when window closes
}

void initMenu() {
	int shadingMenu = glutCreateMenu(menu);
	glutAddMenuEntry("Phong", MENU_SHADING_PHONG);
	glutAddMenuEntry("Normals", MENU_SHADING_NORMALS);

	// Create the main menu, adding the objects menu as a submenu
	glutCreateMenu(menu);
	glutAddSubMenu("Shading", shadingMenu);
	glutAddMenuEntry("Exit", MENU_EXIT);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

}

// Called whenever a screen redraw is requested
void display() {
	// Tell the GLState to render the scene
	glState->paintGL();

	// Scene is rendered to the back buffer, so swap the buffers to display it
	glutSwapBuffers();
}

// Called when the window is resized
void reshape(GLint w, GLint h) {
	// Tell OpenGL the new window size
	width = w; height = h;
	glState->resizeGL(width, height);
}

// Called when a key is pressed
void keyPress(unsigned char key, int x, int y) {

}

// Called when a key is released
void keyRelease(unsigned char key, int x, int y) {
	
}

// Called when a mouse button is pressed or released
void mouseBtn(int button, int state, int x, int y) {
	int modifiers = glutGetModifiers();

	// Press left mouse button
	if (state == GLUT_DOWN && button == GLUT_LEFT_BUTTON) {
		// Start rotating the active light if holding shift
		if (modifiers & GLUT_ACTIVE_SHIFT) {
			float scale = glm::min((float)width, (float)height);
			glState->getLight(activeLight).beginRotate(
				glm::vec2(x / scale, y / scale));

		// Start rotating the camera otherwise
		} else
			glState->beginCameraRotate(glm::vec2(x, y));
	}
	// Release left mouse button
	if (state == GLUT_UP && button == GLUT_LEFT_BUTTON) {
		// Stop camera and light rotation
		glState->endCameraRotate();
		glState->getLight(activeLight).endRotate();
	}
	// Scroll wheel up
	if (button == 3) {
		// Offset the active light if holding shift
		if (modifiers & GLUT_ACTIVE_SHIFT)
			glState->getLight(activeLight).offsetLight(-0.05f);

		// "Zoom in" otherwise
		else
			glState->offsetCamera(-10.0f);
		glutPostRedisplay();
	}
	// Scroll wheel down
	if (button == 4) {
		// Offset the active light if holding shift
		if (modifiers & GLUT_ACTIVE_SHIFT)
			glState->getLight(activeLight).offsetLight(0.05f);

		// "Zoom out" otherwise
		else
			glState->offsetCamera(10.0f);
		glutPostRedisplay();
	}
}

// Called when the mouse moves
void mouseMove(int x, int y) {
	if (glState->isCamRotating()) {
		// Rotate the camera if currently rotating
		glState->rotateCamera(glm::vec2(x, y));
		glutPostRedisplay();	// Request redraw

	} else if (glState->getLight(activeLight).isRotating()) {
		float scale = glm::min((float)width, (float)height);
		glState->getLight(activeLight).rotateLight(
			glm::vec2(x / scale, y / scale));
		glutPostRedisplay();
	}
}

static auto start = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());  // record start time
// Called when there are no events to process
void idle() {
	// Anything that happens every frame (e.g. movement) should be done here
	// Be sure to call glutPostRedisplay() if the screen needs to update as well

	auto	finish	= std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());	// record end time
	auto	elapsed = static_cast<float>((finish - start).count());

	if ((int)elapsed % (int)(1000.0/60.0) == 0) {
		glState->update_time(elapsed);
		glutPostRedisplay();
	}
}

// Called when a menu button is pressed
void menu(int cmd) {}

// Called when the window is closed or the event loop is otherwise exited
void cleanup() {
	glState.reset(nullptr);
}
