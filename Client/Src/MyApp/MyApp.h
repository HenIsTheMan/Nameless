#pragma once

#include "Global/GlobalFuncs.h"

class MyApp final{
public:
	MyApp();

	void InCtor();
	void InDtor();
	void Init();
	void Update(float dt);
	void Render();
private:
	enum struct FBO{
		GeoPass,
		LightingPass,
		PingPong0,
		PingPong1,
		Amt
	};

	enum struct Tex{
		Pos,
		Colours,
		Normals,
		Spec,
		Reflection,
		Lit,
		Bright,
		PingPong0,
		PingPong1,
		Amt
	};

	bool fullscreen;

	float elapsedTime;

	const GLFWvidmode* mode;
	GLFWwindow* win;

	uint FBORefIDs[(int)FBO::Amt];
	uint texRefIDs[(int)Tex::Amt];
	uint RBORefIDs[1];
};