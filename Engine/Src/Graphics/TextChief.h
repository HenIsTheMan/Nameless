#pragma once
#include "../Core.h"
#include "../Global/GlobalFuncs.h"
#include "ShaderProg.h"

class TextChief final{
public:
	TextChief();
	~TextChief();

	void RenderText(ShaderProg& SP, const str& text, const float& x, const float& y, const float& scaleFactor, const glm::vec3& colour); //vec4 colour??
private:
	struct CharMetrics final{
		uint texRefID;
		glm::ivec2 size;
		glm::ivec2 bearing;
		uint advance;
	};

	FT_Library ft;
	FT_Face face;
	std::map<char, CharMetrics> allChars;
	uint VAO;
	uint VBO;

	bool Init();
};