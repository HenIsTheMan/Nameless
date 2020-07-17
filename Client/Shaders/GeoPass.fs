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

///Can be set by client
uniform bool useCustomColour;
uniform bool useCustomDiffuseTexIndex;
uniform vec4 customColour;
uniform int customDiffuseTexIndex;

//Opacity??

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
	if(!useDiffuseMap){
		albedoSpec = useCustomColour ? customColour : fsIn.colour;
	} else{
		albedoSpec = vec4(texture(diffuseMaps[useCustomDiffuseTexIndex ? customDiffuseTexIndex : fsIn.diffuseTexIndex], fsIn.texCoords).rgb, useSpecMap ? texture(specMap, fsIn.texCoords).r : 0.f);
		if(useEmissionMap){
			albedoSpec.rgb += texture(emissionMap, fsIn.texCoords).rgb;
		}
	}
}