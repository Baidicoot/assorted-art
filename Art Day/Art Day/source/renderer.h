#pragma once

#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include "loadShaders.h"
#include "renderer.h"

struct Renderer {
	GLuint program;
	GLuint quad;
	GLFWwindow* window;
	glm::vec3 pos;
	glm::vec3 facing;
	glm::vec2 fov;

	bool init(const char* vert, const char* frag);

	void render(void);
};