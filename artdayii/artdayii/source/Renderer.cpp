#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
using namespace glm;

#include "loadShaders.h"
#include "Renderer.h"

const GLfloat vertex_buffer_data[] = {
		1.0f, 1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f,
};

mat3 yaw(float a) {
	return mat3(
		cos(a), -sin(a), 0,
		sin(a), cos(a), 0,
		0, 0, 1
	);
}

mat3 pitch(float b) {
	return mat3(
		cos(b), 0, sin(b),
		0, 1, 0,
		-sin(b), 0, cos(b)
	);
}

mat3 roll(float c) {
	return mat3(
		1, 0, 0,
		0, cos(c), -sin(c),
		0, sin(c), cos(c)
	);
}

GLuint genQuad() {
	GLuint quadID;
	glGenVertexArrays(1, &quadID);
	glBindVertexArray(quadID);

	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer_data), vertex_buffer_data, GL_STATIC_DRAW);

	return buffer;
}

void Renderer::render() {
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(program);

	glUniform2fv(glGetUniformLocation(program, "res"), 1, &vec2(1366, 700)[0]);
	glUniform1f(glGetUniformLocation(program, "aspect"), 1366 / 700);
	glUniform1f(glGetUniformLocation(program, "dist"), dist);
	glUniform3fv(glGetUniformLocation(program, "pos"), 1, &pos[0]);
	glUniform3fv(glGetUniformLocation(program, "facing"), 1, &facing[0]);
	glUniform2fv(glGetUniformLocation(program, "fov"), 1, &fov[0]);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, quad);
	glVertexAttribPointer(
		0,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0
	);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glDisableVertexAttribArray(0);
};

bool Renderer::init(const char* vert, const char* frag) {
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		return false;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	int count;
	window = glfwCreateWindow(1366, 768, "ART", NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window.");
		glfwTerminate();
		return false;
	}
	glfwMakeContextCurrent(window);

	glewExperimental = true;
	if (glewInit() != GLEW_OK) {
		glfwTerminate();
		return false;
	}

	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	program = LoadShaders(vert, frag);

	quad = genQuad();

	facing = vec3(0, 0, 0);
	pos = vec3(0, 0, 0);
	fov = vec2(3);
	dist = 100;

	return true;
};

void Renderer::advance(float dist) {
	pos += getFacing() * dist;
}

vec3 Renderer::getFacing() {
	return vec3(0, 0, 1) * yaw(facing.z) * pitch(facing.x) * roll(facing.y);
}