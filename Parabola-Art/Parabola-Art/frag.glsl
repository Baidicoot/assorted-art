#version 330 core

out vec4 fragcol;

in vec3 pos;

uniform vec3 color;

void main(){
  fragcol = vec4((pos + 1) / 2, 1);
}