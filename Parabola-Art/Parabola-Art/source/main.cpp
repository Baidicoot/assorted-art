#include "main.h"

GLFWwindow* window;

using namespace std;
using namespace glm;

vector<vector<vec3>> genPointArray(int width, int height) {
	vector<vector<vec3>> points;
	for (int x = 0; x < width; x++) {
		vector<vec3> column;
		for (int y = 0; y < height; y++) {
			float norm_xpos = float(x) / (width - 1) * 2 - 1;
			float norm_ypos = float(y) / (height - 1) * 2 - 1;
			column.push_back(vec3(norm_xpos, 0, norm_ypos));
		}
		points.push_back(column);
		// array organised in colums
		// loc = y * height + x
	}
	return points;
}

vector<vector<vec3>> displacePoints(vector<vector<vec3>> points, vec3 dispFunc(vec3 pos)) {
	vector<vector<vec3>> displaced;
	for (int x = 0; x < points.size(); x++) {
		vector<vec3> column;
		for (int y = 0; y < points[0].size(); y++) {
			column.push_back(dispFunc(points[x][y]));
		}
		displaced.push_back(column);
	}

	return displaced;
}

vec3 findAverPos(vector<vector<vec3>> points) {
	vec3 pos(0);
	int count = 0;
	for (int x = 0; x < points.size(); x++) {
		count += points[x].size();
		for (int y = 0; y < points[x].size(); y++) {
			pos += points[x][y];
		}
	}
	return pos / float(count);
}

vec3 hyperbola(vec3 pos) {
	return vec3(pos.x, abs(pos.x * pos.z), pos.z);
}

vec3 parabola(vec3 pos) {
	float x = pos.x;
	float z = pos.z;
	return vec3(pos.x, x*x + z*z - 0.5, pos.z);
}

vec3 cylander(vec3 pos) { //rotate along z-axis
	float offset = pos.x;
	float rotation = offset * 3.141f;
	return vec3(sin(rotation), cos(rotation), pos.z);
}

vec3 cameraPlane(vec3 pos) {
	return vec3(pos.x, pos.z, 1);
}

vec3 strech(vec3 pos) {
	return pos * vec3(1, 1, 0.25);
}

vec3 twist(vec3 pos) {
	float rotation = pos.x * 3.141f * 0.5;
	return vec3(pos.x, sin(rotation) * pos.z, pos.z * cos(rotation));
}

vec3 strechTwistCylander(vec3 pos){
	return cylander(strech(pos));
}

vec3 mobius(vec3 pos) {
	float offset = pos.x;
	float c_rot = offset * 3.141f;
	float t_rot = offset * 3.141f * 0.5;
	float radius = pos.z * 0.25;
	return vec3(sin(c_rot), sin(t_rot) * radius + cos(c_rot), cos(t_rot) * radius);
}

vec2 getPointXY(vec3 pos, int width, int height) {
	float xpos = ((pos.x + 1) / 2) * (width - 1);
	float ypos = ((pos.z + 1) / 2) * (height - 1);
	return vec2(xpos, ypos);
}

vec3 updown(vec3 pos) {
	vec2 cd = getPointXY(pos, WIDTH, HEIGHT);
	cout << cd.x << " " << cd.y << endl;
	ivec2 coords = ivec2(round(cd.x), round(cd.y));
	if (coords.y % 2 == 0) {
		return vec3(pos.x, pos.y + 0.25, pos.z);
	} else {
		return pos;
	}
}

vec3 forwardBack(vec3 pos) {
	vec2 cd = getPointXY(pos, WIDTH, HEIGHT);
	ivec2 coords = ivec2(round(cd.x), round(cd.y));
	if (coords.x % 2 == 0) {
		return vec3(pos.x, pos.y, pos.z + 0.25);
	}
	else {
		return pos;
	}
}

vec3 updownStrech(vec3 pos) {
	return updown(pos) * vec3(0.25, 1, 1);
}

vec3 updownForwardbackStrech(vec3 pos) {
	return forwardBack(updown(pos)) * vec3(1, 1, 1);
}

vector<vec3> genLineArray(int width, int height, vector<vector<vec3>> points) {
	vector<vec3> lines;
	for (int x =0; x < width; x++) {
		for (int y = 0; y < height; y++) {
			// push back a line from (x, y) to (x-1, y)
			if (x > 0) {
				lines.push_back(points[x][y]);
				lines.push_back(points[x - 1][y]);
			}

			// push back a line from (x, y) to (x, y-1)
			if (y > 0) {
				lines.push_back(points[x][y]);
				lines.push_back(points[x][y - 1]);
			}

		}
	}

	return lines;
}

vector<vec3> genTriArray(int width, int height, vector<vector<vec3>> points) {
	vector<vec3> tris;
	for (int x = 0; x < width; x++) {
		for (int y = 0; y < height; y++) {
			if (x > 0 && y > 0) {
				tris.push_back(points[x][y]);
				tris.push_back(points[x - 1][y]);
				tris.push_back(points[x][y - 1]);
			}

			if (x < (width - 1) && y < (height - 1)) {
				tris.push_back(points[x][y]);
				tris.push_back(points[x + 1][y]);
				tris.push_back(points[x][y + 1]);
			}
		}
	}

	return tris;
}

GLuint genVAO(vector<vec3> lines) {
	GLuint vaoid;
	glGenVertexArrays(1, &vaoid);
	glBindVertexArray(vaoid);

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, lines.size() * sizeof(vec3), &lines[0][0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0
	);

	glBindVertexArray(0);

	return vaoid;
}

int main(void)
{
	/* init */
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(1366, 705, "ART", NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window.");
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	glewExperimental = true;
	if (glewInit() != GLEW_OK) {
		glfwTerminate();
		return -1;
	}

	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	//glDepthMask(GL_TRUE);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);

	vector<vector<vec3>> points = genPointArray(WIDTH, HEIGHT);
	points = displacePoints(points, cameraPlane);
	vec3 center = findAverPos(points);
	vector<vec3> lines = genTriArray(WIDTH, HEIGHT, points);
	GLuint vao = genVAO(lines);
	glBindVertexArray(vao);

	GLuint programID = LoadShaders("vert.glsl", "frag.glsl");
	GLint mvp = glGetUniformLocation(programID, "mvp");
	GLint color = glGetUniformLocation(programID, "color");

	do {
		float radius = 2.5;
		vec3 pos = vec3(sin(glfwGetTime() / 10) * radius, center.y + 2, cos(glfwGetTime() / 10) * radius);
		pos = vec3(0);
		mat4 mat = lookAt(pos, center, vec3(0, 1, 0));
		mat = perspective(radians(50.0f), float(1366)/705, 0.125f, 8.0f) * mat;

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(programID);
		glUniformMatrix4fv(mvp, 1, GL_FALSE, &mat[0][0]);

		glUniform3fv(color, 1, &vec3(0, 1, 0)[0]);
		glDrawArrays(GL_TRIANGLES, 0, lines.size());

		glfwSwapBuffers(window);
		glfwPollEvents();

	} while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);

	glDeleteProgram(programID);

	glfwTerminate();

	return 0;
}