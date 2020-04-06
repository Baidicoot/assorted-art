#version 330 core

#ifdef GL_ES
precision mediump float;
#endif

layout(location = 0) in vec3 vertexPosition;
uniform float aspectRatio;
out vec2 pos;
out vec2 uv;

void main() {
    gl_Position = vec4(vertexPosition, 1.0);

	float x = vertexPosition.x * aspectRatio;

	pos = vec2(x, vertexPosition.y);
	uv = (vertexPosition.xy + 1) / 2;
}