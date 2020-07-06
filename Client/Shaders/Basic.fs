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

uniform sampler2D texSamplers[32];

void main(){
    pos = fsIn.worldSpacePos.xyz;
    normal = normalize(fsIn.normal);
	albedoSpec = vec4(texture(texSamplers[fsIn.texIndex], fsIn.texCoords).rgb, texture(texSamplers[fsIn.texIndex], fsIn.texCoords).r); //??
	//fragColour = fsIn.texIndex < 0 ? fsIn.colour : texture(texSamplers[fsIn.texIndex], fsIn.texCoords);
}