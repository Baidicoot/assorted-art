#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include <iostream>

using namespace std;
using namespace glm;

const float speed = 0.1;

const int width = 1366;
const int height = 705;

const float aspect_ratio = float(width) / float(height);

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Rendering/Renderer.h"
#include "Object/Object.h"
#include "Common/Read.h"
#include "Common/Image.h"

GLFWwindow* window;

vec3 pos = vec3(0);
vec3 vel = vec3(0);
vec3 lookOffset = vec3(0, 0, 1);
vec3 up = vec3(0, 1, 0);

void look() {
	vec<2, double> cursorPos;
	glfwGetCursorPos(window, &cursorPos.x, &cursorPos.y);

	cursorPos = cursorPos / vec<2, double>(width / 2, height / 2);

	lookOffset = normalize(vec3(sin(-cursorPos.x), -cursorPos.y, cos(-cursorPos.x)));
}

void handleInput() {
	vec3 sideways = cross(lookOffset, up);
	vec3 actualup = cross(lookOffset, sideways);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		pos = pos + normalize(lookOffset) * speed;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		pos = pos - normalize(lookOffset) * speed;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		pos = pos - sideways * speed;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		pos = pos + sideways * speed;
	}
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
		pos = pos + actualup * speed;
	}
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
		pos = pos - actualup * speed;
	}
}

mat4 proj = perspective(radians(90.0f), 1366.0f / 705.0f, 0.01f, 1000.0f);

mat4 calcMatrix() {
	return proj * lookAt(pos, pos + lookOffset, up);
}

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

	int count;
	window = glfwCreateWindow(1366, 705, "Scene", NULL, NULL);
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

	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);

	stbi_set_flip_vertically_on_load(true);

	string raster_v = read("source/Shaders/p2vert.glsl");
	string raster_f = read("source/Shaders/p2frag.glsl");
	GLuint blitter = loadProgram(raster_v.c_str(), raster_f.c_str());

	GLint sspos = glGetUniformLocation(blitter, "sspos");
	GLint strength = glGetUniformLocation(blitter, "strength");
	GLint dist = glGetUniformLocation(blitter, "dist");
	GLint aspect = glGetUniformLocation(blitter, "aspect");
	GLint worldpos = glGetUniformLocation(blitter, "worldpos");
	GLint radius = glGetUniformLocation(blitter, "radius");

	GLint tex = glGetUniformLocation(blitter, "tex");

	int w, h, n;
	void* data = stbi_load("sky.png", &w, &h, &n, 0);
	GLuint sky = loadTex(data, w, h, GL_RGBA, GL_RGBA);

	Scene view = loadScene(read("plane.obj"));
	RenderMesh plane = view.genRenderMesh(view.indexByName("Plane"));
	GlobalMeshBuffer<2> viewMesh;
	ArrayAttrib attribs[2] = {
		ArrayAttrib {
			0,
			3,
			GL_FLOAT,
			GL_FALSE,
			0,
			0
		},
		ArrayAttrib {
			1,
			2,
			GL_FLOAT,
			GL_FALSE,
			0,
			0
		},
	};
	GLuint lengths[2] = {
		sizeof(vec3),
		sizeof(vec2)
	};
	viewMesh.init(attribs, lengths, 1000);
	void* locs[2] = {
		&plane.verts[0][0],
		&plane.uvs[0][0]
	};
	GlobalMesh plane_ID = viewMesh.append(locs, plane.verts.size());

	Scene scene = loadScene(read("ship.obj"));
	RenderMesh monkey = scene.genRenderMesh(scene.indexByName("ship"));
	RenderMesh cube = scene.genRenderMesh(scene.indexByName("Cube"));

	Renderer camera;
	if (!camera.init(width * 2, height * 2, 150000)) {
		return -1;
	}

	GlobalMesh obj = camera.writeObjectToBuffers(monkey);
	GlobalMesh bg = camera.writeObjectToBuffers(cube);

	vec3 com = vec3(0);

	float prev, last;
	last = prev = glfwGetTime();

	int nframes = 0;
	do {
		look();
		handleInput();

		mat4 mvp = calcMatrix();

		camera.beginDraws(pos, vec3(50));
		camera.drawObjectToBuffers(obj, mvp, sky);
		camera.drawObjectToBuffers(bg, mvp, sky);

		glUseProgram(blitter);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, 1366, 705);

		vec4 screenspace = mvp * vec4(com, 1); // calculation is currently wrong, somehow.

		float d = distance(pos, vec3(com.x, com.y, com.z));

		glUniform1f(strength, 0.5f);
		glUniform1f(dist, d);
		glUniform2fv(sspos, 1, &vec2(screenspace.x, screenspace.y)[0]);
		glUniform1f(aspect, aspect_ratio);
		glUniform1f(radius, 0);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		bindTexture2D(0, camera.depthbuffer);
		glUniform1i(tex, 0);

		viewMesh.beginDraws();
		viewMesh.draw(GL_TRIANGLES, plane_ID);

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