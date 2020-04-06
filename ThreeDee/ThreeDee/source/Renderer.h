#pragma once

#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include "loadShaders.h"
#include "Renderer.h"
#include "Object.h"

#define OBJS 1

struct Renderer {
	GLuint program;
	GLuint quad;
	GLFWwindow* window;
	vec3 cameraPos;
	vec3 cameraFacing;
	vec2 cameraFov;

	bool init(const char* frag, const char* vert);

	void renderObjects(Sphere objects[OBJS], vec4 lighting);

	void setCameraPos(float x, float y, float z);

	vec3 getFacing();

	void advance(float dist);
};