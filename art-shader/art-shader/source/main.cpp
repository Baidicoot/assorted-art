#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>

#include <GLFW/glfw3.h>
GLFWwindow* window;

#include <glm/glm.hpp>
using namespace glm;

#include "loadShaders.h"
#include <iostream>
using namespace std;

const GLsizei WIDTH = 1366;
const GLsizei HEIGHT = 768;

const GLfloat vertex_buffer_data[] = {
		1.0f, 1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f,
};

struct State {
	GLsizei w;
	GLsizei h;
	GLsizei vw;
	GLsizei vh;

	GLuint echo;
	GLuint main;
	GLuint fade;
	GLuint front;
	GLuint back;
	GLuint fbo;

	GLuint quad = genQuad();

	void swap() {
		GLuint tmp = front;
		front = back;
		back = tmp;
	}

	void draw(GLfloat time) {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, vw, vh);
		glActiveTexture(front);
		glUseProgram(echo);

		glUniform1i(glGetUniformLocation(echo, "state"), 0);
		glUniform1f(glGetUniformLocation(echo, "aspectRatio"), (float)vw / (float)vh);
		glUniform1f(glGetUniformLocation(echo, "time"), time);

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
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	}

	void trails() {
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, back, 0);
		glActiveTexture(front);
		glViewport(0, 0, w, h);
		glUseProgram(fade);

		glUniform1i(glGetUniformLocation(fade, "state"), 0);
		glUniform1f(glGetUniformLocation(fade, "aspectRatio"), (float)vw / (float)vh);

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

		swap();
	}

	void step(vec2 tracer, GLfloat time) {
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, back, 0);
		glActiveTexture(front);
		glViewport(0, 0, w, h);
		glUseProgram(main);

		glUniform1i(glGetUniformLocation(main, "state"), 0);
		glUniform1f(glGetUniformLocation(main, "aspectRatio"), (float)vw / (float)vh);
		glUniform2fv(glGetUniformLocation(main, "tracer"), 1, &tracer[0]);
		glUniform1f(glGetUniformLocation(main, "time"), time);

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

		swap();
	}

	static GLuint texture(GLsizei w, GLsizei h) {
		GLuint tex;
		glGenTextures(1, &tex);
		glBindTexture(GL_TEXTURE_2D, tex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		return tex;
	}

	static GLuint genQuad() {
		GLuint quadID;
		glGenVertexArrays(1, &quadID);
		glBindVertexArray(quadID);

		GLuint buffer;
		glGenBuffers(1, &buffer);
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer_data), vertex_buffer_data, GL_STATIC_DRAW);

		return buffer;
	}
};

vec2 unUV(vec2 in) {
	return vec2(in.x * 2 - 1, in.y * 2 - 1);
}

vec2 updatePos(vec2 p, float t) {
	float xm = (p.x + 1) / 2;
	float ym = (p.y + 1) / 2;
	return vec2((xm * 1.25 * cos(t/2) - 0.25) - sin(t/89)/11, (ym * sin(t) - 0.4) + cos(t/97)/7);
}

int main() {
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	int count;
	window = glfwCreateWindow(1366, 768, "ART", glfwGetMonitors(&count)[1], NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window.");
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		glfwTerminate();
		return -1;
	}

	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	/* Program Specifics Begin */

	GLuint fbo;
	glGenFramebuffers(1, &fbo);

	State state = State{
		WIDTH,
		HEIGHT,
		WIDTH,
		HEIGHT,
		LoadShaders("quad.vert", "echo.frag"),
		LoadShaders("quad.vert", "render.frag"),
		LoadShaders("quad.vert", "fade.frag"),
		State::texture(WIDTH, HEIGHT),
		State::texture(WIDTH, HEIGHT),
		fbo,
	};

	double t = 0;
	double dt = 0.001;

	int fadeEvery = 0;
	int lastFade = fadeEvery;

	double lastTime = glfwGetTime();
	int nbFrames = 0;
	int nSteps = 0;

	vec2 tracer = vec2(0.875, 0);

	do {
		for (int i = 0; i < 32; i++) {
			tracer = updatePos(tracer, t);
			state.step(tracer, t);
			t += dt;
			nSteps++;
		}

		lastFade--;

		if (lastFade == 0) {
			state.trails();
			lastFade = fadeEvery;
		}

		state.draw(t);

		glfwSwapBuffers(window);
		glfwPollEvents();

		double currTime = glfwGetTime();
		nbFrames++;
		if (currTime - lastTime >= 1.0) {
			cout << "FPS: " << nbFrames << endl;
			cout << "STEPS: " << nSteps << endl;
			nbFrames = 0;
			nSteps = 0;
			lastTime += 1.0;
		}
	} while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);

	glfwTerminate();

	return 0;
}