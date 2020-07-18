#version 330 core
out vec4 fragColour;

in vec2 TexCoords;

uniform sampler2D texSampler;
uniform vec3 textColour;
//tex??

void main(){
	fragColour = vec4(textColour, texture(texSampler, TexCoords).r);
}