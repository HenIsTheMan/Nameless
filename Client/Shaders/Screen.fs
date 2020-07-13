#version 330 core
out vec4 fragColour;

in vec2 TexCoords;
uniform sampler2D screenTexSampler;
uniform sampler2D blurTexSampler;

void main(){
	fragColour = vec4(texture(screenTexSampler, TexCoords).rgb, 1.f);
	fragColour.rgb += texture(blurTexSampler, TexCoords).rgb; //Additive blending
}