#version 330 core

#ifdef GL_ES
precision mediump float;
#endif

uniform sampler2D state;
in vec2 uv;

out vec4 color;

void main() {
	vec4 sam = texture2D(state, uv);

	color = sam - 0.005;
}