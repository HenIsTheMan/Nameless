#version 330 core
layout (location = 0) out vec3 pos;
layout (location = 1) out vec3 normal;
layout (location = 2) out vec4 albedoSpec;

in myInterface{
	vec4 worldSpacePos;
	vec4 colour;
	vec2 texCoords;
	vec3 normal;
	vec3 tangent;
	flat int texIndex;
} fsIn;

uniform sampler2D diffuseMaps[28];
uniform sampler2D specMap;
uniform sampler2D emissionMap;
uniform sampler2D reflectionMap;
uniform sampler2D bumpMap;

uniform bool useSpecMap;
uniform bool useEmissionMap;
uniform bool useReflectionMap;
uniform bool useBumpMap;

void main(){
    pos = fsIn.worldSpacePos.xyz;
    normal = normalize(fsIn.normal);
	albedoSpec = fsIn.texIndex < 0 ? vec4(fsIn.colour) : vec4(texture(diffuseMaps[fsIn.texIndex], fsIn.texCoords).rgb, texture(specMap, fsIn.texCoords).r);
}