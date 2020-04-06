#version 430

in vec4 apparentpos;
in vec4 worldpos;
in vec2 uv;
in vec4 normal;

//uniform int drawID;
uniform sampler2D tex;

layout(location = 0) out vec4 buff;

uniform vec3 lightPos;
uniform vec3 cameraPos;

void main(){
	vec3 lightDir = normalize(lightPos - worldpos.xyz);

	float diff = dot(lightDir, normal.xyz);
	float spec = dot(normalize(cameraPos - worldpos.xyz), reflect(-lightDir, normal.xyz));

	buff = texture(tex, uv) * (spec + diff);
}