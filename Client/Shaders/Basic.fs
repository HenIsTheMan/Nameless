#version 330 core
out vec4 fragColour;

in myInterface{
	vec4 colour;
	vec2 texCoords;
	vec3 normal;
	vec3 tangent;
	float process;
} fsIn;

uniform sampler2D texSamplers[32];

void main(){
	switch(int(fsIn.process)){
		case 0:
			fragColour = fsIn.colour;
			break;
		case 1:
			fragColour = texture(texSamplers[0], fsIn.texCoords);
			break;
	}
}