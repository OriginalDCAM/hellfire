#include <iostream>

#include <GL/glew.h>
#include <GL/freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../Engine/Core/Graphics/Renderer.h"
#include "../Engine/Core/Graphics/GL/glsl.h"
#include "../Engine/Core/Graphics/GL/IB.h"
#include "../Engine/Core/Graphics/GL/Geometry/Cube.h"
#include "../Engine/Core/Graphics/GL/Geometry/Plane.h"
#include "../Engine/Helpers/PerspectiveCamera.h"

//--------------------------------------------------------------------------------
// Consts
//--------------------------------------------------------------------------------

const int WIDTH = 800, HEIGHT = 600;

const char* fragshader_name = "fragmentshader.frag";
const char* vertexshader_name = "vertexshader.vert";

unsigned const int DELTA_TIME = 10;

// Global variables
PerspectiveCamera* camera = nullptr;
//Cube* cube = nullptr;
Object3D* scene = nullptr;
//Cube* cube2 = nullptr;
Renderer* renderer = nullptr;
GLuint program_id;

std::vector<Cube*> cubes = {};

bool keys[256] = { false };
int last_mouse_x = WIDTH / 2;
int last_mouse_y = HEIGHT / 2;
bool first_mouse = true;
float last_frame_time = 0.0f;
//--------------------------------------------------------------------------------
// Keyboard handling
//--------------------------------------------------------------------------------

void keyboardHandler(unsigned char key, int a, int b)
{
	float deltaTime = 0.016f; // Fixed timestep or calculate from actual frame time

	switch (key) {
	case 27:  exit(0); break; // ESC key
	}

	glutPostRedisplay();
}

void keyboardDown(unsigned char key, int x, int y) {
	keys[key] = true;

	if (key == 27) {
		exit(0);  // Or your own cleanup function
	}
}

void keyboardUp(unsigned char key, int x, int y) {
	keys[key] = false;
}

// Improved mouse motion handler
void mouseMotion(int x, int y) {
	if (first_mouse) {
		last_mouse_x = x;
		last_mouse_y = y;
		first_mouse = false;
		return;
	}

	float x_offset = x - last_mouse_x;
	float y_offset = last_mouse_y - y; // Reversed

	last_mouse_x = x;
	last_mouse_y = y;

	camera->process_mouse_movement(x_offset, y_offset);

	// Reset mouse to center if it gets too close to the edge
	if (x < WIDTH / 4 || x > WIDTH * 3 / 4 || y < HEIGHT / 4 || y > HEIGHT * 3 / 4) {
		glutWarpPointer(WIDTH / 2, HEIGHT / 2);
		last_mouse_x = WIDTH / 2;
		last_mouse_y = HEIGHT / 2;
	}
}

// Mouse wheel callback
void mouseWheel(int button, int dir, int x, int y) {
	if (dir > 0) {
		camera->process_mouse_scroll(-1.0f);
	}
	else {
		camera->process_mouse_scroll(1.0f);
	}
	glutPostRedisplay();
}

// Main loop
void Loop()
{
	// Calculate delta time
	float current_time = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
	float delta_time = current_time - last_frame_time;
	last_frame_time = current_time;

	// Process continuous input
	if (keys['w']) camera->process_keyboard(FORWARD, delta_time);
	if (keys['s']) camera->process_keyboard(BACKWARD, delta_time);
	if (keys['a']) camera->process_keyboard(LEFT, delta_time);
	if (keys['d']) camera->process_keyboard(RIGHT, delta_time);
	if (keys['q']) camera->process_keyboard(DOWN, delta_time);
	if (keys['e']) camera->process_keyboard(UP, delta_time);

	if (scene && camera)
	{
		for (auto* cube : cubes)
		{
			cube->update();
		}

		camera->update();
		//cube2->update();
		scene->update_world_matrix();
		renderer->Render(*scene, *camera);
	}
}

void TimerLoop(int n)
{
	Loop();
	glutTimerFunc(DELTA_TIME, TimerLoop, 0);
}

void InitGlutGlew(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutCreateWindow("Hello OpenGL");
	glutDisplayFunc(Loop);
	glutTimerFunc(DELTA_TIME, TimerLoop, 0);

	glutKeyboardFunc(keyboardDown);
	glutKeyboardUpFunc(keyboardUp);
	glutPassiveMotionFunc(mouseMotion);
	glutMouseFunc(mouseWheel);

	glutSetCursor(GLUT_CURSOR_NONE);
	glutWarpPointer(WIDTH / 2, HEIGHT / 2);

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	glewInit();
}


void Setup()
{
	scene = new Object3D();
	camera = new PerspectiveCamera(45.0f, (float)WIDTH / HEIGHT, 0.1f, 100.0f);
	camera->set_position(glm::vec3(0.0f, 0.0f, 10.0f)); // Position camera 10 units back

	vector<glm::vec3> cubePositions = {
	glm::vec3(0.0f,  0.0f,  0.0f),
	glm::vec3(2.0f,  5.0f, -15.0f),
	glm::vec3(-1.5f, -2.2f, -2.5f),
	glm::vec3(-3.8f, -2.0f, -12.3f),
	glm::vec3(2.4f, -0.4f, -3.5f),
	glm::vec3(-1.7f,  3.0f, -7.5f),
	glm::vec3(1.3f, -2.0f, -2.5f),
	glm::vec3(1.5f,  2.0f, -2.5f),
	glm::vec3(1.5f,  0.2f, -1.5f),
	glm::vec3(-1.3f,  1.0f, -1.5f)
	};

	for (auto const position : cubePositions)
	{
		auto* new_cube = new Cube(0.25, 0.25, 0.25);
		new_cube->set_position(position);
		new_cube->set_scale(glm::vec3(0.5f, 0.5f, 0.5f));
		scene->add(new_cube);
		cubes.push_back(new_cube);
	}

	auto* plane = new Plane();
	
	plane->set_position(glm::vec3(0.0f,-5.0f, -0.0f));
	plane->set_scale(glm::vec3(50.0f, 50.0f, 50.0f));
	plane->set_rotation(90.0f, glm::vec3(1.0, 0.0, 0.0));

	scene->add(plane);

	//cube = new Cube();
	//cube2 = new Cube();

	//scene->add(cube);
	//cube->add(cube2);

	//cube->set_position(glm::vec3(0.0f, 0.0f, 0.0f));
	//cube2->set_position(glm::vec3(-3.0f, 0.0f, 0.0f));
	//cube->set_scale(glm::vec3(0.5, 0.5, 0.5));

	//cube2->get_model().meshes[0].is_wireframe = true;

	scene->update_world_matrix();

	renderer = new Renderer(program_id);
}

void InitShaders()
{
	char* vertexshader = glsl::readFile(vertexshader_name);
	GLuint vsh_id = glsl::makeVertexShader(vertexshader);
	char* fragshader = glsl::readFile(fragshader_name);
	GLuint fsh_id = glsl::makeFragmentShader(fragshader);
	program_id = glsl::makeShaderProgram(vsh_id, fsh_id);
}

int main(int argc, char** argv)
{
	InitGlutGlew(argc, argv);
	InitShaders();

	glutIdleFunc([]() {
		static bool initialized = false;
		if (!initialized) {
			Setup();
			initialized = true;
		}
	});

	// Hide console window
	//HWND hWnd = GetConsoleWindow();
	//ShowWindow(hWnd, SW_HIDE);

	// Main loop
	glutMainLoop();

	// Clean up
	delete camera;
	delete renderer;

	return 0;
}

