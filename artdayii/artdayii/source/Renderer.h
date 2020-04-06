#pragma once

#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include "loadShaders.h"
#include "Renderer.h"


struct Renderer {
	GLuint program;
	GLuint quad;
	GLFWwindow* window;
	vec3 pos;
	vec3 facing;
	vec2 fov;
	float dist;

	bool init(const char* frag, const char* vert);

	void render();

	void setCameraPos(float x, float y, float z);

	vec3 getFacing();

	void advance(float dist);
};