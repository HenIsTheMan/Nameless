#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec4 colour;
layout (location = 2) in vec2 texCoords;
layout (location = 3) in vec3 normal;
layout (location = 4) in vec3 tangent;
layout (location = 5) in int texIndex;

out myInterface{
	vec4 worldSpacePos;
	vec4 colour;
	vec2 texCoords;
	vec3 normal;
	vec3 tangent;
	flat int texIndex;
} vsOut;

uniform mat4 model;
uniform mat4 PV;

void main(){
	vsOut.worldSpacePos = model * vec4(pos, 1.f);
	vsOut.colour = colour;
	vsOut.texCoords = texCoords;
	vsOut.normal = normal;
	vsOut.tangent = tangent;
	vsOut.texIndex = texIndex;
	gl_Position = PV * vsOut.worldSpacePos;
}