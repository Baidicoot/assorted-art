#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>

#include <GLFW/glfw3.h>
GLFWwindow* window;

#include <glm/glm.hpp>
using namespace glm;

#include "loadShaders.h"
#include "renderer.h"

int main(void)
{
	Renderer camera;
	if (!camera.init("vert.glsl", "frag.glsl")) {
		return -1;
	}

	do {
		camera.render();
	} while (glfwGetKey(camera.window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(camera.window) == 0);

	glfwTerminate();

	return 0;
}