#version 430 core
#define MAX 256

layout(location = 0) in vec3 vertexpos;

uniform float aspect;

out vec3 pos;

void main() {
	gl_Position = vec4(vertexpos, 1);
	pos = vertexpos * vec3(1, aspect, 0);
}