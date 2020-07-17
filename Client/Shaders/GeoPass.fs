#version 330 core
layout (location = 0) out vec3 pos;
layout (location = 1) out vec4 colour;
layout (location = 2) out vec3 normal;
layout (location = 3) out vec3 spec;
layout (location = 4) out vec3 reflection;

in myInterface{
	vec3 pos;
	vec4 colour;
	vec2 texCoords;
	vec3 normal;
	flat int diffuseTexIndex;
} fsIn;

///Can be set by client
uniform bool useCustomDiffuseTexIndex;
uniform bool useCustomColour;
uniform bool useCustomAlpha;
uniform float customAlpha;
uniform int customDiffuseTexIndex;
uniform vec4 customColour;

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

	if(useDiffuseMap){
		colour = texture(diffuseMaps[useCustomDiffuseTexIndex ? customDiffuseTexIndex : fsIn.diffuseTexIndex], fsIn.texCoords);
		if(useCustomAlpha){
			colour.a = customAlpha;
		}
		if(useEmissionMap){
			colour.rgb += texture(emissionMap, fsIn.texCoords).rgb;
		}
	} else{
		colour = useCustomColour ? customColour : fsIn.colour;
	}

	normal = fsIn.normal;
	spec = useSpecMap ? texture(specMap, fsIn.texCoords).rgb : vec3(0.f);
	reflection = useReflectionMap ? texture(reflectionMap, fsIn.texCoords).rgb : vec3(1.f);
}