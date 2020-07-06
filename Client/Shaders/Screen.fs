#version 330 core
out vec4 fragColour;

in vec2 TexCoords;
uniform sampler2D texSampler;

void main(){
	fragColour = vec4(texture(texSampler, TexCoords).rgb, 1.f);
}