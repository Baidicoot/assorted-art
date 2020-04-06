#version 430 core

layout(location = 0) in vec3 pos_IN;
layout(binding = 0) buffer positionBuffer {
	vec3 positions[];
};

void main()
{
    positions[gl_VertexID] = pos_IN;
}