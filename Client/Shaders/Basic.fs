#version 330 core
out vec4 fragColour;

in myInterface{
	vec4 colour;
	vec2 texCoords;
	vec3 normal;
	vec3 tangent;
	flat int texIndex;
} fsIn;

uniform sampler2D texSamplers[32];

void main(){
	fragColour = fsIn.texIndex < 0 ? fsIn.colour : texture(texSamplers[fsIn.texIndex], fsIn.texCoords);
}