#pragma once

#include "../../../Core.h"
#include "../../../Global/GlobalFuncs.h"
#include "Scene/Scene.h"

class App final{
public:
	enum struct FBO{
		GeoPass = 0,
		LightingPass,
		PingPong0,
		PingPong1,
		Amt
	};
	enum struct Tex{
		Pos = 0,
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
	bool Init();
	void Update();
	void PreRender() const;
	void Render();
	void PostRender() const;
private:
	bool InitOptions() const;
	bool fullscreen;
	float elapsedTime;
	float lastFrameTime;
	Scene scene;
	static const GLFWvidmode* mode;
	static GLFWwindow* win;

	uint FBORefIDs[(int)FBO::Amt];
	uint texRefIDs[(int)Tex::Amt];
	uint RBORefIDs[1];
};

template <class T>
inline T& operator++(T& myEnum){
	myEnum = T((int)myEnum + 1);
	return myEnum;
}