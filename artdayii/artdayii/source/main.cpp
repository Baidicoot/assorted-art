#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
using namespace glm;
using namespace std;

#include "loadShaders.h"
#include "Renderer.h"
#include "Object.h"

int main(void)
{
	Renderer camera;
	if (!camera.init("vert.glsl", "frag.glsl")) {
		return false;
	}

	cout << degrees(-1.4) << endl;

	do {
		camera.render();

		double curr = glfwGetTime();
		//camera.advance(0.01);

		if (glfwGetKey(camera.window, GLFW_KEY_Q) == GLFW_PRESS) {
			camera.facing += vec3(0, -0.005, 0);
		}
		if (glfwGetKey(camera.window, GLFW_KEY_E) == GLFW_PRESS) {
			camera.facing += vec3(0, 0.005, 0);
		}
		if (glfwGetKey(camera.window, GLFW_KEY_A) == GLFW_PRESS) {
			camera.facing += vec3(0, 0, 0.005);
		}
		if (glfwGetKey(camera.window, GLFW_KEY_D) == GLFW_PRESS) {
			camera.facing += vec3(0, 0, -0.005);
		}

		if (glfwGetKey(camera.window, GLFW_KEY_W) == GLFW_PRESS) {
			camera.advance(0.01);
		}

		glfwSwapBuffers(camera.window);
		glfwPollEvents();
	} while (glfwGetKey(camera.window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(camera.window) == 0);

	glfwTerminate();

	return 0;
}