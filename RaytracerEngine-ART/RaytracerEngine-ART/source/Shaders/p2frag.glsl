#version 430
#define AMBIENT 0.25

in vec2 uv;

out vec3 color;

uniform sampler2D shadows;
uniform sampler2D tex;
uniform sampler2D colors;

float offset = 0.00125;

const int nsamples = 1;

void main(){
	vec4 depthData = texture(tex, uv);
	vec4 colorData = texture(colors, uv);
	
	float buff = 0;
	float samples = 0;
	
	for(int y = 0; y < nsamples; y++) {
		vec4 shadowData = texture(shadows, uv + vec2(0, offset * y));
		float d = distance(shadowData.w, depthData.w);
		d = 1 / max(d*d, 0.0001);
		buff += shadowData.x * d;
		samples += d;
	}

	for(int x = 0; x < nsamples; x++) {
		vec4 shadowData = texture(shadows, uv + vec2(offset * x, 0));
		float d = distance(shadowData.w, depthData.w);
		d = 1 / max(d*d, 0.0001);
		buff += shadowData.x * d;
		samples += d;
	}

	color = max(AMBIENT, (buff / samples)) * colorData.xyz * max(AMBIENT, colorData.a);
}