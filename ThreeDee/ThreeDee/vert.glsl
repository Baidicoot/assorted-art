#version 330 core

layout(location = 0) in vec3 vertexPos;
uniform vec2 resolution;

void main() {
	gl_Position = vec4(vertexPos, 1.0);
}