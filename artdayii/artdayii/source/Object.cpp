#include <glm/glm.hpp>
using namespace glm;

#include "Object.h"

Sphere Sphere::translate(vec3 amount) {
	return Sphere{
		position + vec4(amount, 0.0),
		color,
	};
}

void arraysFromObjects(Sphere objs[], const unsigned int nobj, vec4 *positions, vec4 *colors) {
	for (int i = 0; i < nobj; i++) {
		positions[i] = objs[i].position;
		colors[i] = objs[i].color;
	}
}