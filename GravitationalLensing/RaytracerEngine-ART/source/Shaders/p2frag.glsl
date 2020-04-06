#version 430
#define AMBIENT 0.25

in vec2 uv;

uniform vec2 sspos;
uniform float strength;
uniform float dist;
uniform float aspect;
uniform float radius;

out vec3 color;

uniform sampler2D tex;

void main(){
	vec2 ssuv = sspos;

	vec2 offset = uv - ssuv;
	float uv_dist = length(offset*vec2(aspect, 1));
	float impactParam = uv_dist * dist;
	float distortion = strength / impactParam;

	offset = offset * (1 - distortion);
	offset += ssuv;

	color = texture(tex, offset).xyz;

	if (impactParam < radius) {
		color = vec3(0);
	}
}