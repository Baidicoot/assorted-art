#version 330 core

#ifdef GL_ES
precision mediump float;
#endif

uniform sampler2D state;
uniform float time;
uniform vec2 tracer;
in vec2 pos;
in vec2 uv;

out vec4 color;

void main() {
	vec4 sam = texture2D(state, uv);

	if (distance(pos, tracer) < 0.002) {
		color = vec4((sin(pos.x) + 1) / 2, 0.0, (sin(pos.y) + 1) / 2, 1.0);
	} else {
		color = sam;
	}
}