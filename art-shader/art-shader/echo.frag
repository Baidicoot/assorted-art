#version 330 core

#ifdef GL_ES
precision mediump float;
#endif

uniform sampler2D state;
uniform float time;
in vec2 uv;

out vec4 color;

vec2 rotate(vec2 v, float a) {
	float s = sin(a);
	float c = cos(a);
	mat2 m = mat2(c, -s, s, c);
	return m * v;
}

void main() {
	vec2 normUV = uv - vec2(0.5);
	vec2 rotUV = rotate(normUV, time / 100) + vec2(0.5);
	
	float d = 0.001;
	vec4 ul = texture2D(state, rotUV + vec2(-d, -d));
	vec4 um = texture2D(state, rotUV + vec2(0, -d)) * 2;
	vec4 ur = texture2D(state, rotUV + vec2(d, -d));
	vec4 ml = texture2D(state, rotUV + vec2(-d, 0)) * 2;
	vec4 mm = texture2D(state, rotUV + vec2(0, 0)) * 4;
	vec4 mr = texture2D(state, rotUV + vec2(d, 0)) * 2;
	vec4 bl = texture2D(state, rotUV + vec2(-d, d));
	vec4 bm = texture2D(state, rotUV + vec2(0, d)) * 2;
	vec4 br = texture2D(state, rotUV + vec2(d, d));
	
	color = mm / 8;
	//color = (ul + um + ur + ml + mm + mr + bl + bm + br) / 16;
}