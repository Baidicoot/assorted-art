#include <glm/glm.hpp>

#ifndef SPHERE
#define SPHERE

struct Sphere {
	glm::vec4 position; //x, y, z, radius
	glm::vec4 color; //r, g, b, a

	Sphere translate(glm::vec3 amount);
};

#endif

void arraysFromObjects(Sphere objs[], const unsigned int nobj, glm::vec4 positions[], glm::vec4 colors[]);