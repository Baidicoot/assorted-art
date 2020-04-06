#version 430
#define EPSILON 1e-8

layout(local_size_x = 1, local_size_y = 1) in;

layout(binding = 0) uniform sampler2D depth;
layout(binding = 1) uniform sampler2D color;
//layout(binding = 2) uniform sampler2D norms;

layout(rgba32f, binding = 3) uniform image2D target;

layout(std430, binding = 0) buffer vertexLayout {
	float vertexData[]; //packing not working..?
};

uniform uvec2 dims;
uniform int ntris;
uniform float time;

uniform int frame;

float radius = 5;
float seed = time;

highp float rand()
{
    highp float a = 12.9898;
    highp float b = 78.233;
    highp float c = 43758.5453;
    highp float dt= dot(gl_GlobalInvocationID.xy, vec2(a, b));
    highp float sn= mod(dt,3.14) * time;
    return fract(sin(sn) * c);
}

struct Ray {
	vec3 origin;
	vec3 direction;
};

uniform vec3 lightPos;

bool IntersectTriangle_MT97(Ray ray, vec3 vert0, vec3 vert1, vec3 vert2,
    inout float t, inout float u, inout float v)
{
    vec3 edge1 = vert1 - vert0;
    vec3 edge2 = vert2 - vert0;

    vec3 pvec = cross(ray.direction, edge2);

    float det = dot(edge1, pvec);
    float inv_det = 1.0f / det;

    vec3 tvec = ray.origin - vert0;

    u = dot(tvec, pvec) * inv_det;
    vec3 qvec = cross(tvec, edge1);

    v = dot(ray.direction, qvec) * inv_det;
    if (v < 0.0 || u + v > 1.0f || u < 0.0 || u > 1.0f || det < EPSILON)
        return false;

    t = dot(edge2, qvec) * inv_det;
    return true;
}

bool shadowCast(Ray ray) {
	float t, u, v;
	for (int i = 0; i < ntris; i++) {
		int index = i * 9;
		if (IntersectTriangle_MT97(ray,
			vec3(vertexData[index], vertexData[index + 1], vertexData[index + 2]),
			vec3(vertexData[index + 3], vertexData[index + 4], vertexData[index + 5]),
			vec3(vertexData[index + 6], vertexData[index + 7], vertexData[index + 8]),
			t, u, v) && t > 0.001)
			return true;
	}
	return false;
}

void main() {
	ivec2 pixel = ivec2(gl_GlobalInvocationID.xy);
	vec2 uv = vec2(pixel) / dims;
	vec4 data = texture(color, uv);
	vec4 pos = texture(depth, uv);
	//vec4 outcol = vec4(data.xyz * data.a, 1);

	if (data.a <= 0) {
		imageStore(target, pixel, vec4(0, 0, 0, pos.w));
		return;
	}

	float oldCol = imageLoad(target, pixel).x;
	if (frame == 1) {
		oldCol = 1;
	}

	vec3 newLightPos = lightPos + normalize(vec3(rand(), rand(), rand())) * rand() * radius;

	Ray r;
	r.origin = pos.xyz;
	r.direction = normalize(newLightPos - r.origin);

	float thisShadow;

	if (shadowCast(r)) {
		thisShadow = 0;
	} else {
		thisShadow = 1;
	}

	float inv_frame = 1/float(frame);

	imageStore(target, pixel, vec4(
		vec3((oldCol * (1 - inv_frame)) + (thisShadow * inv_frame)),
		pos.w
	));
}