#version 330 core

uniform float dist;
uniform float aspect;
uniform vec2 res;
uniform vec3 pos;
uniform vec3 facing;
uniform vec2 fov;

in vec2 uv;
out vec3 color;

float maxDist = dist;
float minDist = dist * 0.99;

mat3 yaw(float a) {
	return mat3(
		cos(a), -sin(a), 0,
		sin(a), cos(a), 0,
		0, 0, 1
	);
}

mat3 pitch(float b) {
	return mat3(
		cos(b), 0, sin(b),
		0, 1, 0,
		-sin(b), 0, cos(b)
	);
}

mat3 roll(float c) {
	return mat3(
		1, 0, 0,
		0, cos(c), -sin(c),
		0, sin(c), cos(c)
	);
}

float sum(vec2 len) {
	return len.x + len.y;
}

vec3 posRay() {
	float px = (2 * ((gl_FragCoord.x + 0.5) / res.x) - 1) * tan(fov.x);
	float py = (2 * ((gl_FragCoord.y + 0.5) / res.y) - 1) * tan(fov.y);

	vec3 dir = normalize(vec3(px, py, 1));

	return pos + (dir * yaw(facing.z) * pitch(facing.x) * roll(facing.y)) * dist;
}

vec3 dirRay() {
	float px = (2 * ((gl_FragCoord.x + 0.5) / res.x) - 1) * tan(fov.x);
	float py = (2 * ((gl_FragCoord.y + 0.5) / res.y) - 1) * tan(fov.y);

	vec3 dir = normalize(vec3(px, py, 1));

	return (dir * yaw(facing.z) * pitch(facing.x) * roll(facing.y));
}

float getDist(vec3 pos) {
	return distance(mod(pos, 1), vec3(0.5)) - 0.0625;
}

bool objRay(vec3 dir, vec3 start) {
	float cdist = 0;
	//int i = 0;

	while (cdist < maxDist) {
		float mdist = getDist(start + cdist * dir);
		if (mdist < 0.001) {
			if (cdist > minDist) {
				return true;
			}

			mdist += 0.1;
		}
		cdist += mdist;
	}

	return false;
}

vec3 findChunkStart(vec3 pos) {
	return pos - mod(pos, 1);
}

bool chunkPositive(vec3 pos) {
	vec3 chunkStart = findChunkStart(pos);
	float sum = floor(chunkStart.x + chunkStart.y + chunkStart.z);
	
	return (mod(sum, 2) == 0);
}

vec3 diff(vec3 from, vec3 to) {
	return to - from;
}

vec3 getDirection(vec3 pos, vec3 s) {
	vec3 start = findChunkStart(s);
	bool positive = chunkPositive(start);

	if (positive) {
		float a0 = 1 / pow(distance(pos, start + vec3(0, 0, 0)), 2);
		float a1 = -1 / pow(distance(pos, start + vec3(0, 0, 1)), 2);
		float a2 = -1 / pow(distance(pos, start + vec3(0, 1, 0)), 2);
		float a3 = 1 / pow(distance(pos, start + vec3(0, 1, 1)), 2);
		float a4 = -1 / pow(distance(pos, start + vec3(1, 0, 0)), 2);
		float a5 = 1 / pow(distance(pos, start + vec3(1, 0, 1)), 2);
		float a6 = 1 / pow(distance(pos, start + vec3(1, 1, 0)), 2);
		float a7 = -1 / pow(distance(pos, start + vec3(1, 1, 1)), 2);

		return (a0 * diff(pos, start + vec3(0, 0, 0)))
			+ (a1 * diff(pos, start + vec3(0, 0, 1)))
			+ (a2 * diff(pos, start + vec3(0, 1, 0)))
			+ (a3 * diff(pos, start + vec3(0, 1, 1)))
			+ (a4 * diff(pos, start + vec3(1, 0, 0)))
			+ (a5 * diff(pos, start + vec3(1, 0, 1)))
			+ (a6 * diff(pos, start + vec3(1, 1, 0)))
			+ (a7 * diff(pos, start + vec3(1, 1, 1)));
	} else {
		float a0 = -1 / pow(distance(pos, start + vec3(0, 0, 0)), 2);
		float a1 = 1 / pow(distance(pos, start + vec3(0, 0, 1)), 2);
		float a2 = 1 / pow(distance(pos, start + vec3(0, 1, 0)), 2);
		float a3 = -1 / pow(distance(pos, start + vec3(0, 1, 1)), 2);
		float a4 = 1 / pow(distance(pos, start + vec3(1, 0, 0)), 2);
		float a5 = -1 / pow(distance(pos, start + vec3(1, 0, 1)), 2);
		float a6 = -1 / pow(distance(pos, start + vec3(1, 1, 0)), 2);
		float a7 = 1 / pow(distance(pos, start + vec3(1, 1, 1)), 2);

		return (a0 * diff(pos, start + vec3(0, 0, 0)))
			+ (a1 * diff(pos, start + vec3(0, 0, 1)))
			+ (a2 * diff(pos, start + vec3(0, 1, 0)))
			+ (a3 * diff(pos, start + vec3(0, 1, 1)))
			+ (a4 * diff(pos, start + vec3(1, 0, 0)))
			+ (a5 * diff(pos, start + vec3(1, 0, 1)))
			+ (a6 * diff(pos, start + vec3(1, 1, 0)))
			+ (a7 * diff(pos, start + vec3(1, 1, 1)));
	}
}

vec3 getRGB(float hue) {
    float c = 1;
    float x = 1 * (1 - abs(mod(hue / 60, 2 - 1)));
    if (hue < 60)
        return vec3(c, x, 0);
    if (hue < 120)
        return vec3(x, c, 0);
    if (hue < 180)
        return vec3(0, c, x);
    if (hue < 240)
        return vec3(0, x, c);
    if (hue < 300)
        return vec3(x, 0, c);
    return vec3(c, 0, x);
}

void main(){
	vec3 end = posRay();
	vec3 d0 = normalize(getDirection(end, end));

	color = d0;
}