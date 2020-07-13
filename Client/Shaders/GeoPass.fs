#version 330 core
layout (location = 0) out vec3 pos;
layout (location = 1) out vec3 normal;
layout (location = 2) out vec4 albedoSpec; //Pass spec as vec3 or vec4 for coloured spec highlight

in myInterface{
	vec3 pos;
	vec4 colour;
	vec2 texCoords;
	vec3 normal;
	flat int diffuseTexIndex;
} fsIn;

uniform bool useDiffuseMap;
uniform bool useSpecMap;
uniform bool useEmissionMap;
uniform bool useReflectionMap;

uniform sampler2D diffuseMaps[28];
uniform sampler2D specMap;
uniform sampler2D emissionMap;
uniform sampler2D reflectionMap;

void main(){
    pos = fsIn.pos;
	normal = fsIn.normal;
	albedoSpec = !useDiffuseMap ? fsIn.colour : vec4(texture(diffuseMaps[fsIn.diffuseTexIndex], fsIn.texCoords).rgb
	+ (useEmissionMap ? texture(emissionMap, fsIn.texCoords).rgb : vec3(0.f)), useSpecMap ? texture(specMap, fsIn.texCoords).r : 0.f);
}