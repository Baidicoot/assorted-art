#include "Renderer.h"

using namespace glm;
using namespace std;

const ArrayAttrib MESH_attribs[3] = {
	ArrayAttrib{ //verts
		0,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		0
	},
	ArrayAttrib{ //uvs
		1,
		2,
		GL_FLOAT,
		GL_FALSE,
		0,
		0
	},
	ArrayAttrib{ //normals
		2,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		0
	},
};
const GLuint MESH_lengths[3] = {
	sizeof(vec3),
	sizeof(vec2),
	sizeof(vec3)
};

const ArrayAttrib SHADOW_attrib = ArrayAttrib{
	0,
	3,
	GL_FLOAT,
	GL_FALSE,
	0,
	0
};
const GLuint SHADOW_length = sizeof(vec3);

bool Renderer::init(GLuint w, GLuint h, GLuint maxVerts) {
	width = w;
	height = h;
	maxverts = maxVerts;

	frame = 1;

	/*Buffers*/

	meshbuffer.init((ArrayAttrib*)MESH_attribs, (GLuint*)MESH_lengths, maxverts);

	/*G-Buffers*/

	GLuint textures[1];
	int res = genRGBADepthFBO(width, height, fbo, 1, textures);
	if (res != 0) {
		cout << "Framebuffer not complete! Error: " << res << endl;
		return false;
	}

	depthbuffer = textures[0];

	/*Programs*/

	string vertCode = read("source/Rendering/Shaders/vert.glsl");
	string fragCode = read("source/Rendering/Shaders/frag.glsl");
	if (vertCode.empty() || fragCode.empty()) {
		cout << "Could not read from shader files!" << endl;
		return false;
	}

	rasterizer = loadProgram(vertCode.c_str(), fragCode.c_str());
	raster_uniforms = RasterizerUniforms{
		glGetUniformLocation(rasterizer, "mvp"),
		glGetUniformLocation(rasterizer, "tex"),
		glGetUniformLocation(rasterizer, "lightPos"),
		glGetUniformLocation(rasterizer, "cameraPos"),
	};

	return true;
}

GlobalMesh Renderer::writeObjectToBuffers(RenderMesh mesh) {
	void* dataLocs[3] = {
		&mesh.verts[0][0],
		&mesh.uvs[0][0],
		&mesh.normals[0][0]
	};
	return meshbuffer.append(dataLocs, mesh.verts.size());
}

void Renderer::beginDraws(vec3 cameraPos, vec3 lightPos) {
	lightpos = lightPos;
	camerapos = cameraPos;

	glEnable(GL_CULL_FACE);

	glUseProgram(rasterizer);

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glViewport(0, 0, width, height);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	meshbuffer.beginDraws();
}

void Renderer::drawObjectToBuffers(GlobalMesh id, mat4 mvp, GLuint tex) {
	glUniformMatrix4fv(raster_uniforms.mvp, 1, GL_FALSE, &mvp[0][0]);
	glUniform3fv(raster_uniforms.lightPos, 1, &lightpos[0]);
	glUniform3fv(raster_uniforms.cameraPos, 1, &camerapos[0]);

	bindTexture2D(0, tex);
	glUniform1i(raster_uniforms.tex, 0);

	meshbuffer.draw(GL_TRIANGLES, id);
}