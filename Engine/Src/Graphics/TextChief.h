#pragma once
#include "../Core.h"
#include "../Global/GlobalFuncs.h"
#include "ShaderProg.h"

class TextChief final{
public:
	struct Character final{ //Metrics??
		uint texRefID;
		glm::ivec2   Size;       // Size of glyph
		glm::ivec2   Bearing;    // Offset from baseline to left/top of glyph
		uint Advance;    // Offset to advance to next glyph
	};

	TextChief();
	~TextChief();
	bool Init();
	void RenderText(ShaderProg& SP, const str& text, const float& x, const float& y, const float& scaleFactor, const glm::vec3& colour); //vec4 colour??
private:
	FT_Library ft;
	FT_Face face;
	std::map<char, Character> Characters;
	uint VAO;
	uint VBO;
};