#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include <vector>
#include <iostream>

using namespace std;
using namespace glm;

const int width = 400;
const int height = 400;

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Object/Object.h"
#include "OpenGL/Helper.h"
#include "Common/Read.h"
#include "Common/Image.h"

GLFWwindow* window;

int main() {

	stbi_set_flip_vertically_on_load(true);

	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		return false;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(width, height, "Scene", NULL, NULL);
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

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	float verts[] = {
		0, 0, 3, 0.5,
	};

	GLuint vertbuff;
	glGenBuffers(1, &vertbuff);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, vertbuff);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(verts), &verts[0], GL_STATIC_DRAW);

	string frag = read("source/Shaders/frag.glsl");
	string vert = read("source/Shaders/vert.glsl");

	GLuint prg = loadProgram(vert.c_str(), frag.c_str());

	GLuint vao;
	ArrayAttrib attribs[] = {
		ArrayAttrib {
			0,
			3,
			GL_FLOAT,
			GL_FALSE,
			0,
			0
		}
	};
	GLuint buffs[1];
	cout << "generated vao: " << genVAOArrBuf(vao, 1, attribs, buffs) << endl;

	writeQuadData(buffs[0]);

	GLfloat data[12];

	glBindBuffer(GL_ARRAY_BUFFER, buffs[0]);
	glGetBufferSubData(GL_ARRAY_BUFFER, 0, 4 * 3, data);

	GLint time = glGetUniformLocation(prg, "time");
	GLint aspect = glGetUniformLocation(prg, "aspect");
	GLint rot = glGetUniformLocation(prg, "rot");
	GLint posoff = glGetUniformLocation(prg, "posoff");
	GLint tris = glGetUniformLocation(prg, "tris");

	vec3 pos = vec3(0);

	//TODO:
	// - allocate SSBO
	// - load scene from file

	float prev = glfwGetTime();
	float last = prev;
	int nframes = 0;

	vec3 up = vec3(0, 1, 0);

	do {
		vec<2, double> cursorPos;
		glfwGetCursorPos(window, &cursorPos.x, &cursorPos.y);

		cursorPos = cursorPos / vec<2, double>(width / 2, height / 2);

		vec3 lookOffset = vec3(sin(cursorPos.x), -cursorPos.y, cos(cursorPos.x));

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
			pos = pos + normalize(vec3(1) * lookOffset) / 10.0f;
		}
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
			pos = pos - normalize(vec3(1) * lookOffset) / 10.0f;
		}
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
			pos = pos - normalize(vec3(1) * cross(lookOffset, up)) / 10.0f;
		}
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
			pos = pos + normalize(vec3(1) * cross(lookOffset, up)) / 10.0f;
		}

		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, vertbuff);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUseProgram(prg);
		glBindVertexArray(vao);
		glUniform1f(aspect, float(height) / float(width));
		glUniform1f(time, glfwGetTime());
		glUniform3fv(posoff, 1, &pos[0]);
		glUniform1i(tris, sizeof(verts) / 4);
		glUniformMatrix4fv(rot, 1, GL_FALSE, &lookAt(pos + lookOffset, pos, up)[0][0]);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		float curr = glfwGetTime();
		if (curr - prev >= 1) {
			prev = curr;
			float deltaT = curr - last;
			cout << nframes << endl;
			nframes = 0;
		}
		last = curr;
		nframes++;

		glfwSwapBuffers(window);
		glfwPollEvents();

	} while(glfwWindowShouldClose(window) == 0 && glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS);
	
	glfwTerminate();

	return 0;
}