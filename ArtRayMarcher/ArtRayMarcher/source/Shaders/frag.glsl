#version 430 core
#define MAX 250

in vec3 pos;
out vec3 color;

uniform float time;
uniform mat4 rot;
uniform vec3 posoff;

float dt = 0.001;
float force = 10;

int tris = 1;

layout(std430, binding = 0) buffer vertexLayout {
	float vd[];
};

struct Sphere {
	vec3 p;
	float r;
};

struct Photon {
	vec3 pos;
	vec3 vel;
};

Photon stepPhoton(Photon photon) {
	vec3 diff = vec3(0, 0, 10) - photon.pos;
	vec3 force = diff / pow(distance(vec3(0, 0, 10), photon.pos), 2) * dt * force;
	photon.vel += force;
	photon.pos += photon.vel;
	return photon;
}

Sphere index(int i) {
	int o = i * 4;
	return Sphere(
		vec3(vd[o], vd[o+1], vd[o+2]),
		vd[o+3]
	);
}

float sdSphere(vec3 pos, Sphere s) {
	return distance(pos, s.p) - s.r;
}

float DE(vec3 pos) {
	float d = 1e20;
	for (int i = 0; i < tris; i++) {
		float d_ = sdSphere(pos, index(i));
		if (d_ < d) {
			d = d_;
		}
	}
	return d;
}

struct Result {
	bool collided;
	vec3 pos;
};

Result failure() {
	return Result(false, vec3(0));
}

Result success(vec3 pos) {
	return Result(true, pos);
}

vec3 search(Photon p) {
	vec3 moveDir = -0.01*p.vel;
	vec3 pos = p.pos;

	for (int i = 0; i < 100; i++) {
		pos += moveDir;
		bool nowInside = DE(pos) < 1;
		if (!nowInside) {
			return pos;
		}
	}
	return pos;
}

Result marchRay(Photon photon) {
	for (int i = 0; i < MAX; i++) {
		photon = stepPhoton(photon);
		if (DE(photon.pos) < 0.1) {
			return success(search(photon));
		}
	}
	return failure();
}

void main() {
	vec3 ppos = pos;
	vec3 vel = normalize(vec3(ppos.x, ppos.y, 1)) * dt;
	vel = (vec4(vel, 0) * rot).xyz;

	Photon p = Photon(ppos + posoff, vel);
	
	if (DE(p.pos) < 0.1) {
		color = vec3(0);
		return;
	}

	Result r = marchRay(p);

	if (r.collided) {
		color = vec3(length(r.pos - p.pos) / 10);
	} else {
		color = normalize(vel);
	}
}