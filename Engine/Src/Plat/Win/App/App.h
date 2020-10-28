#pragma once

#include "../../../Core.h"
#include "../../../Global/GlobalFuncs.h"
#include "Scene/Scene.h"

class App final{
public:
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

	App();
	~App();
	void Init();
	void Update();
	void PreRender() const;
	void Render();
	void PostRender() const;
private:
	bool fullscreen;

	float elapsedTime;
	float lastFrameTime;

	Scene* scene;
	static const GLFWvidmode* mode;
	static GLFWwindow* win;

	uint FBORefIDs[(int)FBO::Amt];
	uint texRefIDs[(int)Tex::Amt];
	uint RBORefIDs[1];

	bool InitOptions() const;
};