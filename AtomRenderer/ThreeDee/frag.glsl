#version 330 core

const int N_OBJECTS = 1;

uniform vec4 positions[N_OBJECTS];
uniform vec4 colors[N_OBJECTS];
uniform vec2 res;
uniform vec3 camera;
uniform vec4 lighting;
uniform vec3 facing;
uniform vec2 fov;

out vec3 color;

vec3 defaultColor = vec3(0);
int maxIter = 250;
int maxDist = 250;
bool shadows = false;

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

float sdTorus( vec3 p, vec2 t )
{
  vec2 q = vec2(length(p.xz)-t.x,p.y);
  return length(q)-t.y;
}

float sdSphere(vec3 p, float r)
{
  return length(p) - r;
}

float getMaxDist(vec4 pos, vec3 point) {
	return sdSphere(pos.xyz - mod(point, 20), pos.w);
}

bool checkIntersection(vec4 space, vec3 point, float closeEnough) {
	return getMaxDist(space, point) <= closeEnough;
}

float getMaxDist(vec3 point) {
	float minimum = getMaxDist(positions[0], point);

	for (int i = 1; i < N_OBJECTS; i++) {
		minimum = min(minimum, getMaxDist(positions[i], point));
	}

	return minimum;
}

int checkForIntersections(vec4 space[N_OBJECTS], vec3 point, float closeEnough) {
	for (int i = 0; i < N_OBJECTS; i++) {
		if (checkIntersection(space[i], point, closeEnough)) {
			return i;
		}
	}
	return -1;
}

float shadowRay(vec3 ro, vec3 rd, float mint, float maxt, float hitDist, float k) {
	float res = 1.0;
    float t = mint;
    for( int i=0; i<256; i++ )
    {
		float h = getMaxDist( ro + rd*t );
        res = min( res, k*h/t );
        t += clamp( h, 0.05, 0.25 );
        if( res<hitDist || t>maxt ) break;
    }
    return clamp( res * 2 - 1.5, -1.0, 0.0 );
}

vec3 getNormal(vec3 pos, float off) {
	vec3 xDir = vec3(off, 0, 0);
	vec3 yDir = vec3(0, off, 0);
	vec3 zDir = vec3(0, 0, off);
	return normalize(vec3(getMaxDist(pos+xDir)-getMaxDist(pos-xDir),
		                getMaxDist(pos+yDir)-getMaxDist(pos-yDir),
		                getMaxDist(pos+zDir)-getMaxDist(pos-zDir)));
}

vec3 getColor(vec3 direction, vec3 start, float maxDist, float hitDist, float occlusion) { //direction must be normalized
	float dist = 0;
	int i = 0;

	while (dist < maxDist && i < maxIter) {
		i++;
		int intersection = checkForIntersections(positions, start + dist*direction, hitDist);
		if (intersection != -1) {
			if (shadows) {
				vec3 pos = start + dist*direction;
				vec3 diff = lighting.xyz - pos;
				vec3 lightDir = normalize(diff);
				float shadow = shadowRay(pos, lightDir, 0.05, length(diff), hitDist, occlusion);

				pos = pos / 4;
				return vec3(sin(pos.x), cos(pos.y), sin(pos.z)) + vec3(1) * shadow;
			} else {
				vec3 pos = start + dist*direction;
				vec3 diff = lighting.xyz - pos;
				vec3 viewDir = normalize(-direction);
				vec3 lightDir = normalize(diff);
				vec3 halfway = normalize(viewDir + lightDir);
				vec3 normal = getNormal(pos, hitDist);
				float spec = pow(max(dot(normal, halfway), 0.0), 0.5);

				pos = pos / 4;
				return vec3(sin(pos.x), cos(pos.y), sin(pos.z)) * clamp(2500 / pow(dist, 2), 0, 1) * spec;
			}
		} else {
			dist += getMaxDist(start + dist*direction);
		}
	}

	return defaultColor;// + vec3(0.5) * direction.y;
}

void main(){
	float maximum, minimum;
	if (res.x > res.y) {
		maximum = res.x;
		minimum = res.y;
	} else {
		maximum = res.y;
		minimum = res.x;
	}

	float aspectRatio = res.x / res.y;

	float px = (2 * ((gl_FragCoord.x + 0.5) / res.x) - 1) * tan(fov.x) * aspectRatio;
	float py = (2 * ((gl_FragCoord.y + 0.5) / res.y) - 1) * tan(fov.y);

	vec3 relDirection = normalize(vec3(px, py, 1));

	vec3 direction = relDirection * yaw(facing.z) * pitch(facing.x) * roll(facing.y);

	color = getColor(direction, camera, maxDist, 0.001, 4);
}