#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
using namespace glm;

#include "loadShaders.h"
#include "Renderer.h"
#include "Object.h"

int main(void)
{
	Renderer camera;
	if (!camera.init("vert.glsl", "frag.glsl")) {
		return false;
	}

	Sphere objects[OBJS] = {
		Sphere {
			vec4(10, 10, 10, 1.25),
			vec4(0, 0, 1, 1),
		}
	};

	float i = 0;
	
	int nFrames = 0;
	double last = glfwGetTime();

	float yaw = 0;
	float pitch = 0.0;
	float roll = 0;

	camera.setCameraPos(0, 0, 0);
	camera.cameraFov = vec2(0.5, 0.5);

	float turnRate = 0.025;
	float moveRate = 0.5;

	do {
		vec<2, double> cursor;
		glfwGetCursorPos(camera.window, &cursor.x, &cursor.y);

		cursor /= vec2(1366, 768);
		cursor *= 2;
		cursor -= 1;

		camera.renderObjects(objects, vec4(camera.cameraPos, 0));

		if (glfwGetKey(camera.window, GLFW_KEY_A) == GLFW_PRESS) {
			yaw -= turnRate;
		}
		if (glfwGetKey(camera.window, GLFW_KEY_D) == GLFW_PRESS) {
			yaw += turnRate;
		}
		if (glfwGetKey(camera.window, GLFW_KEY_W) == GLFW_PRESS) {
			pitch += turnRate;
		}
		if (glfwGetKey(camera.window, GLFW_KEY_S) == GLFW_PRESS) {
			pitch -= turnRate;
		}
		if (glfwGetKey(camera.window, GLFW_KEY_R) == GLFW_PRESS) {
			camera.advance(-moveRate);
		}
		if (glfwGetKey(camera.window, GLFW_KEY_F) == GLFW_PRESS) {
			camera.advance(moveRate);
		}

		double curr = glfwGetTime();

		camera.cameraFacing = vec3(yaw, pitch, roll);

		i += 0.5;
		nFrames++;
		if (curr - last >= 1.0) {
			std::cout << nFrames << " Frames Per Second" << std::endl;
			nFrames = 0;
			last = curr;
		}

		glfwSwapBuffers(camera.window);
		glfwPollEvents();
	} while (glfwGetKey(camera.window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(camera.window) == 0);

	glfwTerminate();

	return 0;
}